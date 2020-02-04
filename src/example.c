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
#define OTHER_BND_PATH "examples/Four_cycle_other.bnd"
#define OTHER_CFG_PATH "examples/Four_cycle_other.cfg"

void setParametersTo(Network* network, double value) {
	// Getting list of parameters 
	SymbolTable* listOfParameters = network->getSymbolTable();
	std::vector<std::string> param_names = listOfParameters->getSymbolsNames();

	// Using parameter set
	for (auto const & param_name : param_names) {
		const Symbol * param = listOfParameters->getSymbol(param_name);

		if (param == NULL)
			throw BNException("parameter " + (param_name) + " not defined in the config file");

		// std::cout << "Changing parameter " << param_name << " value from " << listOfParameters->getSymbolValue(param) << " to ";
		listOfParameters->setSymbolValue(param, value);
		// std::cout << listOfParameters->getSymbolValue(param) << std::endl;
	}
}

int main(int argc, const char * argv[])
{
	try
	{
		std::ofstream null_stream(nullptr);

		MaBEstEngine::init();

		Network * network = new Network();
		network->parse(BND_PATH);

		RunConfig * config = new RunConfig();
		config->parse(network, CFG_PATH);

		IStateGroup::checkAndComplete(network);

		MaBEstEngine * simulation = new MaBEstEngine(network, config);
		simulation->run(NULL);

		std::cout << "> First, with CFG parameter values" << std::endl;
		simulation->display(null_stream, null_stream, std::cout);
		std::cout << std::endl;

		setParametersTo(network, 0);

		// Running the simulation
		delete simulation;
		simulation = new MaBEstEngine(network, config);
		simulation->run(NULL);
		
		std::cout << "> Then, without reloading of the model, with all parameters set to 0" << std::endl;
		simulation->display(null_stream, null_stream, std::cout);
		std::cout << std::endl;

		setParametersTo(network, 1);

		// Running the simulation
		delete simulation;
		simulation = new MaBEstEngine(network, config);
		simulation->run(NULL);
		
		std::cout << "> Finally, with reloading of the model, with all parameters set to 0" << std::endl;
		simulation->display(null_stream, null_stream, std::cout);
		std::cout << std::endl;

		// IStateGroup::reset();
		config = new RunConfig();
		config->parse(network, OTHER_CFG_PATH);
		IStateGroup::checkAndComplete(network);

		delete simulation;
		simulation = new MaBEstEngine(network, config);
		simulation->run(NULL);

		std::cout << "> Also, with a new reloaded CFG, with all parameters set to 0" << std::endl;
		simulation->display(null_stream, null_stream, std::cout);
		std::cout << std::endl;

		std::cout << "> Now we try loading a new model, with the initial cfg" << std::endl;

		delete network;
		network = new Network();
		network->parse(OTHER_BND_PATH);
		// IStateGroup::reset();
		config = new RunConfig();
		config->parse(network, CFG_PATH);
		IStateGroup::checkAndComplete(network);

		delete simulation;
		simulation = new MaBEstEngine(network, config);
		simulation->run(NULL);

		std::cout << "> First, w::getInstance()ith CFG parameter values" << std::endl;
		simulation->display(null_stream, null_stream, std::cout);
		std::cout << std::endl;

		// IStateGroup::reset();
		config = new RunConfig();

		config->parse(network, OTHER_CFG_PATH);
		IStateGroup::checkAndComplete(network);

		delete simulation;
		simulation = new MaBEstEngine(network, config);
		simulation->run(NULL);

		std::cout << "> Then, with the other cfg" << std::endl;
		simulation->display(null_stream, null_stream, std::cout);
		std::cout << std::endl;

	}
	catch(const BNException e)
	{
		std::cerr << e.getMessage() << '\n';
	}
	
	return 0;
}


