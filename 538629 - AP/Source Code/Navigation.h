#pragma once

#include <fstream>
#include <string>

class Navigation
{
	std::ofstream _outFile;

	// Add your code here

private:

	float maxDistanceVar = 0;
public:
	Navigation();
	~Navigation();

	bool BuildNetwork(const std::string& fileNamePlaces, const std::string& fileNameLinks)const;
	bool ProcessCommand(const std::string& commandString);
	bool MaxDistance();
	bool MaxLink();
	int findDistanceNode(int myNodeParameter, int Arcs);
	inline double Pythagarous(const double northing, const double easting, const double northing2, const double easting2) 
	{
		float const x_Distance = (easting2 - easting) * (easting2 - easting);
		float const y_Distance = (northing2 - northing) * (northing2 - northing);
		float const distance = x_Distance + y_Distance;
		float const finalDist = sqrt(distance);
		
		if (finalDist / 1000 > maxDistanceVar)
		{
			maxDistanceVar = finalDist / 1000;
		}
		return maxDistanceVar;
	}
	void ModeTransport(const std::string &TransportType)const;
	
	// Add your code here
};