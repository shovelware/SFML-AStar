//////////////////////////////////////////////////////////// 
// Headers 
//////////////////////////////////////////////////////////// 
#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#endif 
#pragma comment(lib,"opengl32.lib") 
#pragma comment(lib,"glu32.lib") 

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <list>

#include "Graph.hpp"

using std::cout;
using std::endl;
using std::pair;

typedef Graph<char, int> GraphType;
typedef GraphArc<char, int> Arc;
typedef GraphNode<char, int> Node;

typedef vector<Node*> Path;

//Heuristic map is a vector<startNode, vector<endNode, heuristic>>
typedef vector<char, vector<char, float>> HeurMap;

////////////////////////////////////////////////////////////
///Global Variables
//////////////////////////////////////////////////////////// 
const int screenW = 640;
const int screenH = 480;
const sf::Mouse mouse;
const sf::Keyboard keyboard;

//Most everything scales off nodeRadius
const int nodeRadius = 16;

//Drawing stuff
sf::Vector2f b(nodeRadius * 2, nodeRadius * 2);
sf::Font f;
sf::Text t;
sf::Color cBG, cNode, cExp, cArc, cWeight, cPathNode, cPathArc, cData, cG, cH, cHover, cStart, cEnd;
float fD, fH, fG, fW;
sf::Vector2f tOrigin;

//Start and end nodes
Node* nStart;
Node* nEnd;

//Bools for toggling drawing of certain objects
bool drawD = 1;
bool drawG = 1;
bool drawH = 1;
bool drawW = 1;

//Bools 
bool AStar = 0;

int menuItems = 5;
////////////////////////////////////////////////////////////
///Functions
//////////////////////////////////////////////////////////// 

void empty(Node * pNode) {}

void visit(Node * pNode) {
	cout << "\t" << "Visiting: " << pNode->data() << endl;
}

void trackback(Node * pNode) {
	cout << "\t" << "Trackback: " << pNode->data() << ", " << pNode->g() << endl;
}

void found(Node * pNode)
{
	cout << "\t" << "Found: " << pNode->data() << ", " << pNode->g() << endl;
}

void outputUCSPath(Path* p)
{
	Path::iterator vIter = p->begin();
	Path::iterator vEnd = p->end();
	cout << "Path from " << (*vIter)->data() << " to " << (*--p->end())->data() << endl;
	cout << endl;


	int lastCost = 0;
	//Using path to track back
	for (; vIter != vEnd; ++vIter) {
		cout << "\t" << "Node: " << (*vIter)->data() << " (" << (*vIter)->g() - lastCost << ")" << endl;
		lastCost = (*vIter)->g();
	}

	cout << endl;

	cout << "Path total cost: " << lastCost << endl << endl;

}

void outputUCSPathShort(Path* p)
{
	Path::iterator vStart = p->begin();
	Path::iterator vIter = p->begin();
	Path::iterator vLast = --p->end();
	Path::iterator vEnd = p->end();

	cout << "[" << (*vIter)->data() << "-" << (*--p->end())->data() << "]" << " [" << (*--p->end())->g() << "]" << endl;
	cout << "\t";

	int lastCost = 0;
	//Using path to track back
	for (; vIter != vEnd; ++vIter) {

		if (vIter != vStart)
		{
			cout << "(" << (*vIter)->g() - lastCost << ")-";
		}

		cout << (*vIter)->data();
		lastCost = (*vIter)->g();

		if (vIter != vLast)
		{
			cout << "-";
		}
	}
	cout << endl; //End single line path
}

