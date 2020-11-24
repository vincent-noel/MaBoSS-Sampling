# MaBoSS-Sampling ![Ubuntu 18.04](https://github.com/vincent-noel/MaBoSS-Sampling/workflows/Ubuntu%2018.04/badge.svg)

Parameter sampling for MaBoSS models

## Compiling
```
make
```

## Usage
```
MaBoSS-Sampling -c|--config CONF_FILE [-p|--params '$p1, $p2'] [-v|--values '0.1, 1, 10'] [-o|--output RESULT_FILE] BOOLEAN_NETWORK_FILE

MaBoSS-Sampling -c|--config CONF_FILE [-i|--input-ranges INPUT_RANGES_FILE] [-o|--output RESULT_FILE] BOOLEAN_NETWORK_FILE

MaBoSS-Sampling -c|--config CONF_FILE -s|--settings SETTINGS_FILE] [-o|--output RESULT_FILE] BOOLEAN_NETWORK_FILE

MaBoSS-Sampling [-h|--help]
```

## Dependencies
* g++
* flex
* bison
* JsonCpp

## Installing dependencies
- Ubuntu : 
```
apt install g++ flex bison libjsoncpp-dev 
```
- CentOS : 
```
yum install gcc-c++ flex bison jsoncpp-devel
```

