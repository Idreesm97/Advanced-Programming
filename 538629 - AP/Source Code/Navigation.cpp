#include "Navigation.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include "Node.h"
#include "Arc.h"
#include "algorithm";

vector<Node*> myNodes;
vector<Arc*> myArcs;
float distanceNum;
vector<string> modeTransportVector;
vector<float> distanceNode;
vector<int> VisitedNode;
vector<int> observableNode;//observable node add the current node to the vector then finds all the arcs linked to that node in the nodes vector,
//then looks through the arcs to find what arcs the arcs of the current node is linked to.
int vectorCounter = 100;
int backtrackNode = 0;

using namespace std;

// Converts latitude/longitude into eastings/northings
extern void LLtoUTM(const double Lat, const double Long, double &UTMNorthing, double &UTMEasting);
Navigation::Navigation() : _outFile("Output.txt")
{
}

Navigation::~Navigation()
{
}

bool Navigation::ProcessCommand(const string& commandString)
{
	istringstream inString(commandString);
	string command;
	inString >> command;
	if (command == "MaxDist")
	{
		return MaxDistance();
	}
	if (command == "MaxLink") {
		return MaxLink();
	}
	if (command == "FindDist")
	{
		//Finddist needs to be checked as the distance being calculated uses correct theory but has references hard coded into them.
		//Get some understanding of if this is write or not being continuing with finddist
		double northings = 0;
		double eastings = 0;

		double northings2 = 0;
		double eastings2 = 0;

		double maxDistance = 0;
		string nodename1;
		string nodename2;
		int refNumber;
		int refNumber2;
		int counter = 0;
		inString >> refNumber;
		inString >> refNumber2;
		maxDistanceVar = 0;
		for (int i = 0; i < myNodes.size(); i++) {
			if (myNodes[i]->GetRefNum() == refNumber) {
				const float x1 = myNodes[i]->GetLat();
				const float y1 = myNodes[i]->GetLon();
				LLtoUTM(x1, y1, northings, eastings);
				nodename1 = myNodes[i]->GetPlaceName();
				counter++;
			}
			if (myNodes[i]->GetRefNum() == refNumber2)
			{
				const float x2 = myNodes[i]->GetLat();
				const float y2 = myNodes[i]->GetLon();
				LLtoUTM(x2, y2, northings2, eastings2);
				nodename2 = myNodes[i]->GetPlaceName();
				counter++;
			}
			if (counter == 2)
			{
				maxDistance = Pythagarous(northings, eastings, northings2, eastings2);
				break;
			}
		}
		_outFile << "FindDist" << " " << refNumber << " " << refNumber2 << endl;
		_outFile << nodename1 << " , " << nodename2 << " , " << fixed << setprecision(3) << maxDistance << endl;
		_outFile << endl;
		return true;
	}
	if (command == "FindNeighbour")
	{
		//findNeighbour will list all the neighbours to a specified node. FindNeighbour will specify all the arcs connected to a specified node by outputting their ref number
		int refNumber2 = 0;
		int refNumber;
		inString >> refNumber;
		_outFile << "FindNeighbour" << " " << refNumber << endl;
		for (int i = 0; i < myNodes.size(); i++)
		{
			if (myNodes[i]->GetRefNum() == refNumber) {
				vector<Arc*> Arcs = myNodes[i]->GetArcs();
				for (int j = 0; j < Arcs.size(); j++) {
					if (Arcs[j]->GetStartRef() == refNumber)
					{
						refNumber2 = Arcs[j]->GetEndRef();
						_outFile << refNumber2 << endl;
					}
					else if (Arcs[j]->GetEndRef() == refNumber)
					{
						refNumber2 = Arcs[j]->GetStartRef();
						_outFile << refNumber2 << endl;
					}
				}
			}
		}
		_outFile << endl;
		return true;
	}
	if (command == "Check")
	{
#pragma region ModeofTransport Code
		string modeTransport;
		int refNumber;
		int refNumber2;
		inString >> modeTransport;
		inString >> refNumber;
		inString >> refNumber2;
		ModeTransport(modeTransport);
#pragma endregion

#pragma region Check Algorithm
		_outFile << "Check" << " " << modeTransport;
		vector<int> refNodes;
		do
		{
			refNodes.push_back(refNumber);
			refNumber = refNumber2;
			inString >> refNumber2;

		} while (refNumber != refNumber2);
		if (refNumber == refNumber2)
		{
			refNodes.push_back(refNumber2);
		}
		for (int k = 0; k < refNodes.size(); k++)
		{
			_outFile << " " << refNodes[k];

			if (k == refNodes.size() - 1)
			{
				_outFile << endl;
			}
		}
		for (int i = 0; i < myNodes.size(); i++) {
			if (myNodes[i]->GetRefNum() == refNodes[0]) {
				vector<Arc*> Arcs = myNodes[i]->GetArcs();
				for (int j = 0; j < Arcs.size(); j++) {
					if ((Arcs[j]->GetEndRef() == refNodes[1]) && (count(modeTransportVector.begin(), modeTransportVector.end(), Arcs[j]->GetTransport_Type())))
					{
						_outFile << refNodes[0] << " " << refNodes[1] << " PASS" << endl;
						refNodes.erase(refNodes.begin() + 0);
						break;
					}
					else if (Arcs[j]->GetStartRef() == refNodes[1] && (count(modeTransportVector.begin(), modeTransportVector.end(), Arcs[j]->GetTransport_Type())))
					{
						_outFile << refNodes[0] << " " << refNodes[1] << " PASS" << endl;
						refNodes.erase(refNodes.begin() + 0);
						break;
					}
					else if (Arcs[j]->GetEndRef() == refNodes[1] && (!count(modeTransportVector.begin(), modeTransportVector.end(), Arcs[j]->GetTransport_Type())))
					{
						_outFile << refNodes[0] << " " << refNodes[1] << " " << " FAIL" << endl;
						_outFile << endl;
						return true;
					}
					else if (Arcs[j]->GetStartRef() == refNodes[1] && (!count(modeTransportVector.begin(), modeTransportVector.end(), Arcs[j]->GetTransport_Type())))
					{
						_outFile << refNodes[0] << " " << refNodes[1] << " " << " FAIL" << endl;
						_outFile << endl;
						return true;
					}
					else if (j == Arcs.size() - 1)
					{
						_outFile << refNodes[0] << " " << refNodes[1] << " " << " FAIL" << endl;
						_outFile << endl;
						return true;
					}
				}
				i = 0;
				i--;
				if (refNodes.size() == 1)
				{
					_outFile << endl;
					return true;
				}
				//once you get the arcs linked to the target node you should then compare all the 
				//startrefnumbers in the getarcs and see if they match the next refnumber2 to see 
				//if at any point the two nodes are linked
			}
			else if (myNodes[i] == myNodes.back() && myNodes[i]->GetRefNum() != refNodes[0])
			{
				_outFile << refNodes[0] << " " << refNodes[1] << " FAIL" << endl;
				return true;
			}
		}
#pragma endregion
	}
	if (command == "FindRoute")
	{
		//To implement djikstras algo I would havve to find the distance between the current node and all the neighbours of the current node.
		//Then take the shortest path until the target node has been found.

		//Need to find the distance between each arc which has the correct transport type then chose the arc has the shortest distance to the target node.
		//Issue with findroute not working is that the instring transport mode updates before it's able to find the target node meaning it's not able to 
		//get it at all.
#pragma region ReadIn and ModeofTransport
		string modeTransport;
		int refNumber1;
		int refNumber2;
		inString >> modeTransport;
		inString >> refNumber1;
		inString >> refNumber2;
		ModeTransport(modeTransport);
#pragma endregion

#pragma region FindRoute Algorithm
		//Need to include djikstas algorithm in the BFS algorithm. I need to calculate the distance between the current node and the nodes linked through the current node through the arcs.
		//Once I've calculated the distance between the current node and the nodes available through the arcs, pick the shortest distance.

		vector<float> distanceNode;
		vector<int> VisitedNode;
		vector<int> observableNode;//observable node add the current node to the vector then finds all the arcs linked to that node in the nodes vector,
								   //then looks through the arcs to find what arcs the arcs of the current node is linked to.
		const int refTemp = refNumber1;
		const int counter = 0;
		const int counterNode = 0;
		int temp = 0;
		int temp2 = 0;
		_outFile << "FindRoute " << modeTransport << " " << refNumber1 << " " << refNumber2 << endl;
		for (int i = 0; i < myNodes.size(); i++)
		{
			if (myNodes[i]->GetRefNum() == refTemp)
			{
				VisitedNode.push_back(refTemp);
				vector<Arc*> Arcs = myNodes[i]->GetArcs();

				for (int j = 0; j < Arcs.size(); j++)
				{
					if (refTemp == Arcs[j]->GetStartRef() && count(modeTransportVector.begin(), modeTransportVector.end(), Arcs[j]->GetTransport_Type()))
					{
						findDistanceNode(refNumber2, Arcs[j]->GetEndRef());
						observableNode.push_back(Arcs[j]->GetEndRef());
						distanceNode.push_back(distanceNum);
						i = 0;
					}
					else if (refTemp == Arcs[j]->GetEndRef() && count(modeTransportVector.begin(), modeTransportVector.end(), Arcs[j]->GetTransport_Type()))
					{
						findDistanceNode(refNumber2, Arcs[j]->GetStartRef());
						observableNode.push_back(Arcs[j]->GetStartRef());
						distanceNode.push_back(distanceNum);
						i = 0;
					}
					if (j == Arcs.size() - 1 && observableNode.size() == 0)
					{
						_outFile << "FAIL" << endl;
						_outFile << endl;
						return true;
					}
				}
				for (int k = 0; k < distanceNode.size(); k++)
				{
					for (int n = k + 1; n < distanceNode.size(); n++)
					{
						if (distanceNode[k] > distanceNode[n])
						{
							temp = distanceNode[k];
							distanceNode[k] = distanceNode[n];
							distanceNode[n] = temp;
							temp2 = observableNode[k];
							observableNode[k] = observableNode[n];
							observableNode[n] = temp2;
						}
					}
				}
				break;
			}
		}

		for (int x = 0; x < myNodes.size(); x++)
		{
			for (int z = 0; z < observableNode.size(); z++)
			{
				if (myNodes[x]->GetRefNum() == observableNode[0] && (!count(VisitedNode.begin(), VisitedNode.end(), observableNode[0])))
				{
					vector<Arc*> arcsObserved = myNodes[x]->GetArcs();
					for (int s = 0; s < arcsObserved.size(); s++)
					{
						if (observableNode[z] == arcsObserved[s]->GetStartRef() && count(modeTransportVector.begin(), modeTransportVector.end(), arcsObserved[s]->GetTransport_Type()))
						{
							if (!count(VisitedNode.begin(), VisitedNode.end(), arcsObserved[s]->GetEndRef()))
							{
								findDistanceNode(refNumber2, arcsObserved[s]->GetEndRef());
								observableNode.push_back(arcsObserved[s]->GetEndRef());
								distanceNode.push_back(distanceNum);
							}
							if (count(observableNode.begin(), observableNode.end(), refNumber2))
							{
								VisitedNode.push_back(observableNode[0]);
								VisitedNode.push_back(arcsObserved[s]->GetEndRef());
								for (int l = 0; l < VisitedNode.size(); l++)
								{
									_outFile << VisitedNode[l] << endl;

									if (l == VisitedNode.size() - 1)
									{
										_outFile << endl;
									}
								}
								return true;
							}
						}
						else if (observableNode[z] == arcsObserved[s]->GetEndRef() && count(modeTransportVector.begin(), modeTransportVector.end(), arcsObserved[s]->GetTransport_Type()))
						{
							if (!count(VisitedNode.begin(), VisitedNode.end(), arcsObserved[s]->GetStartRef()))
							{
								findDistanceNode(refNumber2, arcsObserved[s]->GetStartRef());
								observableNode.push_back(arcsObserved[s]->GetStartRef());
								distanceNode.push_back(distanceNum);
							}
							if (count(observableNode.begin(), observableNode.end(), refNumber2))
							{
								VisitedNode.push_back(observableNode[0]);
								VisitedNode.push_back(arcsObserved[s]->GetStartRef());
								for (int l = 0; l < VisitedNode.size(); l++)
								{
									_outFile << VisitedNode[l] << endl;

									if (l == VisitedNode.size() - 1)
									{
										_outFile << endl;
									}
								}
								return true;
							}
						}
						if (s == arcsObserved.size() - 1)
						{
							VisitedNode.push_back(myNodes[x]->GetRefNum());
							observableNode.erase(observableNode.begin() + z);
							distanceNode.erase(distanceNode.begin() + z);
							z = 0;
							z--;
							x = 0;

							//This causes the number 18091793 to decrement by 1 causing the node to not be found. Due to the sorting algorithm
							for (int k = 0; k < distanceNode.size(); k++)
							{
								for (int n = k + 1; n < distanceNode.size(); n++)
								{
									if (distanceNode[k] > distanceNode[n])
									{
										temp = distanceNode[k];
										distanceNode[k] = distanceNode[n];
										distanceNode[n] = temp;
										temp2 = observableNode[k];
										observableNode[k] = observableNode[n];
										observableNode[n] = temp2;
									}
								}
							}
							//The issue with with the findshortestroute is that is adds a node sometimes which is not linked to the next node in the visited node. 
							//Meaning its adding an extra node causing the true shortest path to not be found.
							for (int x = 0; x < myNodes.size(); x++)
							{
								for (int z = VisitedNode.size() - 2; z < VisitedNode.size() - 1; z++)
								{
									if (myNodes[x]->GetRefNum() == VisitedNode[z])
									{
										vector<Arc*> ArcsLinked = myNodes[x]->GetArcs();
										for (int l = 0; l < ArcsLinked.size(); l++)
										{
											if (VisitedNode[VisitedNode.size() - 2] == ArcsLinked[l]->GetStartRef())
											{
												if (ArcsLinked[l]->GetEndRef() == VisitedNode.back())
												{
													break;
												}
												else if (l == ArcsLinked.size() - 1)
												{
													//remove the second to last element
													VisitedNode.erase(VisitedNode.end() - 2);
												}
											}
											else if (VisitedNode[VisitedNode.size() - 2] == ArcsLinked[l]->GetEndRef())
											{
												if (ArcsLinked[l]->GetStartRef() == VisitedNode.back())
												{
													break;
												}
												else if (l == ArcsLinked.size() - 1)
												{
													//remove the second to last element
													VisitedNode.erase(VisitedNode.end() - 2);
												}
											}
										}
									}
								}
							}
						}
					}
				}
				else if (count(VisitedNode.begin(), VisitedNode.end(), observableNode[0]))
				{
					observableNode.erase(observableNode.begin() + z);
					distanceNode.erase(distanceNode.begin() + z);
					z = 0;
					z--;
					x = 0;
				}
				else if (myNodes[x] == myNodes.back() && (myNodes[x]->GetRefNum() != observableNode[0]))
				{
					observableNode.erase(observableNode.begin() + z);
					distanceNode.erase(distanceNode.begin() + z);
					z = 0;
					z--;
					x = 0;
				}
				else if (observableNode.size() == 0)
				{
					_outFile << "FAIL" << endl;
					_outFile << endl;
					return true;
				}
			}
		}
#pragma endregion

	}
	if (command == "FindShortestRoute")
	{
#pragma region ReadIn and ModeofTransport
		string modeTransport;
		int refNumber1;
		int refNumber2;
		inString >> modeTransport;
		inString >> refNumber1;
		inString >> refNumber2;
		ModeTransport(modeTransport);
#pragma endregion

#pragma region ShortestRoute Algorithm
		//Need to include djikstas algorithm in the BFS algorithm. I need to calculate the distance between the current node and the nodes linked through the current node through the arcs.
		//Once I've calculated the distance between the current node and the nodes available through the arcs, pick the shortest distance.

		vector<float> distanceNode;
		vector<int> VisitedNode;
		vector<int> observableNode;//observable node add the current node to the vector then finds all the arcs linked to that node in the nodes vector,
								   //then looks through the arcs to find what arcs the arcs of the current node is linked to.
		const int refTemp = refNumber1;
		const int counter = 0;
		const int counterNode = 0;
		int temp = 0;
		int temp2 = 0;
		_outFile << "FindShortestRoute " << modeTransport << " " << refNumber1 << " " << refNumber2 << endl;
		for (int i = 0; i < myNodes.size(); i++)
		{
			if (myNodes[i]->GetRefNum() == refTemp)
			{
				VisitedNode.push_back(refTemp);
				vector<Arc*> Arcs = myNodes[i]->GetArcs();

				for (int j = 0; j < Arcs.size(); j++)
				{
					if (refTemp == Arcs[j]->GetStartRef() && count(modeTransportVector.begin(), modeTransportVector.end(), Arcs[j]->GetTransport_Type()))
					{
						findDistanceNode(refNumber2, Arcs[j]->GetEndRef());
						observableNode.push_back(Arcs[j]->GetEndRef());
						distanceNode.push_back(distanceNum);
						i = 0;
					}
					else if (refTemp == Arcs[j]->GetEndRef() && count(modeTransportVector.begin(), modeTransportVector.end(), Arcs[j]->GetTransport_Type()))
					{
						findDistanceNode(refNumber2, Arcs[j]->GetStartRef());
						observableNode.push_back(Arcs[j]->GetStartRef());
						distanceNode.push_back(distanceNum);
						i = 0;
					}
					if (j == Arcs.size() - 1 && observableNode.size() == 0)
					{
						_outFile << "FAIL" << endl;
						_outFile << endl;
						return true;
					}
				}
				for (int k = 0; k < distanceNode.size(); k++)
				{
					for (int n = k + 1; n < distanceNode.size(); n++)
					{
						if (distanceNode[k] > distanceNode[n])
						{
							temp = distanceNode[k];
							distanceNode[k] = distanceNode[n];
							distanceNode[n] = temp;
							temp2 = observableNode[k];
							observableNode[k] = observableNode[n];
							observableNode[n] = temp2;
						}
					}
				}
				break;
			}
		}

		for (int x = 0; x < myNodes.size(); x++)
		{
			for (int z = 0; z < observableNode.size(); z++)
			{
				if (myNodes[x]->GetRefNum() == observableNode[0] && (!count(VisitedNode.begin(), VisitedNode.end(), observableNode[0])))
				{
					vector<Arc*> arcsObserved = myNodes[x]->GetArcs();
					for (int s = 0; s < arcsObserved.size(); s++)
					{
						if (observableNode[z] == arcsObserved[s]->GetStartRef() && count(modeTransportVector.begin(), modeTransportVector.end(), arcsObserved[s]->GetTransport_Type()))
						{
							if (!count(VisitedNode.begin(), VisitedNode.end(), arcsObserved[s]->GetEndRef()))
							{
								findDistanceNode(refNumber2, arcsObserved[s]->GetEndRef());
								observableNode.push_back(arcsObserved[s]->GetEndRef());
								distanceNode.push_back(distanceNum);
							}
							if (count(observableNode.begin(), observableNode.end(), refNumber2))
							{
								VisitedNode.push_back(observableNode[0]);
								VisitedNode.push_back(arcsObserved[s]->GetEndRef());
								for (int l = 0; l < VisitedNode.size(); l++)
								{
									_outFile << VisitedNode[l] << endl;

									if (l == VisitedNode.size() - 1)
									{
										_outFile << endl;
									}
								}
								return true;
							}
						}
						else if (observableNode[z] == arcsObserved[s]->GetEndRef() && count(modeTransportVector.begin(), modeTransportVector.end(), arcsObserved[s]->GetTransport_Type()))
						{
							if (!count(VisitedNode.begin(), VisitedNode.end(), arcsObserved[s]->GetStartRef()))
							{
								findDistanceNode(refNumber2, arcsObserved[s]->GetStartRef());
								observableNode.push_back(arcsObserved[s]->GetStartRef());
								distanceNode.push_back(distanceNum);
							}
							if (count(observableNode.begin(), observableNode.end(), refNumber2))
							{
								VisitedNode.push_back(observableNode[0]);
								VisitedNode.push_back(arcsObserved[s]->GetStartRef());
								for (int l = 0; l < VisitedNode.size(); l++)
								{
									_outFile << VisitedNode[l] << endl;

									if (l == VisitedNode.size() - 1)
									{
										_outFile << endl;
									}
								}
								return true;
							}
						}
						if (s == arcsObserved.size() - 1)
						{
							VisitedNode.push_back(myNodes[x]->GetRefNum());
							observableNode.erase(observableNode.begin() + z);
							distanceNode.erase(distanceNode.begin() + z);
							z = 0;
							z--;
							x = 0;

							//This causes the number 18091793 to decrement by 1 causing the node to not be found. Due to the sorting algorithm
							for (int k = 0; k < distanceNode.size(); k++)
							{
								for (int n = k + 1; n < distanceNode.size(); n++)
								{
									if (distanceNode[k] > distanceNode[n])
									{
										temp = distanceNode[k];
										distanceNode[k] = distanceNode[n];
										distanceNode[n] = temp;
										temp2 = observableNode[k];
										observableNode[k] = observableNode[n];
										observableNode[n] = temp2;
									}
								}
							}
							//The issue with with the findshortestroute is that is adds a node sometimes which is not linked to the next node in the visited node. 
							//Meaning its adding an extra node causing the true shortest path to not be found.
							for (int x = 0; x < myNodes.size(); x++)
							{
								for (int z = VisitedNode.size() - 2; z < VisitedNode.size() - 1; z++)
								{
									if (myNodes[x]->GetRefNum() == VisitedNode[z])
									{
										vector<Arc*> ArcsLinked = myNodes[x]->GetArcs();
										for (int l = 0; l < ArcsLinked.size(); l++)
										{
											if (VisitedNode[VisitedNode.size() - 2] == ArcsLinked[l]->GetStartRef())
											{
												if (ArcsLinked[l]->GetEndRef() == VisitedNode.back())
												{
													break;
												}
												else if (l == ArcsLinked.size() - 1)
												{
													//remove the second to last element
													VisitedNode.erase(VisitedNode.end() - 2);
												}
											}
											else if (VisitedNode[VisitedNode.size() - 2] == ArcsLinked[l]->GetEndRef())
											{
												if (ArcsLinked[l]->GetStartRef() == VisitedNode.back())
												{
													break;
												}
												else if (l == ArcsLinked.size() - 1)
												{
													//remove the second to last element
													VisitedNode.erase(VisitedNode.end() - 2);
												}
											}
										}
									}
								}
							}
						}
					}
				}
				else if (count(VisitedNode.begin(), VisitedNode.end(), observableNode[0]))
				{
					observableNode.erase(observableNode.begin() + z);
					distanceNode.erase(distanceNode.begin() + z);
					z = 0;
					z--;
					x = 0;
				}
				else if (myNodes[x] == myNodes.back() && (myNodes[x]->GetRefNum() != observableNode[0]))
				{
					observableNode.erase(observableNode.begin() + z);
					distanceNode.erase(distanceNode.begin() + z);
					z = 0;
					z--;
					x = 0;
				}
				else if (observableNode.size() == 0)
				{
					_outFile << "FAIL" << endl;
					_outFile << endl;
					return true;
				}
			}
		}
#pragma endregion
	}

	/*This function is used to ouput any of the data for the functions that you need to create.
	so use a conditional that says if maxdist = true, then return true, else return false.
	 Add your code here
	return false;*/
}