void outputAstarPathShort(Path* p)
{
	Path::iterator vStart = p->begin();
	Path::iterator vIter = p->begin();
	Path::iterator vLast = --p->end();
	Path::iterator vEnd = p->end();

	cout << "[" << (*vIter)->data() << "-" << (*--p->end())->data() << "]" << " [" << (*--p->end())->g() << "]" << endl;
	cout << "\t";

	int lastCost = 0;
	//Using path to track back
	for (; vIter != vEnd; ++vIter) {

		if (vIter != vStart)
		{
			cout << "(" << (*vIter)->g() - lastCost << ")-";
		}

		cout << (*vIter)->data();
		lastCost = (*vIter)->g();

		if (vIter != vLast)
		{
			cout << "-";
		}
	}
}

void loadGraph(GraphType &g, string nodes, string arcs)
{
	//read nodes
	char c;

	int i = 0;
	ifstream myfile;

	myfile.open(nodes);

	while (myfile >> c) {
		g.addNode(c, i++);
	}

	myfile.close();

	//read arcs
	myfile.open(arcs);

	int n1, n2, weight;
	while (myfile >> n1 >> n2 >> weight) {
		g.addDualArc(n1, n2, weight);
	}

	myfile.close();
}

void loadGraphDrawable(GraphType & g, string nodes, string arcs)
{
	//read nodes
	char c;

	int i = 0;
	int x = 0;
	int y = 0;
	ifstream myfile;

	myfile.open(nodes);

	while (myfile >> c >> x >> y) {
		g.addNode(c, i);
		g.nodeArray()[i]->setPosition(sf::Vector2f(x, y));
		++i;
	}

	myfile.close();

	//read arcs
	myfile.open(arcs);

	int from, to, weight;
	while (myfile >> from >> to >> weight) {
		g.addDualArc(from, to, weight);
	}

	myfile.close();
}

float distanceBetween(const sf::Vector2f v1, const sf::Vector2f v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
}

sf::Vector2f midpoint(const sf::Vector2f v1, const sf::Vector2f v2)
{
	return sf::Vector2f((v2.x + v1.x) / 2.0, (v2.y + v1.y) / 2.0);
}

bool mouseOverNode(const sf::Vector2f position, const sf::RenderWindow &w, float radius)
{
	if (distanceBetween(position, sf::Vector2f(mouse.getPosition(w))) < radius)
		return true;

	else return false;
}

template <typename T>
string numToStr(T num)
{
	stringstream s;
	s << num;
	return s.str();
}

template <typename T>
string numToStr(string prefix, T num)
{
	stringstream s;
	s << prefix << num;
	return s.str();
}

bool nodeInPath(Node* n, Path* p )
{
	bool found = false;

	Path::iterator vIter = p->begin();
	Path::iterator vEnd = p->end();

	for (; vIter != vEnd; ++vIter)
	{
		if ((*vIter) == n)
			return true;
	}

	return found;
}

void drawPath(sf::RenderWindow & w, Path & p)
{
	//If we don't have a path to draw, exit
	if (p.empty())
		return;

	//Draw Arcs
	for (Path::iterator vIter = p.begin(), vEnd = p.end(); vIter != vEnd; ++vIter)
	{
		Node* tempNode = (*vIter);

		//If we have no previous, skip drawing
		if (tempNode->getPrev() == NULL)
			continue;

		sf::Vertex line[] =
		{
			sf::Vertex(tempNode->position() + b, cPathArc),
			sf::Vertex(tempNode->getPrev()->position() + b, cPathArc)
		};

		w.draw(line, 2, sf::Lines);
	}
}

void drawArcs(sf::RenderWindow & w, GraphType & const g)
{
	t.setFont(f);
	t.setOrigin(tOrigin);

	//Draw Arcs
	for (int n = 0, numNodes = g.count(); n < numNodes; ++n)
	{
		Node* const tempNode = g.nodeArray()[n];

		for (list<Arc>::const_iterator vIter = tempNode->arcList().begin(), vEnd = tempNode->arcList().end(); vIter != vEnd; ++vIter)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(tempNode->position() + b, cArc),
				sf::Vertex((*vIter).node()->position() + b, cArc)
			};

			w.draw(line, 2, sf::Lines);

			//Draw weight
			if (drawW)
			{
				sf::Vector2f wPos = midpoint(line[0].position, line[1].position);

				t.setCharacterSize(fW);
				t.setPosition(wPos);
				t.setString(numToStr((*vIter).weight()));
				t.setColor(cWeight);
				t.setOrigin(tOrigin);

				line[0] = sf::Vertex(wPos, cArc);
				line[1] = sf::Vertex(t.getPosition(), cArc);
				w.draw(line, 2, sf::Lines);

				w.draw(t);
			}
		}

	}
}

