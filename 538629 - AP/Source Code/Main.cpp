// #######################################################################################
// ##                                                                                   ##
// ##  This main.cpp file WILL BE REPLACED during the marking procedure.                ##
// ##  Therefore all of your implementation code MUST be in the Navigation class,       ##
// ##  or any other new classes that you create and use inside of the Navigation class. ##
// ##                                                                                   ##
// #######################################################################################

#include "Navigation.h"
#include <iostream>
#include <iomanip>

#include "ACW_Wrapper.h"

static void executeACW() {
	ACW_Wrapper wrapper("log.txt");

	// Build Navigation
	wrapper.startTimer();
	Navigation nav;
	if (nav.BuildNetwork("Places.csv", "Links.csv")) {
		const auto elapsed = wrapper.stopTimer();
		std::cout << std::fixed << std::setprecision(1) << "BuildNetwork - " << elapsed << " microseconds" << std::endl;
	}
	else {
		std::cout << "\n*** Error *** BuildNetwork" << std::endl;
	}

	// Process commands
	std::ifstream fin("commands.txt");
	while (!fin.eof())	{
		std::string command;
		std::getline(fin, command);

		wrapper.startTimer();
		if (nav.ProcessCommand(command)) {
			const auto elapsed = wrapper.stopTimer();
			std::cout << command << std::fixed << std::setprecision(1) << " - " << elapsed << " microseconds" << std::endl;
		}
		else {
			std::cout << "\n*** Error *** " << command << std::endl;
		}
	}
}

int main(int, char**) {
	executeACW();
	system("PAUSE");
}