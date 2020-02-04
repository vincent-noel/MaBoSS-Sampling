# MaBoSS-Sampling [![Build Status](https://travis-ci.org/vincent-noel/MaBoSS-Sampling.svg?branch=master)](https://travis-ci.org/vincent-noel/MaBoSS-Sampling)
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

