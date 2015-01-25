#include <iostream>
#include <fstream>

#include "Graph.hpp"

#include <string>

using namespace std;
using std::pair;

typedef GraphArc<string, int> Arc;
typedef GraphNode<string, int> Node;

typedef vector<Node*> Path;
typedef vector<pair<string, int>> PairPath;
typedef vector<PairPath> PathMap;
/*
void empty(Node * pNode) {}

void visit( Node * pNode ) {
	cout << "Visiting: " << pNode->data() << endl;
}

void trackback(Node * pNode) {
	cout << "Trackback: " << pNode->data() << ", " << pNode->g() << endl;
}

void outputPath(Path* p)
{
	Path::iterator pathE = p->end();
	Path::iterator pathB = p->begin();
	pathE--;

	//Using path to track back
	for (; pathE != pathB; pathE--) {
		trackback(*pathE);
	}
	trackback(*pathB);
}

void outputPathPlus(Path* p)
{
	Path::iterator pathE = p->end();
	Path::iterator pathB = p->begin();
	pathE--;
	cout << "=====OPP" << endl;
	cout << "Path from " << (*pathE)->data() << " to " << (*pathB)->data() << endl;
	cout << endl;


	int lastCost = 0;
	//Using path to track back
	for (; pathE != pathB; pathE--) {
		cout << "Node: " << (*pathE)->data() << ", " << (*pathE)->g() - lastCost << endl;
		lastCost = (*pathE)->g();
	}
	cout << "Node: " << (*pathE)->data() << ", " << (*pathE)->g() - lastCost << endl;

	cout << endl;

	cout << "Path total cost: " << (*pathE)->g() << endl;

	cout << "=====" << endl;

}

void outputPathPlusShort(Path* p)
{
	Path::iterator pathE = p->end();
	Path::iterator pathB = p->begin();
	pathE--;
	cout << "=====OPPS" << endl;
	cout << "[" << (*pathE)->data() << "-" << (*pathB)->data() << "]" << " [" << (*pathB)->g() << "]" << endl;
	cout << ">";
	int lastCost = 0;
	//Using path to track back
	for (; pathE != pathB; pathE--) {
		cout << (*pathE)->data() << "(" << (*pathE)->g() - lastCost << ")->";
		lastCost = (*pathE)->g();
	}
	cout << (*pathE)->data() << "(" << (*pathE)->g() - lastCost << ")" << endl;

	cout << "=====" << endl;

}

void addPathToMap(PathMap& map, Path& p)
{
	//create a path of pairs
	vector<pair<string, int>> pairpath;

	Path::iterator pathE = p.end();
	Path::iterator pathB = p.begin();
	pathE--;

	int lastCost = 0;

	//Loop through path and add each node to the pair path
	for (; pathE != pathB; pathE--) {
		pairpath.push_back(pair<string, int>((*pathE)->data(),  (*pathE)->g() - lastCost));

		lastCost = (*pathE)->g();
	}
	//last node
	pairpath.push_back(pair<string, int>((*pathE)->data(), (*pathE)->g() - lastCost));

	map.push_back(pairpath);
}

void outputPairPath(PairPath *p)
{
	cout << "=====" << "PP" << endl;
	
	//each node and it's distance added to a stringstream, add up the total distance
	int i = 0;
	int m = p->size();

	std::stringstream s;
	int total = 0;

	for (; i < m; i++)
	{
		total += p->at(i).second;
		s << p->at(i).first << "(" << p->at(i).second << ")";

		if (i < m - 1)
			s << "->";
	}


	//Path begin to path end, total distance
	cout << "[" << p->begin()->first << "-" << (--p->end())->first << "]" << " [" << total << "]" << endl;
	//Actual path
	cout << s.str() << endl;

	cout << "=====" << endl << endl;
}

void loadGraph(Graph<string, int> &g, string nodes, string arcs)
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

	int from, to, weight;
	while (myfile >> from >> to >> weight) {
		g.addDualArc(from, to, weight);

	}
	myfile.close();
}

void loadGraphAStar(Graph<string, int> & g, string nodes, string arcs)
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
*/
/*
void drawNode(Node* n)
{

}
*/

int Graphmain(int argc, char *argv[]) {
	/*cout << "LAB 5 START\n" << "==========" << endl;

	//create graph <<name, weight(g)>, index>
    Graph<string, int> graph( 30 );
	
	loadGraphAStar(graph, "AStarNodes.txt", "AStarNode.txt");

	//=====//Breadth-First
	// Now traverse the graph.
	//graph.breadthFirst( graph.nodeArray()[0], visit );
	//graph.breadthFirstPlus(graph.nodeArray()[0], graph.nodeArray()[5], visit);

	//=====//UCS
	//cout << endl;
	//Max the distance in all nodes

	Path path;
	//graph.UCS(graph.nodeArray()[0], graph.nodeArray()[5], visit, path);

	//outputPathPlus(&path);

	//Using end node prev chain to track back
	//Node* pathNode = graph.nodeArray()[5];
	//while (pathNode->getPrev() != NULL)
	//{
	//	trackback(pathNode);
	//	pathNode = pathNode->getPrev();
	//}
	
	//=====//UCS Precomputation
	//cout << endl;

	PathMap map;

	
	//Iterate through map and calculate paths
	//outer, inner, maxNodes
	int o, n, m;
	
	//m = 5;
	m = 0;
	for (o = 0; o < m; o++)
	{
		for (n = o + 1; n <= m; n++)
		{
			path.clear();
			graph.UCS(graph.nodeArray()[o], graph.nodeArray()[n], empty, path);
			addPathToMap(map, path);
		}
	}

	//Output path
	PathMap::iterator pmI = map.begin();
	PathMap::iterator pmE = map.end();

	cout << "-----" << endl;
	for (; pmI != pmE; pmI++)
	{
		outputPairPath(&*pmI);
	}

	cout << "LAB 6 START\n" << "==========" << endl;

	graph.AStar(graph.nodeArray()[0], graph.nodeArray()[5], empty, path);

	outputPathPlusShort(&path);
	*/

	cout << "not indented" << endl;
	cout << "\t" << "indented once" << endl;
	cout << "\t\t" << "indented twice" << endl;
	cout << "\t\t\tindented thrice" << endl;
	system("PAUSE");
	return 0;
}
