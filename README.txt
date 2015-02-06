===Controls===
Left Click:
	On an unmarked node:
		Set Start or End if we haven't already.
	
	On Start or End:
		Unset that node.
		
Right Click: Unset End, then again for Start.

Space: Precompute H values from map. (If end node is selected)

W:	Toggle Weight drawing.
D:	Toggle Data drawing.
G:	Toggle G drawing.
H:	Toggle H drawing.

===Buttons===
Pathfinding:
UCS: Runs UCS between marked nodes.
AStar: Runs A* between marked nodes.
Map: Generates a map if there isn't one, otherwise runs A* using the map.

Nodes:
Swap: Switches start and end node.
Random: Randomly selects a start and end node.
Reset: Clears the path if there is one, else clears start and end nodes.

===Notes===
UCS runs regular UCS.
AStar runs UCS to every other node and sets the H to 90% of the path cost.
Mapped AStar uses the Euclidian distance between nodes as the H.

===Colours===
Node: Purple
Expanded Node: Light Purple
Path Node: Cyan
Start: Green
End: Red

Arc: Light Gray
Path Arc: Yellow

Data: White
G: Green
H: Yellow
Weight: Blue