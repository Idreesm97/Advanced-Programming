#include "Node.h"
#include "Arc.h"
#include <vector>
#include "Navigation.h"


//all you need is 1 function which has parameters that the node will require. Once you get the parameters from the nodes in you can seperate them out and use each induvidual parameter to figure out 
//the rest of the issues to solve such as lat and long distances. The starting parameter will be a null pointer.


Node::Node(string& placeName, const int refNum, const float lat, const float lon) : m_placeName(placeName), m_RefNum(refNum), m_GetLat(lat), m_GetLon(lon)
{
}

Node::~Node()
{
}

const string& Node::GetPlaceName(void)const {
	return m_placeName;
}

int Node::GetRefNum(void)const {
	return m_RefNum;
}

float Node::GetLat(void)const {
	return m_GetLat;
}

float Node::GetLon(void)const {
	return m_GetLon;
}
void Node::addArc(Arc* const arcs) {
	m_Arcs.push_back(arcs);
}

const vector<Arc*>& Node::GetArcs()const {
	return m_Arcs;
}