bool Navigation::BuildNetwork(const string &fileNamePlaces, const string &fileNameLinks)const
{
#pragma region BuildNetwork ReadIn
	// Add your code here
	fstream finPlaces(fileNamePlaces);
	fstream finLinks(fileNameLinks);
	char line[255];
	int refNum;
	float lat;
	float lon;
	int refNumStart;
	int refNumEnd;



	if (finPlaces.fail() || finLinks.fail()) {
		return false;
	}
	else
	{
		while (finPlaces.good())
		{

			finPlaces.getline(line, 255, ',');
			string placeName = string(line);

			finPlaces.getline(line, 255, ',');
			istringstream sin(line);
			sin >> refNum;

			finPlaces.getline(line, 255, ',');
			istringstream sin2(line);
			sin2 >> lat;

			finPlaces.getline(line, 255, '\n');
			istringstream sin3(line);
			sin3 >> lon;

			//once code has reached this point create a new instance of the node class and input all the readin strings into the node
			Node *const nodes = new Node(placeName, refNum, lat, lon);
			myNodes.push_back(nodes);
		}
		while (finLinks.good())
		{

			finLinks.getline(line, 255, ',');
			istringstream sin(line);
			sin >> refNumStart;

			finLinks.getline(line, 255, ',');
			istringstream sin1(line);
			sin1 >> refNumEnd;

			finLinks.getline(line, 255, '\n');
			string modeTransport = string(line);

			//once code has reached this point create a new instance of the node class and input all the readin strings into the node
			Arc *const arcs = new Arc(refNumStart, refNumEnd, modeTransport);
			myArcs.push_back(arcs);
		}
	}
#pragma endregion

#pragma region BuildNetwork Linking
	for (int i = 0; i < myArcs.size(); i++) {
		for (int j = 0; j < myNodes.size(); j++)
		{
			if (myArcs[i]->GetStartRef() == myNodes[j]->GetRefNum()) {
				myArcs[i]->setDestination(myNodes[j]);
				myNodes[j]->addArc(myArcs[i]);
			}
			else if (myArcs[i]->GetEndRef() == myNodes[j]->GetRefNum())
			{
				myArcs[i]->setDestination(myNodes[j]);
				myNodes[j]->addArc(myArcs[i]);
			}
		}
	}
	return true;
#pragma endregion
}
// Add your code here

