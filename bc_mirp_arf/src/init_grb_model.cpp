////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_grb_model.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Gurobi's model initialization helper functions declarations.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 9, 2021, 3:09 PM.
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/init_grb_model.hpp"


void init::inventoryLevelVariables(GRBModel& model,
                                   std::vector<std::vector<GRBVar>>& I,
                                   const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing inventory level variables (I_it)");

    I.reserve(pInst->getNbVertices());
    for (auto i = 0; i < pInst->getNbVertices(); ++i)
    {
        I.push_back(std::vector<GRBVar>());
        I[i].reserve(pInst->getT() + 1);
        for (auto t = 0; t <= pInst->getT(); ++t)
        {
            std::ostringstream oss;
            oss << "I_" << i << "_" << t;
            I[i].push_back(model.addVar(0,
                                        i == 0 ? GRB_INFINITY : pInst->getUi(i),
                                        pInst->get_hi(i),
                                        GRB_CONTINUOUS,
                                        oss.str()));
        }
    }
}


void init::quantityVariables(GRBModel& model,
                             std::vector<std::vector<std::vector<GRBVar>>>& q,
                             const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing quantity delivered variables (q_ikt)");

    q.reserve(pInst->getNbVertices());
    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        q.push_back(std::vector<std::vector<GRBVar>>());
        q[i].reserve(pInst->getNbVertices());

        for (int k = 0; k < pInst->getNbVertices(); ++k)
        {
            q[i].push_back(std::vector<GRBVar>(pInst->getT()));

            if (i == 0 || k == 0) continue;

            for (int t = 0; t < pInst->getT(); ++t)
            {
                std::ostringstream oss;
                oss << "q_" << i << "_" << k << "_" << t;
                q[i][k][t] = model.addVar(0,
                                          GRB_INFINITY,
                                          0,
                                          GRB_CONTINUOUS,
                                          oss.str());
            }
        }
    }
}


void init::routingVariables(
    GRBModel& model,
    std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing routing variables (x_ijkt)");

    x.reserve(pInst->getNbVertices());
    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        x.push_back(std::vector<std::vector<std::vector<GRBVar>>>());
        x[i].reserve(pInst->getNbVertices());
        for (int j = 0; j < pInst->getNbVertices(); ++j)
        {
            x[i].push_back(std::vector<std::vector<GRBVar>>());
            x[i][j].reserve(pInst->getNbVertices());

            for (int k = 0; k < pInst->getNbVertices(); ++k)
            {
                x[i][j].push_back(std::vector<GRBVar>(pInst->getT()));
                if (k == 0 || i >= j) continue;

                for (int t = 0; t < pInst->getT(); ++t)
                {
                    std::ostringstream oss;
                    oss << "x_" << i << "_" << j << "_" << k << "_" << t;
                    x[i][j][k][t] = model.addVar(0,
                                                 (i == 0 ? 2 : 1),
                                                 pInst->getCij(i, j),
                                                 GRB_INTEGER,
                                                 oss.str());
                }
            }
        }
    }
}


void init::clusterVariables(GRBModel& model,
                            std::vector<std::vector<std::vector<GRBVar>>>& v,
                            const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing cluster variables (v_ikt)");

    v.reserve(pInst->getNbVertices());
    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        v.push_back(std::vector<std::vector<GRBVar>>());
        v[i].reserve(pInst->getNbVertices());
        for (int k = 0; k < pInst->getNbVertices(); ++k)
        {
            v[i].push_back(std::vector<GRBVar>(pInst->getT()));

            if (k == 0 || i == 0) continue;

            for (int t = 0; t < pInst->getT(); ++t)
            {
                std::ostringstream oss;
                oss << "v_" << i << "_" << k << "_" << t;
                v[i][k][t] = model.addVar(0, 1, 0, GRB_BINARY, oss.str());
            }
        }
    }
}


