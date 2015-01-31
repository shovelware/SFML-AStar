#ifndef GRAPHMAP_H
#define GRAPHMAP_H

#include <vector>
#include <iostream>

using std::pair;
using std::cout;
using std::endl;

template<class NodeType, class ArcType>
class GraphMap  {
private:
	typedef GraphArc<NodeType, ArcType> Node;
public:
	GraphMap();
};

#endif