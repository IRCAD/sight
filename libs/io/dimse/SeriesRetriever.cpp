/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "SeriesRetriever.hpp"

#include "io/dimse/exceptions/RequestFailure.hpp"

#include <core/os/temp_path.hpp>
#include <core/runtime/path.hpp>
#include <core/thread/worker.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>

#include <filesystem>

namespace sight::io::dimse
{

const core::com::slots::key_t series_retriever::PROGRESS_CALLBACK_SLOT = "CMoveProgressCallback";

// ----------------------------------------------------------------------------

series_retriever::series_retriever() :
    m_path("")
{
}

// ----------------------------------------------------------------------------

series_retriever::~series_retriever()
= default;

// ----------------------------------------------------------------------------

void series_retriever::initialize(
    const std::string& _application_title,
    std::uint16_t _applicationport,
    int _timeout,
    progress_callback_slot_t::sptr _progress_callback
)
{
    //Callback
    m_progress_callback = _progress_callback;

    //Creating folder
    m_path = core::os::temp_dir::shared_directory() / "dicom/";
    if(!std::filesystem::exists(m_path))
    {
        std::filesystem::create_directories(m_path);
    }

    //Configure network connection
    this->setAETitle(_application_title.c_str());
    this->setPort(_applicationport);

    // Load configuration
    std::filesystem::path cfg_path = core::runtime::get_library_resource_file_path("io_dimse/storescp.cfg");
    SIGHT_ASSERT("storescp.cfg not found !", std::filesystem::exists(cfg_path));
    this->loadAssociationCfgFile(cfg_path.string().c_str());
    this->setAndCheckAssociationProfile("Default");

    // Set non blocking states & timeout so we don't end up in an infinite loop
    this->setConnectionTimeout(Uint32(_timeout));
    this->setConnectionBlockingMode(DUL_NOBLOCK);
}

// ----------------------------------------------------------------------------

bool series_retriever::start()
{
    // Reset instance count
    m_instance_index = 0;

    // Start listening
    return this->listen().good();
}

// ----------------------------------------------------------------------------

OFCondition series_retriever::handleIncomingCommand(
    T_DIMSE_Message* _incoming_msg,
    const DcmPresentationContextInfo& _pres_context_info
)
{
    OFCondition cond;

    // Process C-STORE request
    if(_incoming_msg->CommandField == DIMSE_C_STORE_RQ)
    {
        cond = handleSTORERequest(_incoming_msg, _pres_context_info.presentationContextID);
    }
    // Process other requests
    else
    {
        cond = DcmSCP::handleIncomingCommand(_incoming_msg, _pres_context_info);
    }

    return cond;
}

// ----------------------------------------------------------------------------

OFCondition series_retriever::handleSTORERequest(
    T_DIMSE_Message* _incoming_msg,
    T_ASC_PresentationContextID _pres_id
)
{
    OFCondition cond;

    // Dump incoming message
    OFString temp_str;

    // Get Dataset
    auto* dataset = new DcmDataset();
    if(this->receiveDIMSEDataset(&_pres_id, &dataset).good())
    {
        if(dataset != nullptr)
        {
            //Find the series UID
            OFString series_id;
            if(dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, series_id).good())
            {
            }

            //Find the instance UID
            OFString instance_id;
            if(dataset->findAndGetOFStringArray(DCM_SOPInstanceUID, instance_id).good())
            {
            }

            //Create Folder
            std::filesystem::path series_path = std::filesystem::path(m_path.string() + series_id.c_str() + "/");
            if(!std::filesystem::exists(series_path))
            {
                std::filesystem::create_directories(series_path);
            }

            //Save the file in the specified folder
            std::string file_path = series_path.string() + instance_id.c_str();
            dataset->saveFile(file_path.c_str());

            // Send a store response
            T_DIMSE_C_StoreRSP rsp {};
            rsp.DimseStatus = STATUS_Success;
            cond            = this->sendSTOREResponse(_pres_id, _incoming_msg->msg.CStoreRQ, rsp.DimseStatus);

            // Dump outgoing message

            if(cond.bad())
            {
                const std::string msg = "Cannot send C-STORE Response to the server.";
                throw io::dimse::exceptions::request_failure(msg);
            }

            // Notify callback
            if(m_progress_callback)
            {
                m_progress_callback->async_run(series_id.c_str(), ++m_instance_index, file_path);
            }
        }
    }

    return cond;
}

} //namespace sight::io::dimse
