////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_grb_model.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Gurobi's model initialization helper functions declarations.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 9, 2021, 3:06 PM.
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef INIT_GRB_MODE_HPP
#define INIT_GRB_MODE_HPP

#include <memory>

#include "gurobi_c++.h"

#include "instance.hpp"

namespace init
{

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
*/
void inventoryLevelVariables(GRBModel& model,
                             std::vector<std::vector<GRBVar>>& I,
                             const std::shared_ptr<const Instance>& pInst);

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
*/
void quantityVariables(GRBModel& model,
                       std::vector<std::vector<std::vector<GRBVar>>>& q,
                       const std::shared_ptr<const Instance>& pInst);

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
*/
void routingVariables(
    GRBModel& model,
    std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
*/
void clusterVariables(GRBModel& model,
                      std::vector<std::vector<std::vector<GRBVar>>>& k,
                      const std::shared_ptr<const Instance>& pInst);

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
*/
void inventoryDefDepotConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void inventoryDefCustomersConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void inventoryLevelConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void mlQuantityCapacityConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief:.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void ouQuantityCapacityConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: These constraints indicate that the amount delivered to any visited
 * customer has to respect its capacity.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void amountDeliveryConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: These constraints ensure that the capacity of the vehicle is
 * respected.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void capacityVehicleConstraint(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: The vehicles leave from the depot and return to it after vising a
 * customer.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void leaveReturnDepotConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: Degree constraints.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void degreeConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: Each customer can be assigned only to one cluster.
 * @param:.
 * @param:.
 * @param:.
*/
void singleClstAssignConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: Number of clusters needs to be at most equal to the total number of
 * available vehicles.
 * @param:.
 * @param:.
 * @param:.
*/
void nbClstLeqNbAvaiableVehicleConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: No visit and no delivery can take place for all customers with an
 * index less than the vehicle index.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void noVisitNoDeliveryVehicleIdxConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: Each arc (i, j) can belong to only one cluster.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void eachArcIn1ClstConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::shared_ptr<const Instance>& pInst);

/**
 * @brief: Customer i always belongs to a cluster in which customer k is the
 * smallest indexed customer.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void customerInClstKSmallestIdxConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst);

} // init namespace

#endif // INIT_GRB_MODE_HPP