void drawNodes(sf::RenderWindow & const w, GraphType & const g, Path & p)
{
	sf::CircleShape circ;
	circ.setOrigin(nodeRadius, nodeRadius);
	circ.setRadius(nodeRadius);

	sf::Text t;
	t.setFont(f);
	t.setOrigin(tOrigin);

	for (int n = 0, numNodes = g.count(); n < numNodes; ++n)
	{
		//Pull out the node we're looking at
		Node* const tempNode = g.nodeArray()[n];
		circ.setPosition(tempNode->position() + b);

		//If our mouse is on the node
		if (mouseOverNode(tempNode->position() + b, w, nodeRadius))
		{
			//Make it bigger and draw it underneath with a nice colour
			circ.setOrigin(nodeRadius * 1.2, nodeRadius * 1.2);
			circ.setRadius(nodeRadius * 1.2);
			circ.setFillColor(cHover);
			w.draw(circ);
			//Reset it
			circ.setOrigin(nodeRadius, nodeRadius);
			circ.setRadius(nodeRadius);
		}

		//If we're in the path use path colour
		if (nodeInPath(tempNode, &p))
		{
			circ.setFillColor(cPathNode);
		}

		else if (tempNode->marked())
		{
			circ.setFillColor(cExp);
		}

		//Else use the default colour
		else circ.setFillColor(cNode);

		//If we're the start, colour differently
		if (nStart != NULL && tempNode == nStart)
			circ.setFillColor(cStart);

		//Or if we're the end
		else if (nEnd != NULL && tempNode == nEnd)
			circ.setFillColor(cEnd);

		//Finally, draw the node
		w.draw(circ);

		//Draw Data
		if (drawD)
		{
			t.setCharacterSize(fD);
			t.setPosition(tempNode->position() + b);
			t.setString(tempNode->data());
			t.setColor(cData);
			w.draw(t);
		}

		//Draw G
		if (drawG)
		{
			t.setCharacterSize(fG);
			t.setPosition((tempNode->position() + b) - sf::Vector2f(nodeRadius / 2, nodeRadius / 2));

			//Correct for max
			int g = tempNode->g();
			if (g >= INT_MAX)
			{
				t.setString("oo");
			}

			else t.setString(numToStr(g));

			t.setColor(cG);
			w.draw(t);
		}

		//Draw H
		if (drawH)
		{
			t.setCharacterSize(fH);
			t.setPosition((tempNode->position() + b) + sf::Vector2f(nodeRadius * 2, nodeRadius * 2));

			int h = tempNode->h();
			if (h >= (FLT_MAX - 2000.0))
			{
				t.setString("MAX");
			}

			else t.setString(numToStr(tempNode->h()));

			t.setColor(cH);
			w.draw(t);
		}



	}
}

void drawGraph(sf::RenderWindow & const w, GraphType & const g, Path* p)
{
	//Set up variables for re-use
	int numNodes = g.count();
	int n;

	//Draw Arcs
	drawArcs(w, g);

	//Draw Path
	drawPath(w, *p);

	//Draw Nodes
	drawNodes(w, g, *p);
}

void drawMenu(sf::RenderWindow & const w, sf::Vector2f pos)
{
	sf::Vector2f iPos(pos);
	sf::Vector2f space(nodeRadius, nodeRadius);

	sf::Text t;
	t.setFont(f);
	t.setOrigin(nodeRadius / 2, nodeRadius / 2);

	stringstream s;

	//D
	//s << "D: " << drawD << endl;
	//t.setString(s.str());
	//t.setPosition(pos + (space * 0.0));

	//G

	//H

	//W

	//A

}

