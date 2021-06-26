////////////////////////////////////////////////////////////////////////////////
/*
 * File: ConfigParameters.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Configuration input parameters class definition
 * (see ./input/example.cfg).
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 9, 2020, 11:58 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>
#include <fstream>
#include <thread>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/config_parameters.hpp"

//////////////////////////////// Helper methods ////////////////////////////////

namespace
{

/**
 * @brief Strings labels of each parameter.
 */
const std::string cInstancePath = "instance_path";
const std::string cOutputDir = "output_dir";
const std::string cSolverShowLog = "solver_show_log";
const std::string cSolverTimeLimit = "solver_time_limit";
const std::string cSolverNbThreads = "solver_nb_threads";
const std::string cNbVehicles = "nb_vehicles";
const std::string c_model_policy = "model_policy";

/**
 * @brief Parse string to boolean.
 * @param: const std::string &: string to be parsed.
 * @return: bool: parsed value.
 */
bool parseBool(const std::string& str)
{
    return str.compare("true") == 0;
}

/**
 * @brief.
*/
ConfigParameters::model::policy_opt parsePolicyOpt(const std::string &str)
{
    if (std::stoi(str) == 0)
    {
        return ConfigParameters::model::policy_opt::ML;
    }
    else
    {
        CHECK_F(std::stoi(str) == 1);
        return ConfigParameters::model::policy_opt::OU;
    }
}

/**
 * @brief:.
 * @param:.
 * @return:.
*/
std::size_t parseUint(const std::string& str)
{
    int val = std::stoi(str);
    CHECK_F(val >= 0, "Input parameter: Invalid value");
    return static_cast<std::size_t>(val);
}

/**
 * @brief Parse string to unsigned int. It also checks if the input string is
 * set to max. If so, then parse to max value.
 * @param: const std::string &: string to be parsed.
 * @return: std::size_t: parsed value.
*/
std::size_t parseNbThreads(const std::string& str)
{
    if (str == "max")
    {
        return std::thread::hardware_concurrency();
    }

    int val = std::stoi(str);
    CHECK_F(val >= 0, "Input parameter: Invalid value");

    return static_cast<std::size_t>(val);
}

/**
 * @brief.
 * @param:.
 * @return:.
 */
std::map<std::string, std::string> readCfgFile(const std::string &config_path)
{
    std::ifstream file(config_path);
    std::string line;

    std::map<std::string, std::string> data;

    while (std::getline(file, line))
    {
        std::size_t begin = line.find_first_not_of( " \f\t\v" );

        if (begin == std::string::npos)
        {
            continue; // skip blank lines
        }

        if (std::string("#").find(line[begin]) != std::string::npos)
        {
            continue; // skip commentary
        }

        std::size_t end = line.find('=', begin);
        std::string key = line.substr(begin, end - begin);

        // (no leading or trailing whitespace allowed)
        key.erase(key.find_last_not_of( " \f\t\v" ) + 1);

        if (key.empty())
        {
            continue; // no blank keys allowed
        }

        // extract the value (no leading or trailing whitespace allowed)
        begin = line.find_first_not_of(" \f\n\r\t\v", end + 1);
        end   = line.find_last_not_of(" \f\n\r\t\v") + 1;

        std::string value = line.substr(begin, end - begin);

        data[key] = value;
    }

    return data;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

ConfigParameters::ConfigParameters(const std::string config_path)
{
    CHECK_F(std::filesystem::exists(config_path));
    mData = readCfgFile(config_path);
    setupParameters();
}


std::string ConfigParameters::getInstancePath() const
{
    return mInstancePath;
}


std::string ConfigParameters::getOutputDir() const
{
    return mOutputDir;
}


ConfigParameters::model ConfigParameters::getModelParams() const
{
    return mModelParam;
}


ConfigParameters::solver ConfigParameters::getSolverParams() const
{
    return mSolverParam;
}


void ConfigParameters::setLogFilePath(const std::string& path)
{
    mSolverParam.logFile_ = path;
}


void ConfigParameters::show() const
{
    RAW_LOG_F(INFO, std::string(80, '-').c_str());
    RAW_LOG_F(INFO, "Parameters");
    for (auto &e : mData)
    {
        std::ostringstream oss;
        oss << std::setw(40) << e.first << std::setw(40) << e.second;
        RAW_LOG_F(INFO, oss.str().c_str());
    }
    RAW_LOG_F(INFO, std::string(80, '-').c_str());
}

/////////////////////////////// private methods ////////////////////////////////

void ConfigParameters::setupParameters()
{
    // ---- General parameters ----
    mInstancePath = mData[cInstancePath];
    mOutputDir = mData[cOutputDir];
    // ---- Solver parameters ----
    mSolverParam.showLog_ = parseBool(mData[cSolverShowLog]);
    mSolverParam.timeLimit_ = parseUint(mData[cSolverTimeLimit]);
    mSolverParam.nbThreads_ = parseNbThreads(mData[cSolverNbThreads]);
    // ---- Model parameters ----
    mModelParam.K_ = parseUint(mData[cNbVehicles]);
    mModelParam.policy_ = parsePolicyOpt(mData[c_model_policy]);
}