bool Navigation::MaxLink() {
#pragma region MaxLink
	double northings;
	double Easting;

	double northings2;
	double eastings2;

	float maxdistance = 0;
	float prevMaxDisance = 0;
	int nodeName1;
	int nodename2;
	int counter = 0;
	maxDistanceVar = 0;

	for (int i = 0; i < myArcs.size(); i++) {
		for (int j = 0; j < myNodes.size(); j++) {
			if (myArcs[i]->GetStartRef() == myNodes[j]->GetRefNum())
			{
				const float x1 = myNodes[j]->GetLat();
				const float y1 = myNodes[j]->GetLon();
				LLtoUTM(x1, y1, northings, Easting);
				counter++;
			}
			if (myArcs[i]->GetEndRef() == myNodes[j]->GetRefNum())
			{
				const float x2 = myNodes[j]->GetLat();
				const float y2 = myNodes[j]->GetLon();
				LLtoUTM(x2, y2, northings2, eastings2);
				counter++;
			}
			if (counter == 2)
			{
				if (maxdistance > prevMaxDisance)
				{
					prevMaxDisance = maxdistance;
				}
				maxdistance = Pythagarous(northings, Easting, northings2, eastings2);
				if (maxdistance > prevMaxDisance)
				{
					nodeName1 = myArcs[i]->GetStartRef();
					nodename2 = myArcs[i]->GetEndRef();
				}
				j = 0;
				counter = 0;
				break;
			}
		}
	}
	_outFile << "MaxLink" << endl;
	_outFile << nodename2 << " , " << nodeName1 << " , " << fixed << setprecision(3) << maxdistance << endl;
	_outFile << endl;
	return true;
#pragma endregion
}

