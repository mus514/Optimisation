////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_valid_inequalities.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Valid inequalities initialization functions declarations.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 6, 2021, 10:19 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef INIT_VALID_INEQUALITIES_HPP
#define INIT_VALID_INEQUALITIES_HPP

#include <memory>
#include <gurobi_c++.h>

#include "instance.hpp"

namespace init
{

/**
 * @brief Vehicle Constraints (VC).
*/
void validInequalitiesVehicleConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief variable reduction (VR).
*/
void validInequalitiesVariableReduction(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief Hierarchical constraints Type 1 (HC1).
*/
void validInequalitiesHierarchicalConstrs1(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst,
    const int val);

/**
 * @brief Hierarchical constraints Type 2 (HC2).
*/
void validInequalitiesHierarchicalConstrs2(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief Hierarchical constraints Type 3 (HC3).
*/
void validInequalitiesHierarchicalConstrs3(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief Ordering by routing cost (COS).
*/
void validInequalitiesCOS(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief Ordering by the quantity delivered per route (QUA).
*/
void validInequalitiesQUA(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief Ordering by the number of customers per route (CUS).
*/
void validInequalitiesCUS(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief Lexicographic ordering (LEX).
*/
void validInequalitiesLEX(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst,
    const int val);

} // init namespace

#endif // INIT_VALID_INEQUALITIES_HPP