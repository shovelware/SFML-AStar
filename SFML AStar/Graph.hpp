#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>
#include <sstream>

#include <time.h>

#include <chrono>
#include <ctime>


using namespace std;

template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;
typedef vector<pair<char, int>> HeurVec;
typedef vector<pair<char, HeurVec>> HeurMap;

template<class NodeType, class ArcType>
class Graph {
private:
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;

    Node** m_pNodes; //An array of all the nodes in the graph.
    int m_maxNodes;
    int m_count;
	float m_heurMult; //Heuristic multiplier for A*

	//Output
	int m_verbosity = 1; //Verbosity for output
	stringstream gop; //Reusable stringstream for output
	void gout(int verbosity); //Output function

	//Map of heuristics for this graph
	HeurMap m_map; //Map will be empty by default
	float distanceBetween(const sf::Vector2f v1, const sf::Vector2f v2);
	float mapLookup(Node* pStart, Node* pEnd);

	//Maximum H and G values
	const int maxG = INT_MAX - 20000;
	const int maxH = INT_MAX - 20000;

	//Preparations
	void clearMarks();
	void clearPrevs();
	void maxGs();
	void maxHs();

public:           
    // Constructor and destructor functions
    Graph( int size );
    ~Graph();

	//Reset graph
	void reset();

    // Accessors
	Node** nodeArray() const { return m_pNodes; }
	float heurMult() { return m_heurMult; }
	int verbosity() { return verbosity; }
	int count() { return m_count; }
	bool hasMap() { return !m_map.empty(); }

	// Manipulators
	void setHeurMult(float HeurMult) { m_heurMult = HeurMult; }
	void setVerbosity(int verbosity) { m_verbosity = verbosity; }

	//Nodes
    bool addNode( NodeType data, int index );
    void removeNode(int index);
	void showNodes();

	//Arcs
	bool addArc(int from, int to, ArcType weight);
	void removeArc(int from, int to);
    Arc* getArc(int from, int to);

	//Dual Arcs
	bool addDualArc(int n1, int n2, ArcType weight);
	void removeDualArc(int n1, int n2);

	//Mapping
	void genMap();
	void mapNodes(Node* pEnd);

	//Graph exercises
    void depthFirst( Node* pNode, void (*pProcess)(Node*) );
	void breadthFirst(Node* pNode, void(*pProcess)(Node*));
	void breadthFirstPlus(Node* pNode, Node* pTarget, void(*pProcess)(Node*));
	void UCS(Node* pStart, Node* pTarget, std::vector<Node*>& path);
	void InitAStar(Node* pTarget);
	void AStar(Node* pStart, Node* pTarget, std::vector<Node*>& path);
	void AStarPrecomp(Node* pStart, Node* pTarget, std::vector<Node*>& path);
};

template<class NodeType, class ArcType>
class NodeSearchCostComparer {
public:
	bool operator()(GraphNode<NodeType, ArcType> * n1, GraphNode<NodeType, ArcType> * n2) {
		return n1->g() > n2->g();
	}
};

template<class NodeType, class ArcType>
class ANodeSearchCostComparer {
public:
	bool operator()(GraphNode<NodeType, ArcType> * n1, GraphNode<NodeType, ArcType> * n2) {
		return n1->g() + n1->h() > n2->g() + n2->h();
	}
};

template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::Graph( int size ) : m_maxNodes( size ), m_heurMult(0.9) {
	int i;
	m_pNodes = new Node * [m_maxNodes];
	// go through every index and clear it to null (0)
	for( i = 0; i < m_maxNodes; i++ ) {
		m_pNodes[i] = 0;
	}
	
	// set the node count to 0.
	m_count = 0;
	gop << "Constructing graph..." << endl;
	gout(3);
}

