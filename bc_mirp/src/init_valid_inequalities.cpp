////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_valid_inequalities.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Valid inequalities initialization functions definitions.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 6, 2021, 10:19 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/init_valid_inequalities.hpp"


void init::validInequalitiesVehicleConstrs(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst)
{
    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getK(); ++k)
        {
            std::ostringstream oss;
            oss << "VI_VC_" << k << "_" << t;
            model.addConstr(y[0][k][t] <= y[0][k - 1][t], oss.str());
        }
    }
}


void init::validInequalitiesVariableReduction(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst)
{
    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int i = 1; i < pInst->getNbVertices(); ++i)
        {
            GRBLinExpr e = 0;
            for (int k = 0; k < pInst->getK(); ++k)
            {
                if (k > i)
                {
                    e += y[i][k][t];
                }

                std::ostringstream oss;
                oss << "VI_VR_" << t << "_" << i << "_" << k;
                model.addConstr(e == 0, oss.str());
            }
        }
    }
}


void init::validInequalitiesHierarchicalConstrs1(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst,
    const int val)
{
    for (int i = 1; i < pInst->getNbVertices() / val; ++i)
    {
        for (int k = 1; k < pInst->getK(); ++k)
        {
            for (int t = 0; t < pInst->getT(); ++t)
            {
                GRBLinExpr e = 0;
                for (int j = 1; j < i; ++j)
                {
                    e += y[j][k - 1][t];
                }

                std::ostringstream oss;
                oss << "VI_HC1_" << (val == 1 ? "full_" : (val == 2 ? "half_" :
                    "quarter_")) << i << "_" << k << "_" << t;
                model.addConstr(y[i][k][t] <= e, oss.str());
            }
        }
    }
}


void init::validInequalitiesHierarchicalConstrs2(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst)
{
    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getK(); ++k)
        {
            for (int i = k; i < pInst->getNbVertices(); ++i)
            {
                for (int l = 0; l < k; ++l)
                {
                    GRBLinExpr e = 0;
                    for (int j = l; j <= l + i - k ; ++j)
                    {
                        e += y[j][l][t];
                    }
                    std::ostringstream oss;
                    oss << "VI_HC2_" << t << "_" << k << "_" << i << "_" << l;
                    model.addConstr(y[i][k][t] <= e, oss.str());
                }
            }
        }
    }
}


void init::validInequalitiesHierarchicalConstrs3(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst)
{
    for (int i = 1; i < pInst->getNbVertices(); ++i)
    {
        for (int t = 0; t < pInst->getT(); ++t)
        {
            for (int k = 0; k < pInst->getK(); ++k)
            {
                GRBLinExpr e = 0;
                for (int j = 0; j < i; ++j)
                {
                    for (int l = 0; l < k; ++l)
                    {
                        e += y[j][l][t];
                    }
                }
                std::ostringstream oss;
                oss << "VI_HC3_" << i << "_" << t << "_" << k;
                model.addConstr(e - (k - 1) * y[i][k][t] >= 0);
            }
        }
    }
}


void init::validInequalitiesCOS(
    GRBModel& model,
    const std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
    const std::shared_ptr<const Instance>& pInst)
{
    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getK(); ++k)
        {
            GRBLinExpr k1 = 0;
            GRBLinExpr k2 = 0;
            for(int i = 0; i < pInst->getNbVertices(); ++i)
            {
                for(int j = i + 1; j < pInst->getNbVertices(); ++j)
                {
                    k1 += pInst->get_cij(i, j) * x[i][j][k - 1][t];
                    k2 += pInst->get_cij(i, j) * x[i][j][k][t];
                }
            }
            std::ostringstream oss;
            oss << "VI_COS_" << t << "_" << k;
            model.addConstr(k1 >= k2, oss.str());
        }
    }
}


void init::validInequalitiesQUA(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& q,
    const std::shared_ptr<const Instance>& pInst)
{
    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getK(); ++k)
        {
            GRBLinExpr q1 = 0;
            GRBLinExpr q2 = 0;
            for(int i = 1; i < pInst->getNbVertices(); ++i)
            {
                q1 += q[i][k - 1][t];
                q2 += q[i][k][t];
            }
            std::ostringstream oss;
            oss << "VI_QUA_" << t << "_" << k;
            model.addConstr(q1 >= q2, oss.str());
        }
    }
}


void init::validInequalitiesCUS(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst)
{
    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getK(); ++k)
        {
            GRBLinExpr y1 = 0;
            GRBLinExpr y2 = 0;
            for(int i = 0; i < pInst->getNbVertices(); ++i)
            {
                y1 += y[i][k - 1][t];
                y2 += y[i][k][t];
            }
            std::ostringstream oss;
            oss << "VI_CUS_" << t << "_" << k;
            model.addConstr(y1 >= y2, oss.str());
        }
    }
}


void init::validInequalitiesLEX(
    GRBModel& model,
    const std::vector<std::vector<std::vector<GRBVar>>>& y,
    const std::shared_ptr<const Instance>& pInst,
    const int val)
{
    for (int t = 0; t < pInst->getT(); ++t)
    {
        for (int k = 1; k < pInst->getK(); ++k)
        {
            GRBLinExpr k1 = 0;
            GRBLinExpr k2 = 0;
            int maxExp = std::floor(pInst->getNbVertices() / val);
            for (int i = 0; i < maxExp; i++)
            {
                k1 += y[i][k - 1][t] * std::pow(2, (maxExp - i) );
                k2 += y[i][k][t] * std::pow(2, (maxExp - i) );
            }
            std::ostringstream oss;
            oss << "VI_LEX_" << (val == 1 ? "full_" : (val == 2 ? "half_" :
                    "quarter_")) << k << "_" << t;
            model.addConstr(k1 >= k2, oss.str());
        }
    }
}