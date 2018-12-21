#include <stdio.h>
#include <iostream>		// std::cout
#include <fstream>		// std::cout
#include <algorithm>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <string.h>
#include <map>

#include "SamplingParameters.h"
#include "RunConfig.h"
#include "BooleanNetwork.h"
#include "MaBEstEngine.h"

const char* prog = "MaBoSS-Sampling";

static int usage(std::ostream& os = std::cerr)
{
  os << "\nUsage:\n\n";
  os << "  " << prog << " [-h|--help]\n\n";
  os << "  " << prog << " -c|--config CONF_FILE [-p|--params '$p1, $p2'] [-v|--values '0.1, 1, 10'] [-o|--output RESULT_FILE] BOOLEAN_NETWORK_FILE\n\n";
  os << "  " << prog << " -c|--config CONF_FILE [-i|--input-ranges INPUT_RANGES_FILE] [-o|--output RESULT_FILE] BOOLEAN_NETWORK_FILE\n\n";
  return 1;
}

std::vector<double> readValues(const char *str)
{
    std::vector<std::string> str_values;
	boost::algorithm::split(str_values, str, boost::is_any_of(","));

	std::vector<double> values;
	for (auto& str_value:str_values) {
		boost::algorithm::trim(str_value);
		values.push_back(std::stod(str_value));
	}

    return values;
}

std::vector<std::string> readParameters(const char *str)
{
    std::vector<std::string> names;
	boost::algorithm::split(names, str, boost::is_any_of(","));

	for (auto& name : names){
		boost::algorithm::trim(name);
	}
    return names;
}

std::map<std::string, std::vector<double>> readParametersCSV(const char * ranges_filename) {

	std::map<std::string, std::vector<double>> ranges;

	std::ifstream ranges_file;
	ranges_file.open(ranges_filename);

	std::string line = "";
	// Iterate through each line and split the content using delimeter
	while (getline(ranges_file, line))
	{
		std::vector<std::string> vec;
		boost::algorithm::split(vec, line, boost::is_any_of(","));
		std::string param_name = vec[0];
		std::vector<double> range;
		for(int i=1; i <vec.size(); i++) {
			range.push_back(stod(vec[i]));
		}
		ranges[param_name] = range;
	}

	// Close the File
	ranges_file.close();

	return ranges;	
}

int main(int argc, const char * argv[])
{
	const char * config_file = NULL;
	const char * network_file = NULL;
	const char * parameters = NULL;
	const char * values = NULL;
	const char * result_file = NULL;
	const char * ranges_filename = NULL;
	std::map<std::string, std::vector<double>> ranges;

	for (int nn = 1; nn < argc; ++nn) {
		const char* s = argv[nn];
		
		if (s[0] == '-') {
		
			if (!strcmp(s, "-c") || !strcmp(s, "--config")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				config_file = argv[++nn];

			} else if (!strcmp(s, "-v") || !strcmp(s, "--values")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				values = argv[++nn];

			} else if (!strcmp(s, "-p") || !strcmp(s, "--params")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}
				parameters = argv[++nn];
			
			} else if (!strcmp(s, "-o") || !strcmp(s, "--output")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				result_file = argv[++nn];

			} else if (!strcmp(s, "-i") || !strcmp(s, "--input-ranges")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				ranges_filename = argv[++nn];
				ranges = readParametersCSV(ranges_filename);

			} else if (!strcmp(s, "--help") || !strcmp(s, "-h")) {
				return usage();
			
			} else {
				std::cerr << '\n' << prog << ": unknown option " << s << std::endl;
				return usage();
			}
		} else if (network_file == NULL) {
			network_file = argv[nn];
		} else {
			std::cerr << '\n' << prog << ": boolean network file is already set to " << network_file << " [" << s << "]" << std::endl;
			return usage();
		}
	}

	if (config_file != NULL && network_file != NULL) 
	{
		try {
			MaBEstEngine::init();
	
			if (ranges_filename == NULL) {

				// Getting the list of parameters
				std::vector<std::string> param_names;
				if (parameters != NULL){
					param_names = readParameters(parameters);
				
				} else {
					// Loading the network
					Network * network = new Network();
					network->parse(network_file);

					// Loading the config
					RunConfig * config = RunConfig::getInstance();
					config->parse(network, config_file);

					IStateGroup::checkAndComplete(network);

					// Getting the list of parameters
					SymbolTable* listOfParameters =	SymbolTable::getInstance();
					param_names = listOfParameters->getSymbolsNames();
				}

				// Initializing the list of values to sample
				std::vector<double> param_values;
				if (values != NULL)
					param_values = readValues(values);
				else
					param_values = {0.01, 0.1, 1};

				for (auto& param_name: param_names)
					ranges[param_name] = param_values;
			}

			if (result_file == NULL){
				SamplingParameters * sampling = new SamplingParameters(network_file, config_file, ranges, std::cout);
				return sampling->run();
			
			} else {
				std::ofstream output;
				output.open(result_file, std::ofstream::out);
				SamplingParameters * sampling = new SamplingParameters(network_file, config_file, ranges, output);
				int res = sampling->run();
				output.close();
				return res;
			}
		}

		catch(const BNException& e) {
			std::cerr << prog << ": " << e;
			return 1;
		}

}
	else return usage();
}


