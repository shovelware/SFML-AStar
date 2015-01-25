#ifndef GRAPHARC_H
#define GRAPHARC_H

#include "GraphNode.hpp"

template<class NodeType, class ArcType>
class GraphArc {
private:
    GraphNode<NodeType, ArcType>* m_pNode;
    ArcType m_weight;

public:    
    // Accessor functions
    GraphNode<NodeType, ArcType>* node() const { return m_pNode; }
	ArcType weight() const { return m_weight; }

    // Manipulator functions
    void setNode(GraphNode<NodeType, ArcType>* pNode) { m_pNode = pNode; }    
    void setWeight(ArcType weight) { m_weight = weight; }
};

#endif