bool Navigation::MaxDistance() {
#pragma region MaxDistance
	
	double Northing = 0;
	double Easting = 0;

	double Northing2 = 0;
	double Easting2 = 0;;

	float prevMaxDistanceNum = 0;
	float maxDistanceNum = 0;
	string nodeName1;
	string nodeName2;

	for (int i = 0; i < myNodes.size(); i++) {
		const float x1 = myNodes[i]->GetLat();
		const float y1 = myNodes[i]->GetLon();

		LLtoUTM(x1, y1, Northing, Easting);


		for (int j = i + 1; j < myNodes.size(); j++)
		{
			const float x2 = myNodes[j]->GetLat();
			const float y2 = myNodes[j]->GetLon();
			LLtoUTM(x2, y2, Northing2, Easting2);
			
			if (maxDistanceNum > prevMaxDistanceNum)
			{
				prevMaxDistanceNum = maxDistanceNum;
			}
			maxDistanceNum = Pythagarous(Northing, Easting, Northing2, Easting2);
			if (maxDistanceNum > prevMaxDistanceNum)
			{
				nodeName1 = myNodes[i]->GetPlaceName();
				nodeName2 = myNodes[j]->GetPlaceName();
			}
		}
	}




	_outFile << "MaxDist" << endl;
	_outFile << nodeName1 << "," << nodeName2 << "," << fixed << setprecision(3) << maxDistanceNum << endl;
	_outFile << endl;
	return true;
#pragma endregion
}

