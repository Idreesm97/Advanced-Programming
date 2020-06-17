#pragma once
#include <string>
#include "Node.h"
using namespace std;

class Node;

class Arc
{
public:
	~Arc();
	Arc(const int startRef, const int endRef, string& transport_Type);
	Arc(const Arc&);
	Arc& operator=(const Arc&);

	int GetStartRef(void)const;
	int GetEndRef(void)const;
	const string& GetTransport_Type(void)const;
	void setDestination(Node* const n);
	Node* getDestination(void)const;

private:
	int m_GetStartRef;
	int m_GetEndRef;
	string m_GetTransportType;
	Node * m_destination;
};