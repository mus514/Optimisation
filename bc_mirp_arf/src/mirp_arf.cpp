////////////////////////////////////////////////////////////////////////////////
/*
 * File: Mirp_arf.cpp
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

#include "../include/mirp_arf.hpp"
#include "../include/init_grb_model.hpp"
// #include "../include/init_valid_inequalities.hpp"

//////////////////////////////// Helper methods ////////////////////////////////

namespace
{


void initModel(GRBModel& model,
               std::vector<std::vector<GRBVar>>& I,
               std::vector<std::vector<std::vector<GRBVar>>>& q,
               std::vector<std::vector<std::vector<std::vector<GRBVar>>>>& x,
               std::vector<std::vector<std::vector<GRBVar>>>& v,
               CallbackSEC &CbSEC,
               const std::shared_ptr<const Instance>& pInst,
               const ConfigParameters::model& params)
{
    RAW_LOG_F(INFO, "Building MIRP-ARF model...");

    try
    {
        /* Initialize variables */
        init::inventoryLevelVariables(model, I, pInst);
        init::quantityVariables(model, q, pInst);
        init::routingVariables(model, x, pInst);
        init::clusterVariables(model, v, pInst);
        
        /* Initialize constraints */
        init::inventoryDefDepotConstrs(model, I, q, pInst);
        init::inventoryDefCustomersConstrs(model, I, q, pInst);
        init::inventoryLevelConstrs(model, I, q, pInst);
        init::amountDeliveryConstrs(model, q, v, pInst);
        init::capacityVehicleConstraint(model, q, v, pInst);
        init::leaveReturnDepotConstrs(model, x, v, pInst);
        init::degreeConstrs(model, x, v, pInst);
        init::singleClstAssignConstrs(model, v, pInst);
        init::nbClstLeqNbAvaiableVehicleConstrs(model, v, pInst);
        init::noVisitNoDeliveryVehicleIdxConstrs(model, x, v, q, pInst);
        init::customerInClstKSmallestIdxConstrs(model, v, pInst);

        /* callback: lazy and cut SEC */
        RAW_LOG_F(INFO, "\tusing lazy and cut (CVRPSEP package)");
        model.set(GRB_IntParam_LazyConstraints, 1);
        model.setCallback(&CbSEC);

        /* define which policy should be use */
        switch (params.policy_)
        {
        case ConfigParameters::model::policy_opt::ML :
        {
            init::mlQuantityCapacityConstrs(model, I, q, pInst);
            break;
        }
        case ConfigParameters::model::policy_opt::OU :
        {
            init::mlQuantityCapacityConstrs(model, I, q, pInst);
            init::ouQuantityCapacityConstrs(model, I, v, q, pInst);
            break;
        }
        default:
        {
            RAW_LOG_F(FATAL, "MIRP-ARF::initModel: invalid policy option");
            break;
        }
        }
        /* valid inequalities */
    }
    catch (GRBException e)
    {
        RAW_LOG_F(FATAL, "MIRP-ARF::initModel: error code: %d",
            e.getErrorCode());
        RAW_LOG_F(FATAL, "MIRP-ARF::initModel: C-Exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "MIRP-ARF::initModel(): unknown Exception");
    }
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

Mirp_arf::Mirp_arf(const std::shared_ptr<const Instance>& pInst,
               const ConfigParameters::model& params) :
    mpInst(pInst),
    mModel(mEnv),
    mCbSEC(m_q, m_x, m_v, pInst)
{
    initModel(mModel, mI, m_q, m_x, m_v, mCbSEC, mpInst, params);
}


bool Mirp_arf::solve(const ConfigParameters::solver& params)
{
    RAW_LOG_F(INFO, "Solving MIRP-ARF...\n%s", std::string(80, '-').c_str());
    bool solved = true;

    try
    {
        // set solver parameters
        mModel.set(GRB_IntParam_OutputFlag, params.showLog_);
        mModel.set(GRB_DoubleParam_TimeLimit, params.timeLimit_);
        mModel.set(GRB_IntParam_Threads, params.nbThreads_);
        mModel.set(GRB_StringParam_LogFile, params.logFile_);

        mModel.optimize();

        if (mModel.get(GRB_IntAttr_Status) == GRB_INFEASIBLE)
        {
            solved = false;
        }
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(FATAL, "MIRP-ARF::solve: error code: %d", e.getErrorCode());
        RAW_LOG_F(FATAL, "MIRP-ARF::solve: C-Exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(FATAL, "MIRP-ARF::solve: unknown Exception");
    }

    return solved;
}


void Mirp_arf::writeIis(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + "_mirp_arf.ilp";

    try
    {
        mModel.computeIIS();
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "MIRP-ARF::writeIis: exp: %s", e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "MIRP-ARF::writeIis: Unknown Exception");
    }
}


void Mirp_arf::writeModel(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + "_mirp_arf.lp";

    try
    {
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "MIRP-ARF::writeModel: exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "MIRP-ARF::writeModel: Unknown Exception");
    }
}


void Mirp_arf::writeResultsJSON(std::string path)
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
        RAW_LOG_F(ERROR, "MIRP-ARF::writeSolution: exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "MIRP-ARF::writeSolution: Unknown Exception");
    }
}


void Mirp_arf::writeSolution(std::string path)
{
    DCHECK_F(std::filesystem::is_directory(path), "dir does not exists");
    path += mpInst->getName() + ".sol";

    try
    {
        mModel.write(path);
    }
    catch (GRBException& e)
    {
        RAW_LOG_F(ERROR, "MIRP-ARF::writeSolution: exp: %s",
            e.getMessage().c_str());
    }
    catch (...)
    {
        RAW_LOG_F(ERROR, "MIRP-ARF::writeSolution: Unknown Exception");
    }
}