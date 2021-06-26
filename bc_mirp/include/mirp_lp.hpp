////////////////////////////////////////////////////////////////////////////////
/*
 * File: mirp_lp.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Classic Inventory-Routing Problem (IRP) linear program [1] class
 * declaration.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on October 22, 2020, 06:47 PM.
 * 
 * References:
 * [1] C. Archetti, L. Bertazzi, G. Laporte and M. G. Speranza. A Branch-and-Cut
 * Algorithm for a Vendor-Managed Inventory-Routing Problem. Transportation
 * Science, 41(3), 2007, pp. 382-391.
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef MIRP_LP_HPP
#define MIRP_LP_HPP

#include <memory>
#include <vector>
#include <gurobi_c++.h>

#include "config_parameters.hpp"
#include "instance.hpp"
#include "callback/callback_sec.hpp"

class Mirp_lp
{
public:

    Mirp_lp(const Mirp_lp& other) = default;
    Mirp_lp(Mirp_lp&& other) = default;
    ~Mirp_lp() = default;

    Mirp_lp() = delete;
    Mirp_lp& operator=(const Mirp_lp& other) = delete;
    Mirp_lp& operator=(Mirp_lp&& other) = delete;

    Mirp_lp(const std::shared_ptr<const Instance>& p_inst,
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
    std::vector<GRBConstr> mConstrs;

    // inventory level variables
    std::vector<std::vector<GRBVar>> mI;
    // product quantity shipped to the retailer by vehicle k
    std::vector<std::vector<std::vector<GRBVar>>> m_q;
    /* equal to one if j immediately follows i in the route traveled at time t
       by vehicle k */
    std::vector<std::vector<std::vector<std::vector<GRBVar>>>> m_x;
    // retailer i is served at time t by vehicle k
    std::vector<std::vector<std::vector<GRBVar>>> m_y;

    CallbackSEC mCbSEC;
};

#endif // MIRP_LP_HPP