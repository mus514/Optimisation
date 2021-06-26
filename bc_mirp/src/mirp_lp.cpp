////////////////////////////////////////////////////////////////////////////////
/*
 * File: mirp_lp.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Classic Inventory-Routing Problem (IRP) linear program [1] class
 * definition.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on October 22, 2020, 06:49 PM.
 * 
 * References:
 * [1] C. Archetti, L. Bertazzi, G. Laporte and M. G. Speranza. A Branch-and-Cut
 * Algorithm for a Vendor-Managed Inventory-Routing Problem. Transportation
 * Science, 41(3), 2007, pp. 382-391.
 */
////////////////////////////////////////////////////////////////////////////////

#include <filesystem>
#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/mirp_lp.hpp"
#include "../include/init_grb_model.hpp"
#include "../include/init_valid_inequalities.hpp"

//////////////////////////////// Helper methods ////////////////////////////////

namespace
{


void initModel(GRBModel& model,
               std::vector<std::vector<GRBVar>>& I,
               std::vector<std::vector<std::vector<GRBVar>>>& q,
               std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
               std::vector<std::vector<std::vector<GRBVar>>>& y,
               std::vector<GRBConstr>& constrs,
               CallbackSEC &CbSEC,
               const std::shared_ptr<const Instance>& pInst,
               const ConfigParameters::model& params)
{
    RAW_LOG_F(INFO, "Building model...");

    try
    {
        /* Initialize variables */
        init::inventoryLevelVariables(model, I, pInst);
        init::quantityVariables(model, q, pInst);
        init::visitationVariables(model, y, pInst);
        init::routingVariables(model, x, pInst);
        
        /* Initialize constraints */
        init::inventoryDefDepotConstrs(model, constrs, I, q, pInst);
        init::inventoryDefCustomersConstrs(model, constrs, I, q, pInst);
        init::inventoryLevelConstrs(model, constrs, I, q, pInst);
        init::quantitiesRoutingConstraint(model, constrs, y, q, pInst);
        init::capacityVehicleConstraint(model, constrs, y, q, pInst);
        init::degreeConstrs(model, constrs, y, x, pInst);
        init::noSplitDelivery(model, y, pInst);

        /* define which policy should be use */
        switch (params.policy)
        {
        case ConfigParameters::model::policy_opt::ML :
        {
            init::mlQuantityCapacityConstrs(model, constrs, I, q, pInst);
            break;
        }
        case ConfigParameters::model::policy_opt::OU :
        {
            init::mlQuantityCapacityConstrs(model, constrs, I, q, pInst);
            init::ouQuantityCapacityConstrs(model, constrs, I, y, q, pInst);
            break;
        }
        default:
        {
            RAW_LOG_F(FATAL, "IRP::initModel(): invalid policy option");
            break;
        }
        }

        switch (params.sec_strategy)
        {
        case ConfigParameters::model::sec_opt::STD :
        {
            init::subtourEliminationConstrs(model, constrs, y, x, pInst);
            break;
        }
        case ConfigParameters::model::sec_opt::CVRPSEP :
        {
            RAW_LOG_F(INFO, "\tusing lazy and cut (CVRPSEP package)");
            model.set(GRB_IntParam_LazyConstraints, 1);
            model.setCallback(&CbSEC);
            break;
        }
        }

        /* valid inequalities */
        if (params.useVIVC_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities VC");
            init::validInequalitiesVehicleConstrs(model, y, pInst);
        }

        if (params.useVIVR_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities VR");
            init::validInequalitiesVariableReduction(model, y, pInst);
        }

        if (params.useVIHC1Full_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities HC1-Full");
            init::validInequalitiesHierarchicalConstrs1(model, y, pInst, 1);
        }

        if (params.useVIHC1Half_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities HC1-Half");
            init::validInequalitiesHierarchicalConstrs1(model, y, pInst, 2);
        }

        if (params.useVIHC1Quarter_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities HC1-Quarter");
            init::validInequalitiesHierarchicalConstrs1(model, y, pInst, 4);
        }

        if (params.useVIHC2_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities HC2");
            init::validInequalitiesHierarchicalConstrs2(model, y, pInst);
        }

        if (params.useVIHC3_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities HC3");
            init::validInequalitiesHierarchicalConstrs3(model, y, pInst);
        }

        if (params.useVICOS_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities COS");
            init::validInequalitiesCOS(model, x, pInst);
        }

        if (params.useVIQUA_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities QUA");
            init::validInequalitiesQUA(model, q, pInst);
        }

        if (params.useVICUS_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities CUS");
            init::validInequalitiesCUS(model, y, pInst);
        }

        if (params.useVILEXFull_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities LEX-Full");
            init::validInequalitiesLEX(model, y, pInst, 1);
        }

        if (params.useVILEXHalf_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities LEX-Half");
            init::validInequalitiesLEX(model, y, pInst, 2);
        }

        if (params.useVILEXQuarter_)
        {
            DRAW_LOG_F(INFO, "\tinitializing valid inequalities LEX-Quarter");
            init::validInequalitiesLEX(model, y, pInst, 4);
        }
    }
    catch (GRBException e)
    {
        RAW_LOG_F(FATAL, "IRP::initModel(): error code: %d", e.getErrorCode());
        RAW_LOG_F(FATAL, "IRP::initModel(): C-Exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "IRP::initModel(): unknown Exception");
    }
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

Mirp_lp::Mirp_lp(const std::shared_ptr<const Instance>& pInst,
               const ConfigParameters::model& params) :
    mpInst(pInst),
    mModel(mEnv),
    mCbSEC(m_q, m_x, m_y, pInst)
{
    initModel(mModel, mI, m_q, m_x, m_y, mConstrs, mCbSEC, mpInst, params);
}


bool Mirp_lp::solve(const ConfigParameters::solver& params)
{
    RAW_LOG_F(INFO, "Solving IRP LP...\n%s", std::string(80, '-').c_str());
    bool solved = true;

    try
    {
        // set solver parameters
        mModel.set(GRB_IntParam_OutputFlag, params.show_log);
        mModel.set(GRB_DoubleParam_TimeLimit, params.time_limit);
        mModel.set(GRB_IntParam_Threads, params.nb_threads);
        mModel.set(GRB_StringParam_LogFile, params.logFile_);

        mModel.optimize();

        if (mModel.get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
        {
            solved = false;
        }
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "IRP::solve(): error code: %d", e.getErrorCode());
        RAW_LOG_F(FATAL, "IRP::solve(): C-Exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "IRP::solve(): unknown Exception");
    }

    return solved;
}


void Mirp_lp::writeIis(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + "_irp.ilp";

    try
    {
        mModel.computeIIS();
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "writeIis() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "writeIis(): Unknown Exception");
    }
}


void Mirp_lp::writeModel(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + "_irp.lp";

    try
    {
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "writeModel() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "writeModel(): Unknown Exception");
    }
}


void Mirp_lp::writeResultsJSON(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + ".json";

    try
    {
        mModel.set(GRB_IntParam_JSONSolDetail, 1);
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "writeSolution() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "writeSolution(): Unknown Exception");
    }
}


void Mirp_lp::writeSolution(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + ".sol";

    try
    {
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "writeSolution() exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "writeSolution(): Unknown Exception");
    }
}