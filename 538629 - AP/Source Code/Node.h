#pragma once
#include <string>
#include "Arc.h"
#include <vector>
using namespace std;

class Arc;

class Node
{
public:
	~Node();
	Node(string& placeName, int refNum, float lat, float lon);

	const string& GetPlaceName(void)const;
	int GetRefNum(void)const;
	float GetLat(void)const;
	float GetLon(void)const;
	void addArc(Arc* const arcs);
	const vector<Arc*>& GetArcs()const;

private:
	vector<Arc*> m_Arcs;
	string m_placeName;
	int m_RefNum;
	float m_GetLat;
	float m_GetLon;
};

