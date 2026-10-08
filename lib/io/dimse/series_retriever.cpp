/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "series_retriever.hpp"

#include "io/dimse/exceptions/request_failure.hpp"
#include "io/dimse/helper/series.hpp"

#include <core/runtime/path.hpp>

#include <filesystem>

namespace sight::io::dimse
{

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
    core::notification::observer::sptr _progress
)
{
    //Callback
    m_progress = _progress;

    //Configure network connection
    this->setAETitle(_application_title.c_str());
    this->setPort(_applicationport);

    // Load configuration
    std::filesystem::path cfg_path = core::runtime::get_library_resource_file_path("io_dimse/storescp.cfg");
    SIGHT_ASSERT("storescp.cfg not found !", std::filesystem::exists(cfg_path));
    this->loadAssociationCfgFile(cfg_path.string().c_str());
    this->setAndCheckAssociationProfile("Default");

    // Set non blocking states & timeout so we don't end up in an infinite loop
    this->setConnectionTimeout(static_cast<Uint32>(_timeout));
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

    // Get Dataset
    auto* dataset = new DcmDataset();
    if(this->receiveDIMSEDataset(&_pres_id, &dataset).good())
    {
        if(dataset != nullptr)
        {
            std::string file_path = io::dimse::helper::series::get_path(*dataset).string();
            dataset->saveFile(file_path.c_str());

            Uint16 dimse_status = STATUS_Success;
            cond = this->sendSTOREResponse(_pres_id, _incoming_msg->msg.CStoreRQ, dimse_status);

            // Dump outgoing message
            if(cond.bad())
            {
                const std::string msg = "Cannot send C-STORE Response to the server.";
                throw io::dimse::exceptions::request_failure(msg);
            }

            // Notify callback
            if(m_progress)
            {
                //Find the series UID
                OFString series_id;
                if(dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, series_id).good())
                {
                    m_progress->done_work(++m_instance_index);
                }
            }
        }
    }

    return cond;
}

} //namespace sight::io::dimse