void init::inventoryDefDepotConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing inventory definition depot constraints");

    model.addConstr(I[0][0] == pInst->getIi0(0), "1C_0");
    for (int t = 1; t <= pInst->getT(); ++t)
    {
        GRBLinExpr e = I[0][t - 1] + pInst->get_rit(0, t - 1);
        for (int i = 1; i < pInst->getNbVertices(); ++i) // skip depot
        {
            for (int k = 1; k < pInst->getNbVertices(); ++k)
            {
                e -= q[i][k][t - 1];
            }
        }

        std::ostringstream oss;
        oss << "1C_" << t;
        model.addConstr(I[0][t] == e, oss.str());
    }
}


void init::inventoryDefCustomersConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing inventory definition customers constrs");

    for (int i = 1; i < pInst->getNbVertices(); ++i)
    {
        std::ostringstream oss;
        oss << "3C_" << i;
        model.addConstr(I[i][0] == pInst->getIi0(i), oss.str());

        for (int t = 1; t <= pInst->getT(); ++t)
        {
            GRBLinExpr e = 0;
            for (int k = 1; k < pInst->getNbVertices(); ++k)
            {
                e += q[i][k][t - 1];
            }

            e += I[i][t - 1] - pInst->get_rit(i, t - 1);

            oss.clear();
            oss.str("");
            oss << "3C_" << i << "_" << t;
            model.addConstr(I[i][t] == e, oss.str());
        }
    }
}


void init::inventoryLevelConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing inventory level constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        GRBLinExpr e = 0;
        for (int i = 1; i < pInst->getNbVertices(); ++i)
        {
            for (int k = 1; k < pInst->getNbVertices(); ++k)
            {
                e += q[i][k][t];
            }
        }

        std::ostringstream oss;
        oss << "5C_" << t;
        model.addConstr(I[0][t] >= e, oss.str());
    }
}


void init::mlQuantityCapacityConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing ML quantity capacity constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int i = 1; i < pInst->getNbVertices(); ++i)
        {
            GRBLinExpr e = 0;
            for (int k = 1; k < pInst->getNbVertices(); ++k)
            {
                e += q[i][k][t];
            }

            std::ostringstream oss;
            oss << "6C_ML" << i << "_" << t;
            model.addConstr(e <= pInst->getUi(i) - I[i][t], oss.str());
        }
    }
}


void init::ouQuantityCapacityConstrs(
    GRBModel& model,
    const std::vector<std::vector<GRBVar>>& I,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tOU quantity capacity constraints");

    for (auto i = 1; i < pInst->getNbVertices(); ++i)
    {
        for (auto t = 0; t < pInst->getT(); ++t)
        {
            GRBLinExpr e1 = 0;
            GRBLinExpr e2 = 0;
            for (int k = 1; k < pInst->getNbVertices(); ++k)
            {
                e1 += q[i][k][t];
                e2 += v[i][k][t];
            }

            std::ostringstream oss;
            oss << "6C_OU" << i << "_" << t;
            model.addConstr(e1 >= pInst->getUi(i) * e2 - I[i][t], oss.str());
        }
    }
}


void init::amountDeliveryConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing amount delivered to customers constrs");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int i = 1; i < pInst->getNbVertices(); ++i)
        {
            for (int k = 1; k < pInst->getNbVertices(); ++k)
            {
                std::ostringstream oss;
                oss << "7C_" << i << "_" << k << "_" << t;
                model.addConstr(q[i][k][t] <= pInst->getUi(i) * v[i][k][t],
                                oss.str());
            }
        }
    }
}


void init::capacityVehicleConstraint(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing capacity vehicles constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getNbVertices(); ++k)
        {
            GRBLinExpr e = 0;
            for (int i = 1; i < pInst->getNbVertices(); ++i)
            {
                e += q[i][k][t];
            }

            std::ostringstream oss;
            oss << "8C_" << k << "_" << t;
            // assumes that the fleet is homogeneous. Then "getCk(0)"
            model.addConstr(e <= pInst->getCk(0) * v[k][k][t], oss.str());
        }
    }
}


