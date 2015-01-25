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

void drawGraph(sf::RenderWindow & w, GraphType & g, int b)
{
	int numNodes = g.count();
	sf::Vector2f border(b, b);
	sf::CircleShape circ;
	circ.setOrigin(16, 16);
	circ.setRadius(16);

	for (int n = 0; n < numNodes; ++n)
	{
		//Draw the node
		
		circ.setPosition(g.nodeArray()[n]->position() + border);
		w.draw(circ);

		//Draw its h and g values

		//Draw its arcs
		for (list<Arc>::const_iterator vIter = g.nodeArray()[n]->arcList().begin(), vEnd = g.nodeArray()[n]->arcList().end(); vIter != vEnd; ++vIter)
		{
			sf::Vertex line[] =
			{
				sf::Vertex(g.nodeArray()[n]->position() + border),
				sf::Vertex((*vIter).node()->position() + border)
			};

			w.draw(line, 2, sf::Lines);
		}
		
	}
}

void drawPath(sf::RenderWindow & w, Path & p, sf::Color c)
{

}

////////////////////////////////////////////////////////////
///Entrypoint of application 
//////////////////////////////////////////////////////////// 
int main()
{
	// Create the main window 
	sf::RenderWindow window(sf::VideoMode(screenW, screenH, 32), "SFML A*");

	Graph<string, int> graph(30);

	loadGraphDrawable(graph, "AStarNodes.txt", "AStarArcs.txt");
	cout << endl;

	//Working (Any graph)
	//graph.breadthFirst(graph.nodeArray()[0], visit);

	//Not working? (Q2Nodes and Q2Arcs)
	//graph.breadthFirstPlus(graph.nodeArray()[0], graph.nodeArray()[15], visit);
	graph.setVerbosity(0);
	Path path;

	//graph.UCS(graph.nodeArray()[0], graph.nodeArray()[29], visit, path);
	//outputUCSPathShort(&path);

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

		// Draw loop
		window.clear();
		
		drawGraph(window, graph, 32);

		window.display();

	}

	return EXIT_SUCCESS;
}