#ifndef GRAPH_H
#define GRAPH_H

#include <list>
#include <queue>
#include <sstream>

using namespace std;

template <class NodeType, class ArcType> class GraphArc;
template <class NodeType, class ArcType> class GraphNode;

template<class NodeType, class ArcType>
class Graph {
private:
    typedef GraphArc<NodeType, ArcType> Arc;
    typedef GraphNode<NodeType, ArcType> Node;

    Node** m_pNodes; //An array of all the nodes in the graph.
    int m_maxNodes;
    int m_count;
	float m_heurMult; //Heuristic multiplier for A*

	int m_verbosity = 1; //Verbosity for output
	stringstream gop; //Reusable stringstream for output
	void gout(int verbosity); //Output function

	//Preparations
	void prepUCS();
	void prepAStar();
	void clearMarks();

public:           
    // Constructor and destructor functions
    Graph( int size );
    ~Graph();

    // Accessors
	Node** nodeArray() const { return m_pNodes; }
	float heurMult() { return m_heurMult; }
	int verbosity() { return verbosity; }
	int count() { return m_count; }

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

	//Graph exercises
    void depthFirst( Node* pNode, void (*pProcess)(Node*) );
	void breadthFirst(Node* pNode, void(*pProcess)(Node*));
	void breadthFirstPlus(Node* pNode, Node* pTarget, void(*pProcess)(Node*));
	void UCS(Node* pStart, Node* pTarget, void(*pProcess)(Node*), std::vector<Node*>& path);
	void AStar(Node* pStart, Node* pTarget, void(*pProcess)(Node*), std::vector<Node*>& path);
};

template<class NodeType, class ArcType>
class NodeSearchCostComparer {
public:
	bool operator()(GraphNode<NodeType, ArcType> * n1, GraphNode<NodeType, ArcType> * n2) {
		return n1->g() > n2->g();
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
	int index;
	for (index = 0; index < m_maxNodes; index++) {
		if (m_pNodes[index] != 0)
		{
			m_pNodes[index]->setMarked(false);

			gop << m_pNodes[index]->data() << " unmarked." << endl;
			gout(4);
		}
	}
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::prepUCS()
{
	gop << "=== Prepping UCS ===" << endl;
	gout(1);

	int index;
	for (index = 0; index < m_maxNodes; index++) {
		if (m_pNodes[index] != 0) {
			m_pNodes[index]->setG(INT_MAX);
			m_pNodes[index]->setMarked(false);
			m_pNodes[index]->setPrev(NULL);

			gop << "\t" << m_pNodes[index]->data() << " distance maxed, unmarked and cleared previous." << endl;
			gout(4);
		}
	}

	gop << "=== UCS Prepped ===" << endl << endl;
	gout(1);
}

template<class NodeType, class ArcType>
void Graph<NodeType, ArcType>::prepAStar()
{
	gop << "=== Prepping A* ===" << endl;
	gout(1);
	int index;
	for (index = 0; index < m_maxNodes; index++) {
		if (m_pNodes[index] != 0) {
			m_pNodes[index]->setH(FLT_MAX - 1000);
			m_pNodes[index]->setMarked(false);
			m_pNodes[index]->setPrev(NULL);

			gop << "\t" << m_pNodes[index]->data() << " heuristic maxed, unmarked and cleared previous." << endl;
			gout(4);
		}
	}

	gop << "=== A* Prepped ===" << endl << endl;
	gout(1);
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
void Graph<NodeType, ArcType>::UCS(Node* pStart, Node* pTarget, void(*pProcess)(Node*), std::vector<Node*>& path)
{
	//init distances and unmark
	prepUCS();
	pStart->setG(0);

	gop << "=== UCS from " << pStart->data() << " to " << pTarget->data() << " ===" << endl;
	gout(1);

	//make & set up queue
	priority_queue<Node*, vector<Node*>, NodeSearchCostComparer<NodeType, ArcType>> pq;
	
	//Start of UCS
	pq.push(pStart);
	pStart->setMarked(true);
	
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
			Node* childNode = (*iter).node();

			//if the previous node is not top of the queue
			if (childNode != pq.top()->getPrev())
			{
				//Get total weight of this route
				int c = pq.top()->getArc(childNode)->weight() + pq.top()->g();

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
	}
	
	gop << "=== UCS from " << pStart->data() << " to " << pTarget->data() << " complete. ===" << endl << endl;
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
void Graph<NodeType, ArcType>::AStar(Node* pStart, Node* pTarget, void(*pProcess)(Node*), std::vector<Node*>& path)
{
	gop << "=== A* from " << pStart->data() << " to " << pTarget->data() << " ===" << endl;
	gop << "(UCS used to initialise h values)" << endl << endl;
	gout(1);

	//Init h by way of UCS
	vector<Node*> UCSP;
	UCS(pStart, pTarget, pProcess, UCSP);

	//init distances and unmark
	prepAStar();
	pStart->setG(0);

	int gn = 0;
	for (vector<Node*>::iterator vIter = UCSP.begin(), vEnd = UCSP.end(); vIter < vEnd; vIter++)
	{
		(*vIter)->setH(gn);
		gn = (*vIter)->g() * m_heurMult;
	}

	clearMarks();
	//make & set up queue
	priority_queue<Node*, vector<Node*>, NodeSearchCostComparer<NodeType, ArcType>> pq;

	//Start of UCS
	pq.push(pStart);
	pStart->setMarked(true);

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
			Node * childNode = (*iter).node();

			//if the previous node is not top of the queue
			//ROSS: Child should be in pq?
			if (childNode != pq.top()->getPrev())
			{

				//Get total weight of this route
				float fn = pq.top()->g() + childNode->h();

				gop << "\t" << "Checking: " << pq.top()->data() << " -> " << childNode->data() << " [" << fn << " < " << (childNode->g()) << "]" << endl;

				//if it's lower than the weight of the current route
				if (fn < (childNode->g()))
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
					childNode->setMarked(true);
					gop << "Queueing:  " << childNode->data() << endl;
				}
			}
		}
		gop << "Popping: " << pq.top()->data() << endl << endl;
		gout(2);
		pq.pop();
	}

	gop << "=== A* from " << pStart->data() << " to " << pTarget->data() << " completed ===" << endl;
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
}

#include "GraphNode.hpp"
#include "GraphArc.hpp"


#endif