void init::leaveReturnDepotConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing leave-return depot constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getNbVertices(); ++k)
        {
            GRBLinExpr e = 0;
            for (int j = 1; j < pInst->getNbVertices(); ++j)
            {
                e += x[0][j][k][t];
            }

            std::ostringstream oss;
            oss << "9C_" << k << "_" << t;
            model.addConstr(e == 2 * v[k][k][t], oss.str());
        }
    }
}


void init::degreeConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing degree constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getNbVertices(); ++k)
        {
            for (int i = 1; i < pInst->getNbVertices(); ++i)
            {
                GRBLinExpr e1 = 0;
                for (int j = i + 1; j < pInst->getNbVertices(); ++j)
                {
                    e1 += x[i][j][k][t];
                }

                GRBLinExpr e2 = 0;
                for (int j = 0; j < i; ++j)
                {
                    e2 += x[j][i][k][t];
                }

                std::ostringstream oss;
                oss << "10C_" << i << "_" << k << "_" << t;
                model.addConstr(e1 + e2 == 2 * v[i][k][t], oss.str());
            }
        }
    }
}


void init::singleClstAssignConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing single cluster assignment constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int i = 1; i < pInst->getNbVertices(); ++i)
        {
            GRBLinExpr e = 0;
            for (int k = 1; k < pInst->getNbVertices(); ++k)
            {
                e += v[i][k][t];
            }

            std::ostringstream oss;
            oss << "11C_" << i << "_" << t;
            model.addConstr(e <= 1, oss.str());
        }
    }
}


void init::nbClstLeqNbAvaiableVehicleConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO, "\tinitializing # clst <= # K constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        GRBLinExpr e = 0;
        for (int k = 1; k < pInst->getNbVertices(); ++k)
        {
            e += v[k][k][t];
        }

        std::ostringstream oss;
        oss << "12C_" << "_" << t;
        model.addConstr(e <= pInst->getK(), oss.str());
    }
}


void init::noVisitNoDeliveryVehicleIdxConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO,"\tinitializing no visit/delivery vehicle idx constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int i = 1; i < pInst->getNbVertices(); ++i)
        {
            for (int k = i + 1; k < pInst->getNbVertices(); ++k)
            {
                std::ostringstream oss;
                oss << "13C_" << i << "_" << k << "_" << t;
                model.addConstr(v[i][k][t] == 0, oss.str());
                oss.clear();
                oss.str("");
                oss << "14C_" << i << "_" << k << "_" << t;
                model.addConstr(q[i][k][t] == 0, oss.str());
                oss.clear();
                oss.str("");
                oss << "15C_" << i << "_" << k << "_" << t;
                model.addConstr(x[0][i][k][t] == 0, oss.str());

                for (int j = i + 1; j < pInst->getNbVertices(); ++j)
                {
                    oss.clear();
                    oss.str("");
                    oss << "16C_" << i << "_" << j << "_" << k << "_" << t;
                    model.addConstr(x[i][j][k][t] == 0, oss.str());
                }
            }
        }
    }
}


void init::eachArcIn1ClstConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO,"\tinitializing each arc at most in 1 cluster constraints");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getNbVertices(); ++k)
        {
            for (int i = 1; i < pInst->getNbVertices(); ++i)
            {
                for (int j = i + 1; j < pInst->getNbVertices(); ++j)
                {
                    std::ostringstream oss;
                    oss << "17C_" << i << "_" << j << "_" << k << "_" << t;
                    model.addConstr(x[i][j][k][t] <= 1, oss.str());
                }
            }
        }
    }
}


void init::customerInClstKSmallestIdxConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& v,
    const std::shared_ptr<const Instance>& pInst)
{
    DRAW_LOG_F(INFO,"\tinitializing customer in clst k smallest idx constrs");

    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int i = 1; i < pInst->getNbVertices(); ++i)
        {
            for (int k = 1; k < pInst->getNbVertices(); ++k)
            {
                std::ostringstream oss;
                model.addConstr(v[i][k][t] - v[k][k][t] <= 0, oss.str());
                oss << "18C_" << i << "_" << k << "_" << t;
            }
        }
    }
}