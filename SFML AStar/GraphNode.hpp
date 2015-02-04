#ifndef GRAPHNODE_H
#define GRAPHNODE_H

#include <list>
#include <SFML/System/Vector2.hpp>

// Forward references
template <typename NodeType, typename ArcType> class GraphArc;

template<class NodeType, class ArcType>
class GraphNode {
private:

// typedef the classes to make our lives easier.
typedef GraphArc<NodeType, ArcType> Arc;
typedef GraphNode<NodeType, ArcType> Node;

    NodeType m_data;
    list<Arc> m_arcList;
	Node* m_prevNode;
    bool m_marked;
	ArcType m_g; //Actual distance, used for UCS and A*
	int m_h; //Heuristic, used for A*
	sf::Vector2f m_pos; //Position, used for drawing

public:
	//Constructor
	GraphNode() : m_prevNode(NULL), m_g(0), m_marked(false) {}

    // Accessor functions
    list<Arc> const & arcList() const { return m_arcList; }
    bool marked() const { return m_marked; }
	NodeType const & data() const { return m_data; }
	ArcType const & g() const { return m_g; }
	int const & h() const { return m_h; }
	sf::Vector2f const & position() const { return m_pos; }
	
    // Manipulator functions
    void setData(NodeType data) { m_data = data; }
    void setMarked(bool mark) { m_marked = mark; }
	void setG(ArcType g) { m_g = g; }
	void setH(float h) { m_h = h; }
	void setPosition(sf::Vector2f position) { m_pos = position; }

	//Arcs
    Arc* getArc( Node* pNode );  
    void addArc( Node* pNode, ArcType pWeight );
	void removeArc(Node* pNode);

	//Previous pointer
	Node* getPrev() { return m_prevNode; }
	void setPrev(Node* newPrev) { m_prevNode = newPrev; }
};

template<typename NodeType, typename ArcType>
GraphArc<NodeType, ArcType>* GraphNode<NodeType, ArcType>::getArc( Node* pNode ) {

     list<Arc>::iterator iter = m_arcList.begin();
     list<Arc>::iterator endIter = m_arcList.end();
     Arc* pArc = 0;
     
     // find the arc that matches the node
     for( ; iter != endIter && pArc == 0; ++iter ) {         
          if ( (*iter).node() == pNode) {
               pArc = &( (*iter) );
          }
     }

     // returns null if not found
     return pArc;
}

template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::addArc( Node* pNode, ArcType weight ) {
   // Create a new arc.
   Arc a;
   a.setNode(pNode);
   a.setWeight(weight);   
   // Add it to the arc list.
   m_arcList.push_back( a );
}

template<typename NodeType, typename ArcType>
void GraphNode<NodeType, ArcType>::removeArc( Node* pNode ) {
     list<Arc>::iterator iter = m_arcList.begin();
     list<Arc>::iterator endIter = m_arcList.end();

     int size = m_arcList.size();
     // find the arc that matches the node
     for( ; iter != endIter && m_arcList.size() == size;  
                                                    ++iter ) {
          if ( (*iter).node() == pNode) {
             m_arcList.remove( (*iter) );
          }                           
     }
}

#include "GraphArc.hpp"

#endif