bool setNode(GraphType &g, const sf::RenderWindow &w)
{
	bool action = false;
	int numNodes = g.count();
	
	for (int n = 0; n < numNodes; ++n)
	{
		Node* tempNode = g.nodeArray()[n];
	
		if (mouseOverNode(tempNode->position() + b, w, nodeRadius))
		{
			//If we hit the start, unmark it
			if (nStart == tempNode)
			{
				nStart = NULL;
				action == true;
			}

			//If we hit the end, unmark it
			else if (nEnd == tempNode)
			{
				nEnd = NULL;
				action == true;
			}

			//If there's no start, mark this one
			else if (nStart == NULL)
			{
				nStart = tempNode;
				action = true;
			}
	
			//If there's no end and it's not the start, mark this one
			else if (nEnd == NULL && tempNode != nStart)
			{
				nEnd = tempNode;
				action = true;
			}
		}
	}

	return action;
}

bool clearNode()
{
	bool action = false;

	if (nEnd != NULL)
	{
		nEnd = NULL;
		action = true;
	}

	else if (nStart != NULL)
	{
		nStart = NULL;
		action = true;
	}

	return action;
}

bool runUCS(GraphType & g, Node* n1, Node* n2, Path & p)
{
	if (n1 != NULL && n2 != NULL)
	{
		g.UCS(n1, n2, empty, p);
		return true;
	}

	else return false;
}

bool runUCSA(GraphType & g, Node* n1, Node * n2)
{
	if (n1 != NULL && n2 != NULL)
	{
		g.UCSA(n1, n2, empty);
		return true;
	}

	else return false;
}

bool runAStar(GraphType & g, Node* n1, Node* n2, Path & p)
{
	if (n1 != NULL && n2 != NULL)
	{
		g.AStar(n1, n2, empty, p);
		return true;
	}

	else return false;
}

void PrayThisWorks(GraphType & g, Path & p)
{
	int c = g.count() - 1;

	for (int i = 0; i < c; ++i)
	{
		for (int j = 0; j < c ; ++j)
		{
			if (i != j)
			{
				runUCS(g, g.nodeArray()[i], g.nodeArray()[j], p);
				cout << i << "->" << j << endl;
			}
		}
	}
}

