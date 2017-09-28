/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwPacsIO/SeriesRetriever.hpp"

#include "fwPacsIO/exceptions/RequestFailure.hpp"

#include <fwDcmtkTools/Dictionary.hpp>

#include <fwRuntime/operations.hpp>

#include <fwThread/Worker.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem/operations.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>

namespace fwPacsIO
{

const ::fwCom::Slots::SlotKeyType SeriesRetriever::s_PROGRESS_CALLBACK_SLOT = "CMoveProgressCallback";

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

void SeriesRetriever::initialize(const std::string& applicationTitle,
                                 unsigned short applicationport, int timeout,
                                 ProgressCallbackSlotType::sptr progressCallback)
{
    //Callback
    m_progressCallback = progressCallback;

    //Creating folder
    m_path = ::fwTools::System::getTemporaryFolder() / "dicom/";
    if (!::boost::filesystem::exists(m_path))
    {
        ::boost::filesystem::create_directories(m_path);
    }

    // Load dictionary
    ::fwDcmtkTools::Dictionary::loadDictionary();

    //Configure network connection
    this->setAETitle(applicationTitle.c_str());
    this->setPort(applicationport);

    // Load configuration
    ::boost::filesystem::path cfgPath =
        ::fwRuntime::getLibraryResourceFilePath("fwPacsIO-" FWPACSIO_VER "/storescp.cfg");
    SLM_ASSERT("storescp.cfg not found !", ::boost::filesystem::exists(cfgPath));
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

OFCondition SeriesRetriever::handleIncomingCommand(T_DIMSE_Message* incomingMsg,
                                                   const DcmPresentationContextInfo& presContextInfo)
{
    OFCondition cond;

    // Process C-STORE request
    if( incomingMsg->CommandField == DIMSE_C_STORE_RQ )
    {
        cond = handleSTORERequest( incomingMsg, presContextInfo.presentationContextID );
    }
    // Process other requests
    else
    {
        cond = DcmSCP::handleIncomingCommand(incomingMsg, presContextInfo);
    }

    return cond;
}

// ----------------------------------------------------------------------------

OFCondition SeriesRetriever::handleSTORERequest(T_DIMSE_Message* incomingMsg,
                                                T_ASC_PresentationContextID presID)
{
    OFCondition cond;

    // Dump incoming message
    SLM_TRACE("Received C-STORE Request");
    OFString tempStr;
    OSLM_TRACE(DIMSE_dumpMessage(tempStr, incomingMsg->msg.CStoreRQ, DIMSE_INCOMING, NULL, presID));

    // Get Dataset
    DcmDataset* dataset = new DcmDataset();
    if (this->receiveDIMSEDataset(&presID, &dataset).good())
    {
        if (dataset != NULL)
        {

            //Find the series UID
            OFString seriesID;
            if(dataset->findAndGetOFStringArray(DCM_SeriesInstanceUID, seriesID).good())
            {
                SLM_TRACE("Series Instance UID: " + std::string(seriesID.c_str()));
            }

            //Find the instance UID
            OFString iname;
            if (dataset->findAndGetOFStringArray(DCM_SOPInstanceUID, iname).good())
            {
                SLM_TRACE("SOP Instance UID: " + std::string(iname.c_str()));
            }

            //Create Folder
            ::boost::filesystem::path seriesPath = ::boost::filesystem::path(m_path.string() + seriesID.c_str() + "/");
            if (!::boost::filesystem::exists(seriesPath))
            {
                ::boost::filesystem::create_directories(seriesPath);
            }

            //Save the file in the specified folder
            std::string filePath = seriesPath.string() + iname.c_str();
            dataset->saveFile(filePath.c_str());

            // Send a store response
            T_DIMSE_C_StoreRSP rsp;
            rsp.DimseStatus = STATUS_Success;
            cond            = this->sendSTOREResponse(presID, incomingMsg->msg.CStoreRQ, rsp.DimseStatus);

            // Dump outgoing message
            SLM_TRACE("Sending C-STORE Response");
            OSLM_TRACE(DIMSE_dumpMessage(tempStr, rsp, DIMSE_OUTGOING, NULL, presID));

            if (cond.bad())
            {
                const std::string msg = "Cannot send C-STORE Response to the server.";
                throw ::fwPacsIO::exceptions::RequestFailure(msg);
            }
            else
            {
                SLM_TRACE("C-STORE Response successfully sent.");
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

} //namespace fwPacsIO

