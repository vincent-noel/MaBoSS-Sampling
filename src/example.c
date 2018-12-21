#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "RunConfig.h"
#include "BooleanNetwork.h"
#include "BooleanGrammar.h"
#include "MaBEstEngine.h"

#define BND_PATH "examples/Four_cycle.bnd"
#define CFG_PATH "examples/Four_cycle.cfg"

int main(int argc, const char * argv[])
{

	std::ofstream null_stream(nullptr);

	MaBEstEngine::init();

	Network * network = new Network();
	network->parse(BND_PATH);

	RunConfig * config = RunConfig::getInstance();
	config->parse(network, CFG_PATH);

    IStateGroup::checkAndComplete(network);

	MaBEstEngine * simulation = new MaBEstEngine(network, config);
	simulation->run(NULL);

	std::cout << "> First, with CFG parameter values" << std::endl;
	simulation->display(null_stream, null_stream, std::cout);
	std::cout << std::endl;

	// Getting list of parameters 
	SymbolTable* listOfParameters = SymbolTable::getInstance();
	std::vector<std::string> param_names = listOfParameters->getSymbolsNames();

	// Using parameter set
	for (auto const & param_name : param_names) {
		const Symbol * param = listOfParameters->getSymbol(param_name);

		if (param == NULL)
			throw BNException("parameter " + (param_name) + " not defined in the config file");

		// std::cout << "Changing parameter " << param_name << " value from " << listOfParameters->getSymbolValue(param) << " to ";
		listOfParameters->setSymbolValue(param, 0);
		// std::cout << listOfParameters->getSymbolValue(param) << std::endl;
	}

	// Running the simulation
	simulation = new MaBEstEngine(network, config);
	simulation->run(NULL);
	
	std::cout << "> Then, without reloading of the model, with all parameters set to 0" << std::endl;
	simulation->display(null_stream, null_stream, std::cout);
	std::cout << std::endl;

  	// Resetting old network variables
    // Probably should be done while we parse the network ?
    IStateGroup::reset();
    NodeDecl::reset();

	network = new Network();
	network->parse(BND_PATH);

	config = RunConfig::getInstance();
	config->parse(network, CFG_PATH);

    IStateGroup::checkAndComplete(network);
	
	// Getting list of parameters 
	listOfParameters = SymbolTable::getInstance();
	param_names = listOfParameters->getSymbolsNames();

	// Using parameter set
	for (auto const & param_name : param_names) {
		const Symbol * param = listOfParameters->getSymbol(param_name);

		if (param == NULL)
			throw BNException("parameter " + (param_name) + " not defined in the config file");

		// std::cout << "Changing parameter " << param_name << " value from " << listOfParameters->getSymbolValue(param) << " to ";
		listOfParameters->setSymbolValue(param, 0);
		// std::cout << listOfParameters->getSymbolValue(param) << std::endl;
	}

	// Running the simulation
	simulation = new MaBEstEngine(network, config);
	simulation->run(NULL);
	
	std::cout << "> Finally, with reloading of the model, with all parameters set to 0" << std::endl;
	simulation->display(null_stream, null_stream, std::cout);

	return 0;
}


