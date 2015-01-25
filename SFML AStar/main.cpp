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

typedef Graph<string, int> GraphType;
typedef GraphArc<string, int> Arc;
typedef GraphNode<string, int> Node;

typedef vector<Node*> Path;
typedef vector<pair<string, int>> PairPath;
typedef vector<PairPath> PathMap;

////////////////////////////////////////////////////////////
///Global Variables
//////////////////////////////////////////////////////////// 
const int screenW = 640;
const int screenH = 480;
const sf::Mouse mouse;
const sf::Keyboard keyboard;

//Mouse click bools
bool lc, lp, rc, rp;

const int nodeRadius = 16;
sf::Vector2f b(32, 32);
sf::Font f;
sf::Text t;
sf::Color cBG, cNode, cArc, cWeight, cPathNode, cPathArc, cData, cG, cH, cHover, cStart, cEnd;
Node* nStart;
Node* nEnd;

bool drawG = false;
bool drawH = false;
bool drawW = false;

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
	string c;

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
	string c;

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
	return sf::Vector2f((v2.x - v1.x) / 2, (v2.y - v1.y) / 2);
}

bool mouseOverNode(const sf::Vector2f position, const sf::RenderWindow &w, float radius)
{
	if (distanceBetween(position, sf::Vector2f(mouse.getPosition(w))) < radius)
		return true;

	else return false;
}

void drawGraph(sf::RenderWindow & w, GraphType & g)
{
	int numNodes = g.count();
	int n;

	sf::CircleShape circ;
	circ.setOrigin(nodeRadius, nodeRadius);
	circ.setRadius(nodeRadius);

	sf::Text t;
	t.setFont(f);
	t.setOrigin(nodeRadius / 2, nodeRadius + nodeRadius / 2);

	//Draw Arcs
	for (n = 0; n < numNodes; ++n)
	{
		for (list<Arc>::const_iterator vIter = g.nodeArray()[n]->arcList().begin(), vEnd = g.nodeArray()[n]->arcList().end(); vIter != vEnd; ++vIter)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(g.nodeArray()[n]->position() + b, cArc),
				sf::Vertex((*vIter).node()->position() + b, cArc)
			};

			w.draw(line, 2, sf::Lines);

			//Draw weight
			if (drawW)
			{
				t.setCharacterSize(nodeRadius * 2);
				t.setPosition(midpoint(line[0].position, line[1].position));
				//Remove next line for testing what the hell is happening with strings
				t.setPosition(50, 10 * n);
				t.setString("" + (*vIter).weight());
				t.setColor(cWeight);
				w.draw(t);
			}
		}
		
	}

	//Draw Nodes
	for (n = 0; n < numNodes; ++n)
	{
		//Pull out the node we're looking at
		Node* tempNode = g.nodeArray()[n];
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
		
		//Use the default colour
		circ.setFillColor(cNode);

		//If we're the start, colour differently
		if (nStart != NULL && tempNode == nStart)
				circ.setFillColor(cStart);
		
		//Or if we're the end
		else if (nEnd != NULL && tempNode == nEnd)
			circ.setFillColor(cEnd);

		w.draw(circ);

		//Draw Data
		t.setCharacterSize(nodeRadius * 2);
		t.setPosition(tempNode->position() + b);
		t.setString(tempNode->data());
		t.setColor(cData);
		w.draw(t);

		//Draw G
		if (drawG)
		{
			t.setCharacterSize(nodeRadius);
			t.setPosition(tempNode->position() - sf::Vector2f(0, nodeRadius * 2));

			//The whole string stream is going fucken loopy
			int g = tempNode->g();
			if (g > 1000)
			{
				t.setString("MAX");
			}

			else t.setString("" + g);

			t.setColor(cG);
			w.draw(t);
		}

		//Draw H
		if (drawH)
		{
			t.setCharacterSize(nodeRadius);
			t.setPosition(tempNode->position() + sf::Vector2f(0, nodeRadius * 2));
			//t.setString(tempNode->h());
			t.setColor(cH);
			w.draw(t);
		}



	}


	//Draw its data

	//Draw its h and g values
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
			if (nStart == tempNode)
			{
				nStart = NULL;
				action == true;
			}

			else if (nEnd == tempNode)
			{
				nEnd = NULL;
				action == true;
			}

			else if (nStart == NULL)
			{
				nStart = tempNode;
				action = true;
			}
	
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

void drawPath(sf::RenderWindow & w, Path & p, sf::Color c)
{
	//for (Path::const_iterator)
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

	cBG = sf::Color(64, 64, 64, 255);
	cNode = sf::Color(66, 33, 99, 255);
	cArc = sf::Color(196, 196, 196, 255);
	cPathNode = sf::Color(0, 255, 255, 255);
	cPathArc = sf::Color(255, 255, 255);
	cData = sf::Color(255, 255, 255, 255);
	cG = sf::Color(200, 200, 200, 255);
	cH = sf::Color(200, 200, 200, 255);
	cHover = sf::Color(250, 150, 50, 255);
	cStart = sf::Color(0, 255, 0, 255);
	cEnd = sf::Color(255, 0, 0, 255);

	//Debug text I guess?
	t.setFont(f);
	t.setColor(sf::Color::White);
	t.setCharacterSize(30);
	t.setPosition(sf::Vector2f(400, 50));

	//Set up graph
	Graph<string, int> graph(30);
	loadGraphDrawable(graph, "AStarNodes.txt", "AStarArcs.txt");
	cout << endl;

	//Working (Any graph)
	//graph.breadthFirst(graph.nodeArray()[0], visit);

	//Not working? (Q2Nodes and Q2Arcs)
	//graph.breadthFirstPlus(graph.nodeArray()[0], graph.nodeArray()[15], visit);
	graph.setVerbosity(0);
	Path path;

	//UCS broke too fuck
	graph.UCS(graph.nodeArray()[0], graph.nodeArray()[29], visit, path);
	outputUCSPathShort(&path);

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

		//Input
		lp = lc;
		rp = rc;
		if (mouse.isButtonPressed(mouse.Left))
			lc = true;
		else lc = false;
		
		if (mouse.isButtonPressed(mouse.Right))
			rc = true;
		else rc = false;

		//Left release
		if (lp == true && lc == false)
		{
			setNode(graph, window);
		}

		//Right release
		if (rp == true && rc == false)
		{

		}

		// Draw loop
		window.clear(cBG);
		
		drawGraph(window, graph);

		window.display();

	}

	return EXIT_SUCCESS;
}