#include "Arc.h"
#include "Node.h"



Arc::Arc(const int startRef, const int endRef, string& Transport_Type) : m_GetStartRef(startRef), m_GetEndRef(endRef), m_GetTransportType(Transport_Type), m_destination()
{
}


Arc::~Arc()
{
}


int Arc::GetStartRef(void)const {
	return m_GetStartRef;
}

int Arc::GetEndRef(void)const {
	return m_GetEndRef;
}

const string& Arc::GetTransport_Type(void)const {
	return m_GetTransportType;
}

void Arc::setDestination(Node* const n) {
	m_destination = n;
}

Node* Arc::getDestination(void)const {
	return m_destination;
}