////////////////////////////////////////////////////////////
///Entrypoint of application 
//////////////////////////////////////////////////////////// 
int main()
{
	// Create the main window 
	sf::RenderWindow window(sf::VideoMode(screenW, screenH, 32), "SFML A*");

	//Set up assets & drawing stuff
	f.loadFromFile("FORCED SQUARE.ttf");

	//Set up colours
	cBG = sf::Color(64, 64, 64, 255);
	cNode = sf::Color(66, 33, 99, 255);
	cExp = sf::Color(132, 66, 198, 255);
	cArc = sf::Color(196, 196, 196, 255);
	cWeight = sf::Color(196, 128, 128, 255);
	cPathNode = sf::Color(0, 255, 255, 255);
	cPathArc = sf::Color(255, 255, 0, 2555);
	cData = sf::Color(255, 255, 255, 255);
	cG = sf::Color(100, 200, 100, 255);
	cH = sf::Color(200, 200, 100, 255);
	cHover = sf::Color(250, 150, 50, 255);
	cStart = sf::Color(0, 255, 0, 255);
	cEnd = sf::Color(255, 0, 0, 255);

	//Set up font sizes & Origin
	fD = nodeRadius * 2;
	fG = nodeRadius;
	fH = nodeRadius;
	fW = nodeRadius;
	tOrigin = sf::Vector2f(nodeRadius / 2, nodeRadius + nodeRadius / 2);
	
	//Set up graph
	Graph<char, int> graph(30);
	loadGraphDrawable(graph, "AStarNodes.txt", "AStarArcs.txt");
	cout << endl;

	//Working (Any graph)
	//graph.breadthFirst(graph.nodeArray()[0], visit);

	//Not working? (Q2Nodes and Q2Arcs)
	//graph.breadthFirstPlus(graph.nodeArray()[0], graph.nodeArray()[15], visit);

	graph.setVerbosity(1);
	Path path;

	//UCS broke too fuck (j/k my graph was wrong (AStar Arcs and Nodes))
	//graph.UCS(graph.nodeArray()[0], graph.nodeArray()[17], visit, path);
	//outputUCSPathShort(&path);

	//Who the hell knows about A*

	// Start game loop 
	while (window.isOpen())
	{
		// Process events 
		sf::Event Event;
		while (window.pollEvent(Event))
		{
			// Close window : exit 
			if (Event.type == sf::Event::Closed)
				window.close();

			// Escape key : exit 
			if ((Event.type == sf::Event::KeyPressed) && (Event.key.code == sf::Keyboard::Escape))
				window.close();

		}

#pragma region INPUT

		//Input
		static bool lMouse;
		static bool rMouse;
		static bool mMouse;

		static bool kA;
		static bool kD;
		static bool kG;
		static bool kH;
		static bool kW;

		static bool kSpace;

		// Left Mouse: Set/unset node
		if (mouse.isButtonPressed(mouse.Left))
		{
			if (!lMouse)
				setNode(graph, window);
			lMouse = true;
		}

		else lMouse = false;
		
		// Middle Mouse : Clear everything
		if (mouse.isButtonPressed(mouse.Middle))
		{
			if (!mMouse)
			{
				nStart = NULL;
				nEnd = NULL;
				path.clear();
				graph.reset();
			}
			mMouse = true;
		}

		else mMouse = false;

		// Right Mouse : Run Pathfinding
		if (mouse.isButtonPressed(mouse.Right))
		{
			if (!rMouse)
			{
				if (nStart != NULL && nEnd != NULL)
				{
					if (AStar)
						//runUCSA(graph, nStart, nEnd);
						runAStar(graph, nStart, nEnd, path);

					else runUCS(graph, nStart, nEnd, path);
					//nStart = nEnd = NULL;
					//outputUCSPathShort(&path);
				}
			}

			rMouse = true;
		}

		else rMouse = false;


		// D : Toggle weight drawing
		if (keyboard.isKeyPressed(keyboard.A))
		{
			if (!kA)
			{
				AStar = !AStar;
			}

			kA = true;
		}

		else kA = false;

		// W : Toggle weight drawing
		if (keyboard.isKeyPressed(keyboard.W))
		{
			if (!kW)
			{
				drawW = !drawW;
			}

			kW = true;
		}

		else kW = false;


		// G : Toggle g drawing
		if (keyboard.isKeyPressed(keyboard.G))
		{
			if (!kG)
			{
				drawG = !drawG;
			}

			kG = true;
		}

		else kG = false;


		// H : Toggle h drawing
		if (keyboard.isKeyPressed(keyboard.H))
		{
			if (!kH)
			{
				drawH = !drawH;
			}

			kH = true;
		}

		else kH = false;


		// D : Toggle weight drawing
		if (keyboard.isKeyPressed(keyboard.D))
		{
			if (!kD)
			{
				drawD = !drawD;
			}

			kD = true;
		}

		else kD = false;

		// Space : Bump
		if (keyboard.isKeyPressed(keyboard.Space))
		{
			if (!kSpace)
			{
				PrayThisWorks(graph, path);
			}

			kSpace = true;
		}

		else kSpace = false;

#pragma endregion

		// Draw loop
		window.clear(cBG);
		
		drawGraph(window, graph, &path);

		//Pack up menu items

		//drawMenu();

		window.display();

	}

	return EXIT_SUCCESS;
}