int Navigation::findDistanceNode(const int myNodeParameter, const int Arcs) {
	//With this function I get the distance from the starting node to the nodes linked to the current node by its arcs. Now I need to calculate the distance from that node linked to the
	//current node by its arcs to the target node.
#pragma region FindDistance Code
	double eastings;
	double northings;

	double eastings2;
	double northings2;

	double x1;
	double y1;

	double x2;
	double y2;

	int counter = 0;
	float maxDistanceNode = 0;
	maxDistanceVar = 0;

	for (int i = 0; i < myNodes.size(); i++)
	{
		if (myNodes[i]->GetRefNum() == myNodeParameter)
		{
			x1 = myNodes[i]->GetLat();
			y1 = myNodes[i]->GetLon();
			LLtoUTM(x1, y1, northings, eastings);
			counter++;
		}
		if (myNodes[i]->GetRefNum() == Arcs)
		{
			x2 = myNodes[i]->GetLat();
			y2 = myNodes[i]->GetLon();
			LLtoUTM(x2, y2, northings2, eastings2);
			counter++;
		}
		if (counter == 2)
		{
			maxDistanceNode = Pythagarous(northings, eastings, northings2, eastings2);
			distanceNum = maxDistanceNode;
			return maxDistanceNode;
		}

	}
#pragma endregion

}

