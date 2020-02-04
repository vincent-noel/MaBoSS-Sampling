/* 
   MaBoSS (Markov Boolean Stochastic Simulator)
   Copyright (C) 2011-2018 Institut Curie, 26 rue d'Ulm, Paris, France
   
   MaBoSS is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.
   
   MaBoSS is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along wSymbolTable::getInstance()ith this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA 
*/

/*
   Module:
     SamplingParameters.cc     
     Authors:
     Vincent Noel <contact@vincent-noel.fr>
     
     Date:
     Dec 2018
*/
#include <stdio.h>
#include <iostream>		// std::cout
#include <iomanip>      // std::setprecision
#include <vector>

#include "SamplingParameters.h"

void SamplingParameters::displayHeader() {
	
	output << "ParameterSet Id\t";

	const std::vector<Node*>& nodes = network->getNodes();

	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		output << (*it)->getLabel() << "\t";
	}

	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		output << "max(" << (*it)->getLabel() << ")\t";
	}

	for (std::vector<std::string>::const_iterator it = param_names.begin(); it != param_names.end(); it++) {
		output << *it;

		if (std::next(it) != param_names.end()) 
			output << "\t";
	}

	output << "\n";

}

void SamplingParameters::displayHeaderWithCondition() {
	
	output << "Condition Id\tParameterSet Id\t";

	const std::vector<Node*>& nodes = network->getNodes();

	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		output << (*it)->getLabel() << "\t";
	}

	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		output << "max(" << (*it)->getLabel() << ")\t";
	}

	for (std::vector<std::string>::const_iterator it = param_names.begin(); it != param_names.end(); it++) {
		output << *it;

		if (std::next(it) != param_names.end()) 
			output << "\t";
	}

	output << "\n";

}

void SamplingParameters::displayFinalProba(std::map<std::string, double> parameter_set, int parameter_set_id, const std::map<Node *, double> results, const std::map<Node *, double> max_results, PSetSimulation * simulation) {

	const std::vector<Node*>& nodes = simulation->getNodes();

	output << parameter_set_id << "\t";

	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		output << results.at(*it) << "\t";

	}
	
	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		output << max_results.at(*it) << "\t";

	}
		
	for (std::map<std::string, double>::const_iterator it = parameter_set.begin(); it != parameter_set.end(); it++) {
		output << it->second;

		if (std::next(it) != parameter_set.end()) {
			output << "\t";
		}
	}

	output << std::endl;
} 


void SamplingParameters::displayFinalProbaWithCondition(std::map<std::string, double> parameter_set, int condition_id, int parameter_set_id, const std::map<Node *, double> results, const std::map<Node *, double> max_results, PSetSimulation * simulation) {

	const std::vector<Node*>& nodes = simulation->getNodes();

	output << condition_id << "\t" << parameter_set_id << "\t";

	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		output << results.at(*it) << "\t";
	}
	
	for (std::vector<Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
		output << max_results.at(*it) << "\t";
	}

	for (std::map<std::string, double>::const_iterator it = parameter_set.begin(); it != parameter_set.end(); it++) {
		output << it->second;

		if (std::next(it) != parameter_set.end()) {
			output << "\t";
		}
	}

	output << std::endl;
} 

std::vector<std::map<std::string, double>> SamplingParameters::generateCombinations(std::map<std::string, std::vector<double>> param_ranges) {

	std::vector<std::string> names;
	std::vector<std::vector<double>> values;

	for (const auto& pair : param_ranges) {
		names.push_back(pair.first);
		values.push_back(pair.second);
	}


    std::vector<std::vector<double>> s = {{}};
    for (const auto& u : values) {
        std::vector<std::vector<double>> r;
        for (const auto& x : s) {
            for (const auto y : u) {
                r.push_back(x);
                r.back().push_back(y);
            }
        }
        s = move(r);
    }

	std::vector<std::map<std::string, double>> result;

	for (auto p_set : s) {
		std::map<std::string, double> result_set;

		int i=0;
		for (auto const& value : p_set) {
			result_set[names[i]] = value;
			i++;
		}
		result.push_back(result_set);
	}

    return result;
}

int SamplingParameters::run()
{
	MaBEstEngine::init();
	// Resetting old network variables
	// Probably should be done while we parse the network ?
	// IStateGroup::reset();
	// NodeDecl::reset();

	// Loading the network
	Network * network = new Network();
	network->parse(network_file);

	// Loading the config
	RunConfig * config = new RunConfig();
	config->parse(network, config_file);

    IStateGroup::checkAndComplete(network);

	std::vector<std::map<std::string, double>> parameter_sets = generateCombinations(param_ranges);

	if (conditions.size() > 0) {
		displayHeaderWithCondition();

		int c=0;
		for (const auto& condition: conditions) {
		
			// Sampling the parameter values for all parameters
			int i=0;
			for (std::vector<std::map<std::string, double>>::const_iterator it = parameter_sets.begin(); it != parameter_sets.end(); it++){

				std::map<std::string, double> pset = *it;

				for (const auto& initial_value: condition) {
					pset[initial_value.first] = initial_value.second;
				}

				PSetSimulation * pset_simulation = new PSetSimulation(network_file, config_file, pset);
				pset_simulation->run();

				const std::map<Node *, double> results = pset_simulation->getLastNodesDist();
				const std::map<Node *, double> max_results = pset_simulation->getMaxNodesDist();
				
				displayFinalProbaWithCondition(*it, c, i, results, max_results, pset_simulation);
				delete pset_simulation;
				i++;
			}

			c++;
		}

	} else {

		displayHeader();

		// Sampling the parameter values for all parameters
		int i=0;
		for (std::vector<std::map<std::string, double>>::const_iterator it = parameter_sets.begin(); it != parameter_sets.end(); it++){

			PSetSimulation * pset_simulation = new PSetSimulation(network_file, config_file, *it);
			pset_simulation->run();

			const std::map<Node *, double> results = pset_simulation->getLastNodesDist();
			const std::map<Node *, double> max_results = pset_simulation->getMaxNodesDist();
			
			displayFinalProba(*it, i, results, max_results, pset_simulation);
			delete pset_simulation;
			i++;
		}
	}

	return 0;
}