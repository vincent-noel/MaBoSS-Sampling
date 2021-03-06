#include <stdio.h>
#include <iostream>		// std::cout
#include <fstream>		// std::cout
#include <algorithm>
#include <vector>
#include <string.h>
#include <map>
#include <jsoncpp/json/json.h>

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
  os << "  " << prog << " -c|--config CONF_FILE -s|--settings SETTINGS_FILE] [-o|--output RESULT_FILE] BOOLEAN_NETWORK_FILE\n\n";
  return 1;
}

std::vector<std::string> split(std::string& s) 
{
   std::vector<std::string> tokens;
   std::string token;
   std::istringstream tokenStream(s);
   while (std::getline(tokenStream, token, ','))
   {
      tokens.push_back(token);
   }
   return tokens;
}

std::string trim(std::string& s)
{
    size_t first = s.find_first_not_of(' ');
    size_t last = s.find_last_not_of(' ');
    return s.substr(first, (last-first+1));
}

std::vector<double> readValues(const char *str)
{
    std::vector<std::string> str_values;
	std::string string_str(str);
	str_values = split(string_str);

	std::vector<double> values;
	for (auto& str_value:str_values)
		values.push_back(std::stod(trim(str_value)));

    return values;
}

std::vector<std::string> readParameters(const char *str)
{
    std::vector<std::string> names;
	std::string string_str(str);
	names = split(string_str);

	for (auto& name : names)
		name = trim(name);
	
    return names;
}

std::map<std::string, std::vector<double>> readParametersCSV(const char * ranges_filename) 
{
	std::map<std::string, std::vector<double>> ranges;

	std::ifstream ranges_file;
	ranges_file.open(ranges_filename);

	std::string line = "";
	while (getline(ranges_file, line))
	{
		std::vector<std::string> vec = split(line);
		std::string param_name = vec[0];
		std::vector<double> range;
		for(int i=1; i <vec.size(); i++) {
			range.push_back(stod(vec[i]));
		}
		ranges[param_name] = range;
	}

	ranges_file.close();

	return ranges;	
}


std::pair<std::vector<std::map<std::string, double> >, std::map<std::string, std::vector<double> > > readSettings(const char * settings_filename) {

	std::vector<std::map<std::string, double> > conditions;
	std::map<std::string, std::vector<double> > parameters;


	std::ifstream settings_file(settings_filename);
    Json::Reader reader;
    Json::Value obj;

	reader.parse(settings_file, obj);

	for (const auto& cell_line: obj["cell_lines"]) {

		std::map<std::string, double> res_condition;
		
		for (const auto& condition: cell_line["conditions"]) {
			std::string parameter_name = condition["name"].asString();
			double parameter_value = condition["value"].asDouble();

			res_condition[parameter_name] = parameter_value;
		}

		conditions.push_back(res_condition);
	}

	for (const auto& optim_param: obj["parameters"]) {

		std::string p_name = optim_param["name"].asString();
		std::vector<double> values;
		for (const auto& value:optim_param["values"]) {
			values.push_back(value.asDouble());
		}

		parameters[p_name] = values;
	}

	settings_file.close();

	return std::make_pair(conditions, parameters);
}

int main(int argc, const char * argv[])
{
	const char * config_file = NULL;
	const char * network_file = NULL;
	const char * parameters_filename = NULL;
	const char * values = NULL;
	const char * result_file = NULL;
	const char * ranges_filename = NULL;
	const char * settings_filename = NULL;

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
				parameters_filename = argv[++nn];
			
			} else if (!strcmp(s, "-o") || !strcmp(s, "--output")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				result_file = argv[++nn];

			} else if (!strcmp(s, "-s") || !strcmp(s, "--settings")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				settings_filename = argv[++nn];

			} else if (!strcmp(s, "-i") || !strcmp(s, "--input-ranges")) {
				if (nn == argc-1) {
					std::cerr << '\n' << prog << ": missing value after option " << s << '\n'; return usage();
				}

				ranges_filename = argv[++nn];

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

		std::map<std::string, std::vector<double> > parameters;
		std::vector<std::map<std::string, double> > conditions;

		try {
			MaBEstEngine::init();
	
			if (settings_filename != NULL) {
				std::pair<
					std::vector<std::map<std::string, double> >, 
					std::map<std::string, std::vector<double> >
				> settings = readSettings(settings_filename);

				conditions = settings.first;
				parameters = settings.second;

			} else if (ranges_filename != NULL) {
				parameters = readParametersCSV(ranges_filename);

			} else {

				// Getting the list of parameters
				std::vector<std::string> param_names;
				if (parameters_filename != NULL){
					param_names = readParameters(parameters_filename);
				
				} else {
					// Loading the network
					Network * network = new Network();
					network->parse(network_file);

					// Loading the config
					RunConfig * config = new RunConfig();
					config->parse(network, config_file);

					IStateGroup::checkAndComplete(network);

					// Getting the list of parameters
					SymbolTable* listOfParameters =	network->getSymbolTable();
					param_names = listOfParameters->getSymbolsNames();
				}

				// Initializing the list of values to sample
				std::vector<double> param_values;
				if (values != NULL)
					param_values = readValues(values);
				else
					param_values = {0.01, 0.1, 1};

				for (auto& param_name: param_names)
					parameters[param_name] = param_values;
			}

			if (result_file == NULL){
				SamplingParameters * sampling = new SamplingParameters(network_file, config_file, parameters, conditions, std::cout);
				return sampling->run();
			
			} else {
				std::ofstream output;
				output.open(result_file, std::ofstream::out);
				SamplingParameters * sampling = new SamplingParameters(network_file, config_file, parameters, conditions, output);
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