template<class NodeType, class ArcType>
Graph<NodeType, ArcType>::~Graph() {

	int index;
	for( index = 0; index < m_maxNodes; index++ ) {
		if( m_pNodes[index] != 0 ) {
			delete m_pNodes[index];
		}
	}
	// Delete the actual array
	delete m_pNodes;
	gop << "Deconstructing Graph..." << endl;
	gout(3);
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::reset()
{
	clearMarks();
	maxGs();
	maxHs();
}

template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addNode( NodeType data, int index ) {
   bool nodeNotPresent = false;
   // find out if a node does not exist at that index.
   if ( m_pNodes[index] == 0) {
		nodeNotPresent = true;
		// create a new node, put the data in it, and unmark it.
		m_pNodes[index] = new Node;
		m_pNodes[index]->setData(data);
		m_pNodes[index]->setMarked(false);

		gop << "\t" << "Adding node: " << data << endl;
		gout(3);
		// increase the count and return success.
		m_count++;
    }
    return nodeNotPresent;
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeNode( int index ) {

    // Only proceed if node does exist.
    if( m_pNodes[index] != 0 ) {

		// now find every arc that points to the node that
         // is being removed and remove it.
		int node;
		Arc* arc;
		
		// loop through every node
		for( node = 0; node < m_maxNodes; node++ ) {
		     // if the node is valid...
		     if( m_pNodes[node] != 0 ) {
		         // see if the node has an arc pointing to the current node.
		         arc = m_pNodes[node]->getArc( m_pNodes[index] );
		     }
		     // if it has an arc pointing to the current node, then
              // remove the arc.
		     if( arc != 0 ) {
		         removeArc( node, index );
		     }
		}
		gop << "\t" << "Removing node: " << m_pNodes[index]->data() << endl;
		gout(3);

		// now that every arc pointing to the current node has been removed,
        // the node can be deleted.
		delete m_pNodes[index];
		m_pNodes[index] = 0;
		m_count--;
		
    }
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::showNodes()
{
	gop << "Node List: " << endl;
	for (int i = 0; i < m_count; ++i)
	{
		gop << "\t" << m_pNodes[i]->data() << endl;
		
	}
	gop << "Total Nodes: " << m_count << endl << endl;
	gout(0);
}

template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addArc( int from, int to, ArcType weight ) {
     bool proceed = true; 
     // make sure both nodes exist.
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         proceed = false;
     }
        
     // if an arc already exists we should not proceed
     if( m_pNodes[from]->getArc( m_pNodes[to] ) != 0 ) {
         proceed = false;
     }

     if (proceed == true) {
        // add the arc to the "from" node.
        m_pNodes[from]->addArc( m_pNodes[to], weight );

		gop << "\t" << "Adding arc from " << m_pNodes[from]->data() << " to " << m_pNodes[to]->data() << " weight " << weight << endl;
		gout(3);
     }
        
     return proceed;
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeArc( int from, int to ) {
     // Make sure that the node exists before trying to remove
     // an arc from it.
     bool nodeExists = true;
     if( m_pNodes[from] == 0 || m_pNodes[to] == 0 ) {
         nodeExists = false;
     }

     if (nodeExists == true) {
        // remove the arc.
        m_pNodes[from]->removeArc( m_pNodes[to] );

		gop << "\t" << "Removing arc from " << m_pNodes[from]->data() << " to " << m_pNodes[to]->data() << endl;
		gout(3);
     }
}

template<class NodeType, class ArcType>
bool Graph<NodeType, ArcType>::addDualArc(int n1, int n2, ArcType weight) {
	bool proceed = true;
	// make sure both nodes exist.
	if (m_pNodes[n1] == 0 || m_pNodes[n2] == 0) {
		proceed = false;
	}

	// if an arc already exists we should not proceed
	if (m_pNodes[n1]->getArc(m_pNodes[n2]) != 0) {
		proceed = false;
	}

	if (proceed == true) {
		// add the arc to n1 and n2.
		m_pNodes[n1]->addArc(m_pNodes[n2], weight);
		m_pNodes[n2]->addArc(m_pNodes[n1], weight);

		gop << "\t" << "Adding dual arc between " << m_pNodes[n1]->data() << " and " << m_pNodes[n2]->data() << " weight " << weight << endl;
		gout(3);
	}

	return proceed;
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::removeDualArc(int n1, int n2) {
	// Make sure that the node exists before trying to remove
	// an arc from it.
	bool nodeExists = true;
	if (m_pNodes[n1] == 0 || m_pNodes[n2] == 0) {
		nodeExists = false;
	}

	if (nodeExists == true) {
		// remove the arc.
		m_pNodes[n1]->removeArc(m_pNodes[n2]);

		gop << "\t" << "Removing dual arc between " << m_pNodes[n1]->data() << " to " << m_pNodes[n2]->data() << endl;
		gout(3);
	}

}

template<class NodeType, class ArcType>
// Dev-CPP doesn't like Arc* as the (typedef'd) return type?
GraphArc<NodeType, ArcType>* Graph<NodeType, ArcType>::getArc( int from, int to ) {
     Arc* pArc = 0;
     // make sure the to and from nodes exist
     if( m_pNodes[from] != 0 && m_pNodes[to] != 0 ) {
         pArc = m_pNodes[from]->getArc( m_pNodes[to] );
     }
                
     return pArc;
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearMarks()
{
	for (int index = 0; index < m_maxNodes; ++index) {
		if (m_pNodes[index] != 0)
		{
			m_pNodes[index]->setMarked(false);

			gop << m_pNodes[index]->data() << " unmarked." << endl;
			gout(4);
		}
	}

	gop << "Unmarked all nodes" << endl;
	gout(2);
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::clearPrevs()
{
	
	for (int index = 0; index < m_maxNodes; ++index) {
		if (m_pNodes[index] != 0)
		{
			m_pNodes[index]->setPrev(NULL);

			gop << m_pNodes[index]->data() << " previous cleared." << endl;
			gout(4);
		}
	}

	gop << "All previous pointers cleared." << endl;
	gout(2);
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::maxGs()
{

	for (int index = 0; index < m_maxNodes; ++index) {
		if (m_pNodes[index] != 0)
		{
			m_pNodes[index]->setG(maxG);

			gop << m_pNodes[index]->data() << " G maxed." << endl;
			gout(4);
		}
	}

	gop << "All G values maxed." << endl;
	gout(2);
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::maxHs()
{

	for (int index = 0; index < m_maxNodes; ++index) {
		if (m_pNodes[index] != 0)
		{
			m_pNodes[index]->setH(maxH);

			gop << m_pNodes[index]->data() << " heuristic maxed." << endl;
			gout(4);
		}
	}

	gop << "All heuristic values maxed." << endl;
	gout(2);
}

template<class NodeType, class ArcType>
float Graph<NodeType, ArcType>::distanceBetween(const sf::Vector2f v1, const sf::Vector2f v2)
{
	return sqrt(pow(v2.x - v1.x, 2) + pow(v2.y - v1.y, 2));
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::genMap()
{
	HeurMap map;
	HeurVec temp;
	Node* nodeI;
	Node* nodeJ;

	int c = m_count;
	map.reserve(c);

	//For each node I
	for (int i = 0; i < c; ++i)
	{
		nodeI = m_pNodes[i];
		temp.clear();
		temp.reserve(c);
		
		//For each other node J (including I!)
		for (int j = 0; j < c; ++j)
		{
			nodeJ = m_pNodes[j];

			//Add J and the euclidian distance to it to the map
			temp.push_back(make_pair(nodeJ->data(), distanceBetween(nodeI->position(), nodeJ->position())));
		}

		//Then add I and the list of distances to the map
		map.push_back(make_pair(nodeI->data(), temp));
	}

	gop << "Heuristic map generated." << endl;
	gout(1);

	m_map = map;
}

template<class NodeType, class ArcType>
float Graph<NodeType, ArcType>::mapLookup(Node* pStart, Node* pEnd)
{
	//Distance will be maximum if not found
	float distance = maxH;

	//Find starting node in map
	for (HeurMap::iterator mIter = m_map.begin(), mEnd = m_map.end(); mIter < mEnd; ++mIter)
	{
		if (mIter->first == pStart->data())
		{
			//Find the second node in heuristic vector for first node
			for (HeurVec::iterator vIter = mIter->second.begin(), vEnd = mIter->second.end(); vIter < vEnd; ++vIter)
			{
				if (vIter->first == pEnd->data())
				{
					distance = vIter->second;
				}
			}
		}
	}

	return distance;
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::mapNodes(Node* pEnd)
{
	float distance;
	//lookup pEnd in the map, grab each distance and set it to the appropriate node
	for (int i = 0, c = m_count; i < c; ++i)
	{
		m_pNodes[i]->setH(mapLookup(pEnd, m_pNodes[i]));
	}
}

// ----------------------------------------------------------------
//  Name:           depthFirst
//  Description:    Performs a depth-first traversal on the specified 
//                  node.
//  Arguments:      The first argument is the starting node
//                  The second argument is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::depthFirst( Node* pNode, void (*pProcess)(Node*) ) {

	gop << "Depth First:" << endl;
	gout(1);
     if( pNode != 0 ) {
           // process the current node and mark it
           pProcess( pNode );
           pNode->setMarked(true);

           // go through each connecting node
           list<Arc>::iterator iter = pNode->arcList().begin();
           list<Arc>::iterator endIter = pNode->arcList().end();
        
		   for( ; iter != endIter; ++iter) {
			    // process the linked node if it isn't already marked.
                if ( (*iter).node()->marked() == false ) {
                   depthFirst( (*iter).node(), pProcess);
                }            
           }
     }

	 gop << endl;
	 gout(1);
}

// ----------------------------------------------------------------
//  Name:           breadthFirst
//  Description:    Performs a depth-first traversal the starting node
//                  specified as an input parameter.
//  Arguments:      The first parameter is the starting node
//                  The second parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirst( Node* pNode, void (*pProcess)(Node*) ) {
	gop << "Breadth First: " << endl;
	gout(1);
   if( pNode != 0 ) {
	  queue<Node*> nodeQueue;        
	  // place the first node on the queue, and mark it.
      nodeQueue.push( pNode );
      pNode->setMarked(true);

      // loop through the queue while there are nodes in it.
      while( nodeQueue.size() != 0 ) {
         // process the node at the front of the queue.
         pProcess( nodeQueue.front() );

         // add all of the child nodes that have not been 
         // marked into the queue
         list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
         list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();
         
		 for( ; iter != endIter; iter++ ) {
              if ( (*iter).node()->marked() == false) {
				 // mark the node and add it to the queue.
                 (*iter).node()->setMarked(true);
                 nodeQueue.push( (*iter).node() );
              }
         }

         // dequeue the current node.
         nodeQueue.pop();
      }
   }  
	gop << endl;
	gout(1);
}

// ----------------------------------------------------------------
//  Name:           breadthFirstPlus
//  Description:    Performs a depth-first traversal from the starting node
//                  to the target node specified as input parameters.
//  Arguments:      The first parameter is the starting node
//					The second parameter is the target node
//                  The third parameter is the processing function.
//  Return Value:   None.
// ----------------------------------------------------------------
template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::breadthFirstPlus(Node* pNode, Node* pTarget, void(*pProcess)(Node*)) {
	//Doubtful that this actually works, but I've already been marked on a previous iteration so whatevs
	gop << "Targeted Breadth First: " << endl;
	gout(1);

	if (pNode != 0) {
		queue<Node*> nodeQueue;
		bool found = false;
		// place the first node on the queue, and mark it.
		nodeQueue.push(pNode);
		pNode->setMarked(true);

		// loop through the queue while there are nodes in it.
		while (nodeQueue.size() != 0 && !found) {
			// process the node at the front of the queue.
			pProcess(nodeQueue.front());

			// add all of the child nodes that have not been 
			// marked into the queue
			list<Arc>::const_iterator iter = nodeQueue.front()->arcList().begin();
			list<Arc>::const_iterator endIter = nodeQueue.front()->arcList().end();

			for (; iter != endIter && !found; iter++) {
				//if the node is our target set found to true
				if ((*iter).node() == pTarget)
				{
					(*iter).node()->setPrev(nodeQueue.front());
					found = 1;
				}
				//else add it to the queue
				else if ((*iter).node()->marked() == false) {
					// mark the node and add it to the queue.
					(*iter).node()->setMarked(true);
					(*iter).node()->setPrev(nodeQueue.front());
					nodeQueue.push((*iter).node());
				}
			}

			// dequeue the current node.
			nodeQueue.pop();
			
		}
	}

	gop << endl;
	gout(1);
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::UCS(Node* pStart, Node* pTarget, std::vector<Node*>& path)
{
	gop << "\a=== UCS from " << pStart->data() << " to " << pTarget->data() << " ===" << endl;
	gout(2);

	//Start timer
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	//Unmark, clear Prev, max G, set up first node
	clearMarks();
	clearPrevs();
	maxGs();
	pStart->setG(0);
	pStart->setMarked(true);

	//make & set up queue
	priority_queue<Node*, vector<Node*>, NodeSearchCostComparer<NodeType, ArcType>> pq;
	
	//Start of UCS
	pq.push(pStart);
	
	//Priority Queueue loop
	while (!pq.empty() && pq.top() != pTarget)
	{

		gop << "TOP: " << pq.top()->data() << endl;

		//for each arc
		for (list<Arc>::const_iterator iter = pq.top()->arcList().begin(), endIter = pq.top()->arcList().end(); iter != endIter; ++iter)
		{
			//Pull out the node to test
			Node* childNode = iter->node();

			//if the previous node is not top of the queue
			if (childNode != pq.top()->getPrev())
			{
				//Get total weight of this route
				int c = pq.top()->g() + iter->weight();

				gop << "\t" << "Checking: " << pq.top()->data() << " -> " << childNode->data() << " [" << c << " < " << (childNode->g()) << "]" << endl;

				//if it's lower than the weight of the current route
				if (c < (childNode->g()))
				{
					//Set the node's internal weight to the arc from previous plus internal weight of previous
					childNode->setG(c);

					//Set previous pointer of the node to the previous node in the new path
					childNode->setPrev(pq.top());

					gop << "\t\t" << "True, " << childNode->data() << " g is now " << c << ", previous is now " << pq.top()->data() << endl;
				}

				else
				{
					if (childNode->getPrev() != 0)
						gop << "\t\t" << "False, " << childNode->data() << " g remaining " << (childNode->g()) << ", previous remains " << childNode->getPrev()->data() << endl;
					else gop << "\t\t" << "False, " << childNode->data() << " g remaining " << (childNode->g()) << ", previous remains NULL" << endl;
				}

				//if not marked
				if (childNode->marked() == false) {
					//add it to the queue and mark
					pq.push(childNode);
					gop << "Queueing:  " << childNode->data() << endl;
					childNode->setMarked(true);
				}
			}
		}
		gop << "Popping: " << pq.top()->data() << endl << endl;
		gout(2);

		pq.pop();

		if (!pq.empty())
			make_heap(const_cast<Node**>(&pq.top()), const_cast<Node**>(&pq.top()) + pq.size(), NodeSearchCostComparer<NodeType, ArcType>());
		
	}
	
	//End timer
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	gop << "\a\a=== UCS from " << pStart->data() << " to " << pTarget->data() << " complete. (" << elapsed_seconds.count() << "s)===" << endl << endl;
	gout(1);

	//Add the nodes to path
	path.clear();
	while (pTarget->getPrev() != NULL)
	{
		path.push_back(pTarget);
		pTarget = pTarget->getPrev();
	}
	path.push_back(pTarget);

	std::reverse(path.begin(), path.end());
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::InitAStar(Node* pTarget)
{	
	//Unmark, clear prev, max G, set up first node
	clearMarks();
	clearPrevs();
	maxGs();
	pTarget->setG(0);
	pTarget->setMarked(true);

	//Set up queue
	priority_queue<Node*, vector<Node*>, NodeSearchCostComparer<NodeType, ArcType>> pq;
	pq.push(pTarget);

	//Priority Queue loop
	while (!pq.empty() && pTarget != NULL)
	{
		//for each arc
		for (list<Arc>::const_iterator iter = pq.top()->arcList().begin(),  endIter = pq.top()->arcList().end(); iter != endIter; ++iter)
		{
			//pull out the node to test
			Node* childNode = iter->node();

			//If the previous node is not top of the queue
			if (childNode != pq.top()->getPrev())
			{
				//Get total weight of this route
				int c = pq.top()->g() + iter->weight();

				//if it's lower than the current weight
				if (c < childNode->g())
				{
					//set internal weight to route weight
					childNode->setG(c);
				}

				//if not marked
				if (childNode->marked() == false)
				{
					//add it to the queue and unmark
					pq.push(childNode);
					childNode->setMarked(true);
				}
			}
		}

		//Set heuristic using multiplier
		pq.top()->setH((pq.top()->g() * m_heurMult));

		pq.pop();

		if (!pq.empty())
			make_heap(const_cast<Node**>(&pq.top()), const_cast<Node**>(&pq.top()) + pq.size(), class NodeSearchCostComparer<NodeType, ArcType>());

	}
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::AStar(Node* pStart, Node* pTarget, std::vector<Node*>& path)
{
	gop << "\a=== A* from " << pStart->data() << " to " << pTarget->data() << " ===" << endl;
	gout(2);

	//Start timer
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	//Init path h by way of UCS
	InitAStar(pTarget);

	//make & set up queue
	priority_queue<Node*, vector<Node*>, ANodeSearchCostComparer<NodeType, ArcType>> pq;
	
	//Unmark, clear Prev, max G, set up first node
	clearMarks();
	clearPrevs();
	maxGs();
	pStart->setG(0);
	pStart->setMarked(true);
	
	//Start of A*
	pq.push(pStart);
	
	//Priority Queueue loop
	while (!pq.empty() && pq.top() != pTarget)
	{
	
		gop << "TOP: " << pq.top()->data() << endl;
	
		//for each child node
		list<Arc>::const_iterator iter = pq.top()->arcList().begin();
		list<Arc>::const_iterator endIter = pq.top()->arcList().end();
	
		//Process all children of the top node
		for (; iter != endIter; iter++) 
		{
			//Pull out the node to test
			Node * childNode = iter->node();
	
			//if the previous node is not top of the queue & Not marked
			if (childNode != pq.top()->getPrev() && !childNode->marked())
			{
	
				//Get f of this route (Weight to parent + arc to child)
				float fn = pq.top()->g() + iter->weight();
	
				gop << "\t" << "Checking: " << pq.top()->data() << " -> " << childNode->data() << " [" << fn << " < " << (childNode->g()) << "]" << endl;
	
				//if it's lower than the weight of the current route
				if (fn < childNode->g())
				{
					//Set the node's internal weight to the arc from previous plus internal weight of previous
					childNode->setG(fn);
					//Set previous pointer of the node to the previous node in the new path
					childNode->setPrev(pq.top());
	
					gop << "\t\t" << "True, " << childNode->data() << " weight is now " << fn << ", previous is now " << pq.top()->data() << endl;
				}
	
				else
				{
					if (childNode->getPrev() != 0)
						gop << "\t\t" << "False, " << childNode->data() << " remaining " << (childNode->g()) << ", previous remains " << childNode->getPrev()->data() << endl;
					else gop << "\t\t" << "False, " << childNode->data() << " remaining " << (childNode->g()) << ", previous remains NULL" << endl;
				}
				
				//if not marked
				if (childNode->marked() == false) {
					//add it to the queue and mark
					pq.push(childNode);
					gop << "Queueing:  " << childNode->data() << endl;
					childNode->setMarked(true);
				}
			}
		}
		gop << "Popping: " << pq.top()->data() << endl << endl;
		gout(2);
	
	
		if (!pq.empty())
			make_heap(const_cast<Node**>(&pq.top()), const_cast<Node**>(&pq.top()) + pq.size(), 
			ANodeSearchCostComparer<NodeType, ArcType>());
		pq.pop();
	}
	
	//End timer
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	
	gop << "\a\a=== A* from " << pStart->data() << " to " << pTarget->data() << " complete. (" << elapsed_seconds.count() << "s)===" << endl << endl;
	gout(1);
	
	//Add the nodes to path
	path.clear();
	while (pTarget->getPrev() != NULL)
	{
		path.push_back(pTarget);
		pTarget = pTarget->getPrev();
	}
	path.push_back(pTarget);
	
	std::reverse(path.begin(), path.end());
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::AStarPrecomp(Node* pStart, Node* pTarget, std::vector<Node*>& path)
{

	gop << "\a=== Precomputed A* from " << pStart->data() << " to " << pTarget->data() << " ===" << endl;
	gout(2);

	//Start timer
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	//Init H Values
	mapNodes(pTarget);
	
	//make & set up queue
	priority_queue<Node*, vector<Node*>, ANodeSearchCostComparer<NodeType, ArcType>> pq;

	//Unmark, clear Prev, max G, set up first node
	clearMarks();
	clearPrevs();
	maxGs();
	pStart->setG(0);
	pStart->setMarked(true);

	//Start of A*
	pq.push(pStart);

	//Priority Queueue loop
	while (!pq.empty() && pq.top() != pTarget)
	{

		gop << "TOP: " << pq.top()->data() << endl;

		//for each child node
		list<Arc>::const_iterator iter = pq.top()->arcList().begin();
		list<Arc>::const_iterator endIter = pq.top()->arcList().end();

		//Process all children of the top node
		for (; iter != endIter; iter++)
		{
			//Pull out the node to test
			Node * childNode = iter->node();

			//if the previous node is not top of the queue & Not marked
			if (childNode != pq.top()->getPrev() && !childNode->marked())
			{

				//Get f of this route (Weight to parent + arc to child)
				float fn = pq.top()->g() + iter->weight();

				gop << "\t" << "Checking: " << pq.top()->data() << " -> " << childNode->data() << " [" << fn << " < " << (childNode->g()) << "]" << endl;

				//if it's lower than the weight of the current route
				if (fn < childNode->g())
				{
					//Set the node's internal weight to the arc from previous plus internal weight of previous
					childNode->setG(fn);
					//Set previous pointer of the node to the previous node in the new path
					childNode->setPrev(pq.top());

					gop << "\t\t" << "True, " << childNode->data() << " weight is now " << fn << ", previous is now " << pq.top()->data() << endl;
				}

				else
				{
					if (childNode->getPrev() != 0)
						gop << "\t\t" << "False, " << childNode->data() << " remaining " << (childNode->g()) << ", previous remains " << childNode->getPrev()->data() << endl;
					else gop << "\t\t" << "False, " << childNode->data() << " remaining " << (childNode->g()) << ", previous remains NULL" << endl;
				}

				//if not marked
				if (childNode->marked() == false) {
					//add it to the queue and mark
					pq.push(childNode);
					gop << "Queueing:  " << childNode->data() << endl;
					childNode->setMarked(true);
				}
			}
		}
		gop << "Popping: " << pq.top()->data() << endl << endl;
		gout(2);
	if (!pq.empty())
			make_heap(const_cast<Node**>(&pq.top()), const_cast<Node**>(&pq.top()) + pq.size(), 
			ANodeSearchCostComparer<NodeType, ArcType>());
		pq.pop();

		
	}

	//End timer
	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	gop << "\a\a=== A* from " << pStart->data() << " to " << pTarget->data() << " complete. (" << elapsed_seconds.count() << "s)===" << endl << endl;
	gout(1);
	
	//Add the nodes to path
	path.clear();
	while (pTarget->getPrev() != NULL)
	{
		path.push_back(pTarget);
		pTarget = pTarget->getPrev();
	}
	path.push_back(pTarget);
	
	std::reverse(path.begin(), path.end());
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::gout(int verbosity)
{
	if (verbosity <= m_verbosity)
	{
		cout << gop.str();
	}
	
	gop.str(string()); //Clear stringstream after attempting to output
	string s = gop.str();
}

#include "GraphNode.hpp"
#include "GraphArc.hpp"


#endif
