/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/runtime/operations.hpp>
#include <core/thread/Worker.hpp>
#include <core/tools/System.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>

#include <filesystem>

namespace sight::io::dimse
{

const core::com::Slots::SlotKeyType SeriesRetriever::s_PROGRESS_CALLBACK_SLOT = "CMoveProgressCallback";

// ----------------------------------------------------------------------------

SeriesRetriever::SeriesRetriever() :
    m_path(""),
    m_progressCallback(ProgressCallbackSlotType::sptr())
{
}

// ----------------------------------------------------------------------------

SeriesRetriever::~SeriesRetriever()
{
}

// ----------------------------------------------------------------------------

void SeriesRetriever::initialize(
    const std::string& applicationTitle,
    unsigned short applicationport,
    int timeout,
    ProgressCallbackSlotType::sptr progressCallback
)
{
    //Callback
    m_progressCallback = progressCallback;

    //Creating folder
    m_path = core::tools::System::getTemporaryFolder() / "dicom/";
    if(!std::filesystem::exists(m_path))
    {
        std::filesystem::create_directories(m_path);
    }

    //Configure network connection
    this->setAETitle(applicationTitle.c_str());
    this->setPort(applicationport);

    // Load configuration
    std::filesystem::path cfgPath = core::runtime::getLibraryResourceFilePath("io_dimse/storescp.cfg");
    SIGHT_ASSERT("storescp.cfg not found !", std::filesystem::exists(cfgPath));
    this->loadAssociationCfgFile(cfgPath.string().c_str());
    this->setAndCheckAssociationProfile("Default");

    // Set non blocking states & timeout so we don't end up in an infinite loop
    this->setConnectionTimeout(timeout);
    this->setConnectionBlockingMode(DUL_NOBLOCK);
}

// ----------------------------------------------------------------------------

bool SeriesRetriever::start()
{
    // Reset instance count
    m_instanceIndex = 0;

    // Start listening
    return this->listen().good();
}

// ----------------------------------------------------------------------------

OFCondition SeriesRetriever::handleIncomingCommand(
    T_DIMSE_Message* incomingMsg,
    const DcmPresentationContextInfo& presContextInfo
)
{
    OFCondition cond;

    // Process C-STORE request
    if(incomingMsg->CommandField == DIMSE_C_STORE_RQ)
    {
        cond = handleSTORERequest(incomingMsg, presContextInfo.presentationContextID);
    }
    // Process other requests
    else
    {
        cond = DcmSCP::handleIncomingCommand(incomingMsg, presContextInfo);
    }

    return cond;
}

// ----------------------------------------------------------------------------

OFCondition SeriesRetriever::handleSTORERequest(
    T_DIMSE_Message* incomingMsg,
    T_ASC_PresentationContextID presID
)
{
    OFCondition cond;

    // Dump incoming message
    OFString tempStr;

    // Get Dataset
    DcmDataset* dataset = new DcmDataset();
    if(this->receiveDIMSEDataset(&presID, &dataset).good())
    {
        if(dataset != NULL)
        {
            //Find the series UID
            OFString seriesID;
            if(dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, seriesID).good())
            {
            }

            //Find the instance UID
            OFString iname;
            if(dataset->findAndGetOFStringArray(DCM_SOPInstanceUID, iname).good())
            {
            }

            //Create Folder
            std::filesystem::path seriesPath = std::filesystem::path(m_path.string() + seriesID.c_str() + "/");
            if(!std::filesystem::exists(seriesPath))
            {
                std::filesystem::create_directories(seriesPath);
            }

            //Save the file in the specified folder
            std::string filePath = seriesPath.string() + iname.c_str();
            dataset->saveFile(filePath.c_str());

            // Send a store response
            T_DIMSE_C_StoreRSP rsp;
            rsp.DimseStatus = STATUS_Success;
            cond            = this->sendSTOREResponse(presID, incomingMsg->msg.CStoreRQ, rsp.DimseStatus);

            // Dump outgoing message

            if(cond.bad())
            {
                const std::string msg = "Cannot send C-STORE Response to the server.";
                throw io::dimse::exceptions::RequestFailure(msg);
            }
            else
            {
            }

            // Notify callback
            if(m_progressCallback)
            {
                m_progressCallback->asyncRun(seriesID.c_str(), ++m_instanceIndex, filePath);
            }
        }
    }

    return cond;
}

} //namespace sight::io::dimse
