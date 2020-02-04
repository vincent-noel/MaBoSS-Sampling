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
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA 
*/

/*
   Module:
     SamplingParameters.h     
     Authors:
     Vincent Noel <contact@vincent-noel.fr>
     
     Date:
     Dec 2018
*/
#include <iostream>		// std::cout
#include <vector>
#include <map>

#include "BooleanNetwork.h"
#include "BooleanGrammar.h"
#include "RunConfig.h"
#include "MaBEstEngine.h"
#include "PSetSimulation.h"

class SamplingParameters {

    const char * network_file;
    const char * config_file;

    Network * network;

    std::map<std::string, std::vector<double>> param_ranges;
    std::vector<std::string> param_names;

    std::vector<std::map<std::string, double> > conditions;

    std::ostream& output;

    void displayHeader();
    void displayHeaderWithCondition();
    void displayFinalProba(std::map<std::string, double> parameter_set, int parameter_set_id, const std::map<Node *, double> results, const std::map<Node *, double> max_results, PSetSimulation*);
    void displayFinalProbaWithCondition(std::map<std::string, double> parameter_set, int condition_id, int parameter_set_id, const std::map<Node *, double> results, const std::map<Node *, double> max_results, PSetSimulation*);
    std::vector<std::map<std::string, double>> generateCombinations(std::map<std::string, std::vector<double>> param_ranges);

public:

    SamplingParameters(const char * network_file, const char * config_file, std::map<std::string, std::vector<double>> param_ranges, std::ostream& output) :
        network_file(network_file), config_file(config_file), param_ranges(param_ranges), output(output) {
        
        	MaBEstEngine::init();
    
            // Resetting old network variables
            // Probably should be done while we parse the network ?
            // IStateGroup::reset();
            // NodeDecl::reset();

            // Loading the network
            network = new Network();
            network->parse(network_file);

            // Loading the config
            RunConfig * config = new RunConfig();
            config->parse(network, config_file);

            IStateGroup::checkAndComplete(network);
            
            for (const auto& pair : param_ranges)
	        	param_names.push_back(pair.first);
	        
        };

    SamplingParameters(const char * network_file, const char * config_file, std::map<std::string, std::vector<double>> param_ranges, std::vector<std::map<std::string, double> > conditions, std::ostream& output) :
        network_file(network_file), config_file(config_file), param_ranges(param_ranges), output(output), conditions(conditions) {
        
        	MaBEstEngine::init();
    
            // Resetting old network variables
            // Probably should be done while we parse the network ?
            // IStateGroup::reset();
            // NodeDecl::reset();

            // Loading the network
            network = new Network();
            network->parse(network_file);

            // Loading the config
            RunConfig * config = new RunConfig();
            config->parse(network, config_file);

            IStateGroup::checkAndComplete(network);
            
            for (const auto& pair : param_ranges)
	        	param_names.push_back(pair.first);
	        
        };



    ~SamplingParameters();

    int run();
};