void Navigation::ModeTransport(const string &modeTransport)const
{
	modeTransportVector.clear();

	if (modeTransport == "Rail")
	{
		modeTransportVector.push_back("Rail");
	}
	else if (modeTransport == "Ship")
	{
		modeTransportVector.push_back("Ship");
	}
	else if (modeTransport == "Bus")
	{
		modeTransportVector.push_back("Bus");
		modeTransportVector.push_back("Ship");
	}
	else if (modeTransport == "Car")
	{
		modeTransportVector.push_back("Car");
		modeTransportVector.push_back("Bus");
		modeTransportVector.push_back("Ship");
	}
	else if (modeTransport == "Bike")
	{
		modeTransportVector.push_back("Ship");
		modeTransportVector.push_back("Bike");
		modeTransportVector.push_back("Rail");
		modeTransportVector.push_back("Bus");
		modeTransportVector.push_back("Car");
	}
	else if (modeTransport == "Foot")
	{
		modeTransportVector.push_back("Bus");
		modeTransportVector.push_back("Foot");
		modeTransportVector.push_back("Bike");
		modeTransportVector.push_back("Car");
		modeTransportVector.push_back("Rail");
		modeTransportVector.push_back("Ship");
	}
	else
	{
		cout << "Mode of transport is not valid" << endl;
	}
}
//This allows me to not repeat any code as the transport method can be called. 
//The revelant method types can be added to the vector depending on what transport has been called.






