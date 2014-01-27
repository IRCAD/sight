/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <boost/filesystem/operations.hpp>

#include <fwThread/Worker.hpp>
#include <fwTools/System.hpp>

#include "fwDicomIOExt/exceptions/RequestFailure.hpp"
#include "fwDicomIOExt/dcmtk/helper/Dictionary.hpp"

#include "fwDicomIOExt/dcmtk/SeriesRetriever.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{

const ::fwCom::Slots::SlotKeyType SeriesRetriever::s_STORE_INSTANCE_CALLBACK_SLOT = "storeInstanceCallbackFromMoveRetrieveMethod";

// ----------------------------------------------------------------------------

SeriesRetriever::SeriesRetriever() :
        m_path(""),
        m_storeInstanceCallback(StoreInstanceCallbackSlotType::sptr())
{
}

// ----------------------------------------------------------------------------

SeriesRetriever::~SeriesRetriever()
{
}

// ----------------------------------------------------------------------------

void SeriesRetriever::initialize(const std::string& applicationTitle,
        unsigned int applicationport, int timeout,
        StoreInstanceCallbackSlotType::sptr storeCallback)
{
    //Callback
    m_storeInstanceCallback = storeCallback;

    //Creating folder
    m_path = ::fwTools::System::getTemporaryFolder() / "dicom/";
    if (!::boost::filesystem::exists(m_path))
    {
        ::boost::filesystem::create_directories(m_path);
    }

    //Load dictionary
    ::fwDicomIOExt::dcmtk::helper::Dictionary::loadDictionary();

    //Configure network connection
    this->setAETitle(applicationTitle.c_str());
    this->setPort(applicationport);

    // Set transfer syntaxes
    OFList < OFString > transferSyntaxes;
    transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
    transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
    transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);

    // Add storage presentation context
    this->addPresentationContext(UID_CTImageStorage, transferSyntaxes);
    this->addPresentationContext(UID_EnhancedCTImageStorage, transferSyntaxes);
    this->addPresentationContext(UID_MRImageStorage, transferSyntaxes);
    this->addPresentationContext(UID_UltrasoundImageStorage, transferSyntaxes);
    this->addPresentationContext(UID_SecondaryCaptureImageStorage, transferSyntaxes);
    this->addPresentationContext(UID_ComputedRadiographyImageStorage, transferSyntaxes);

    // Set non blocking states & timeout so we don't end up in an infinite loop
    this->setConnnectionTimeout(timeout);
    this->setConnectionBlockingMode(DUL_NOBLOCK);
}

// ----------------------------------------------------------------------------

bool SeriesRetriever::start()
{
    // Start listening
    return this->listen().good();
}

// ----------------------------------------------------------------------------

OFCondition SeriesRetriever::handleIncomingCommand(T_DIMSE_Message *incomingMsg,
            const DcmPresentationContextInfo &presContextInfo)
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

OFCondition SeriesRetriever::handleSTORERequest(T_DIMSE_Message *incomingMsg,
            T_ASC_PresentationContextID presID)
{
    OFCondition cond;

    // Dump incoming message
    SLM_TRACE("Received C-STORE Request");
    OFString tempStr;
    OSLM_TRACE(DIMSE_dumpMessage(tempStr, incomingMsg->msg.CStoreRQ, DIMSE_INCOMING, NULL, presID));

    // Get Dataset
    DcmDataset* dataset = new DcmDataset();
    if (this->receiveDIMSEDataset(&presID, &dataset, NULL, NULL).good())
    {
        if (dataset != NULL)
        {
            //Find the name of the file
            OFString iname;
            if (dataset->findAndGetOFString(DCM_SOPInstanceUID, iname, 0, true).good())
            {
                OSLM_TRACE("SOP IUID: " << iname.c_str());
            }

            //Find the series instance UID (folder name)
            OFString seriesID;
            if(dataset->findAndGetOFString(DCM_SeriesInstanceUID, seriesID).good())
            {
                OSLM_TRACE("Series Instance UID: " << iname.c_str());
            }

            //Find the instance number
            long int instanceNumber;
            if(dataset->findAndGetLongInt(DCM_InstanceNumber, instanceNumber).good())
            {
                OSLM_TRACE("Instance Number: " << instanceNumber);
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
            cond = this->sendSTOREResponse(presID, incomingMsg->msg.CStoreRQ, rsp, NULL);

            // Dump outgoing message
            SLM_TRACE("Sending C-STORE Response");
            OSLM_TRACE(DIMSE_dumpMessage(tempStr, rsp, DIMSE_OUTGOING, NULL, presID));

            if (cond.bad())
            {
                std::string msg = "Cannot send C-STORE Response to the server. ";
                throw ::fwDicomIOExt::exceptions::RequestFailure(msg);
            }
            else
            {
                SLM_TRACE("C-STORE Response successfully sent");
            }

            // Notify callback
            if(m_storeInstanceCallback)
            {
                SLM_TRACE("Notify Store Callback:" + instanceNumber);
                m_storeInstanceCallback->asyncRun(seriesID.c_str(), instanceNumber, filePath);
            }

        }
    }

    return cond;

}

} //dcmtk
} //namespace fwDicomIOExt

