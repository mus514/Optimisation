////////////////////////////////////////////////////////////////////////////////
/*
 * File: mirp_arf.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Multi-vehicle Inventory-Routing Problem (MIRP) Asymmetric
 * Representatives Formulation (ARF) [1] class declaration.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 9, 2021, 02:01 PM.
 * 
 * References:
 * [1] C. Archetti, L. Bertazzi, G. Laporte and M. G. Speranza. A Branch-and-Cut
 * Algorithm for a Vendor-Managed Inventory-Routing Problem. Transportation
 * Science, 41(3), 2007, pp. 382-391.
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef MIRP_ARF_HPP
#define MIRP_ARF_HPP

#include <memory>
#include <vector>

#include "gurobi_c++.h"

#include "callback/callback_sec.hpp"
#include "config_parameters.hpp"
#include "instance.hpp"

class Mirp_arf
{
public:

    Mirp_arf(const Mirp_arf& other) = default;
    Mirp_arf(Mirp_arf&& other) = default;
    ~Mirp_arf() = default;

    Mirp_arf() = delete;
    Mirp_arf& operator=(const Mirp_arf& other) = delete;
    Mirp_arf& operator=(Mirp_arf&& other) = delete;

    Mirp_arf(const std::shared_ptr<const Instance>& p_inst,
           const ConfigParameters::model& params);

    bool solve(const ConfigParameters::solver& params);

    void writeIis(std::string path);

    void writeModel(std::string path);

    void writeResultsJSON(std::string path);

    void writeSolution(std::string path);

private:

    // pointer to instance
    std::shared_ptr<const Instance> mpInst;

    GRBEnv mEnv;
    GRBModel mModel;

    // inventory level variables
    std::vector<std::vector<GRBVar>> mI;
    /* represent the quantity delivered to customer i belonging to cluster k in
       period t */
    std::vector<std::vector<std::vector<GRBVar>>> m_q;
    /* equal to one if j immediately follows i in the route traveled at time t
       in cluster k */
    std::vector<std::vector<std::vector<std::vector<GRBVar>>>> m_x;
    /* equal to one if customer i belongs to cluster k in period t, i.e.,
       customer i belongs to the cluster in which customer k is the smallest
       indexed customer */
    std::vector<std::vector<std::vector<GRBVar>>> m_v;

    CallbackSEC mCbSEC;
};

#endif // MIRP_ARF_HPP