/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <boost/foreach.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>

#include <fwCore/spyLog.hpp>
#include <fwTools/System.hpp>

#include "fwDicomIOExt/exceptions/NegociateAssociationFailure.hpp"
#include "fwDicomIOExt/exceptions/NetworkInitializationFailure.hpp"
#include "fwDicomIOExt/exceptions/PresentationContextMissing.hpp"
#include "fwDicomIOExt/exceptions/RequestFailure.hpp"
#include "fwDicomIOExt/exceptions/TagMissing.hpp"
#include "fwDicomIOExt/dcmtk/helper/Dictionary.hpp"

#include "fwDicomIOExt/dcmtk/SeriesEnquirer.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{

const ::fwCom::Slots::SlotKeyType SeriesEnquirer::s_PROGRESS_CALLBACK_SLOT = "progressCallback";
const ::fwCom::Slots::SlotKeyType SeriesEnquirer::s_STORE_INSTANCE_CALLBACK_SLOT = "storeInstanceCallbackFromGetRetrieveMethod";

SeriesEnquirer::SeriesEnquirer() :
        m_moveApplicationTitle(""),
        m_progressCallback(ProgressCallbackSlotType::sptr()),
        m_path(""),
        m_storeInstanceCallback(StoreInstanceCallbackSlotType::sptr())
{

}

// ----------------------------------------------------------------------------

SeriesEnquirer::~SeriesEnquirer()
{
}

// ----------------------------------------------------------------------------

Uint8 SeriesEnquirer::findUncompressedPC(const OFString& sopClass)
{
    Uint8 pc;
    pc = this->findPresentationContextID(sopClass, UID_LittleEndianExplicitTransferSyntax);
    if (pc == 0)
    {
        pc = this->findPresentationContextID(sopClass, UID_BigEndianExplicitTransferSyntax);
    }
    if (pc == 0)
    {
        pc = this->findPresentationContextID(sopClass, UID_LittleEndianImplicitTransferSyntax);
    }
    return pc;
}

// ----------------------------------------------------------------------------

void SeriesEnquirer::initialize(const std::string& applicationTitle, const std::string& peerHostName,
        unsigned int peerPort, const std::string& peerApplicationTitle,
        const std::string& moveApplicationTitle, StoreInstanceCallbackSlotType::sptr storeCallback,
        ProgressCallbackSlotType::sptr progressCallback)
{
    //Save move application title for move requests
    m_moveApplicationTitle = moveApplicationTitle;

    //Move callback
    m_progressCallback = progressCallback;

    //Store Callback
    m_storeInstanceCallback = storeCallback;

    //Creating folder
    m_path = ::fwTools::System::getTemporaryFolder() / "dicom/";
    if (!m_path.empty() && !::boost::filesystem::exists(m_path))
    {
        ::boost::filesystem::create_directories(m_path);
    }

    //Load dictionary
    ::fwDicomIOExt::dcmtk::helper::Dictionary::loadDictionary();

    //Configure network connection
    this->setAETitle(applicationTitle.c_str());
    this->setPeerHostName(peerHostName.c_str());
    this->setPeerPort(peerPort);
    this->setPeerAETitle(peerApplicationTitle.c_str());

    // Use presentation context for FIND/MOVE in study root, propose all uncompressed transfer syntaxes
    OFList < OFString > transferSyntaxes;
    transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
    transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
    transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);

    // Clear presentation context
    this->clearPresentationContexts();

    // Add Verification SOP Class presentation context
    this->addPresentationContext(UID_VerificationSOPClass, transferSyntaxes);

    // Add study presentation context
    this->addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, transferSyntaxes);
    this->addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, transferSyntaxes);
    this->addPresentationContext(UID_GETStudyRootQueryRetrieveInformationModel, transferSyntaxes);

    // Add presentation context for C-GET store requests
    for (Uint16 j = 0; j < numberOfDcmLongSCUStorageSOPClassUIDs; j++)
    {
        this->addPresentationContext(dcmLongSCUStorageSOPClassUIDs[j], transferSyntaxes, ASC_SC_ROLE_SCP);
    }

}

// ----------------------------------------------------------------------------

bool SeriesEnquirer::connect()
{
    // Initialize network
    OFCondition result = this->initNetwork();
    if (result.bad())
    {
        const std::string msg = "Unable to set up the network: " + std::string(result.text());
        throw ::fwDicomIOExt::exceptions::NetworkInitializationFailure(msg);
    }

    // Negotiate Association
    result = this->negotiateAssociation();
    if (result.bad())
    {
        const std::string msg = "Unable to negotiate association: " + std::string(result.text());
        throw ::fwDicomIOExt::exceptions::NegociateAssociationFailure(msg);
    }

    return true;

}

// ----------------------------------------------------------------------------

void SeriesEnquirer::disconnect()
{
    this->closeAssociation(DCMSCU_RELEASE_ASSOCIATION);
}

// ----------------------------------------------------------------------------

bool SeriesEnquirer::isConnectedToPacs() const
{
    return this->isConnected();
}

// ----------------------------------------------------------------------------

bool SeriesEnquirer::pingPacs()
{
    return this->sendECHORequest(0).good();
}

// ----------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::sendFindRequest(DcmDataset dataset)
{
    OFList< QRResponse* > findResponses;

    // Try to find a presentation context
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_FINDStudyRootQueryRetrieveInformationModel);
    if (presID == 0)
    {
        const std::string msg = "There is no uncompressed presentation context for Study Root FIND";
        throw ::fwDicomIOExt::exceptions::PresentationContextMissing(msg);
    }

    // Send the request
    OFCondition result = this->sendFINDRequest(presID, &dataset, &findResponses);

    return findResponses;
}

// ----------------------------------------------------------------------------

OFCondition SeriesEnquirer::sendMoveRequest(DcmDataset dataset)
{
    // Be sure that the needed informations are set
    SLM_ASSERT("The path where to store the series is not set.", !m_path.empty());
    SLM_ASSERT("The move application title is not set.", !m_moveApplicationTitle.empty());

    // Try to find a presentation context
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel);
    SLM_WARN_IF("There is no uncompressed presentation context for Study Root MOVE", presID == 0);

    // Fetches all images of this particular study
    OFCondition result;
    OFList< RetrieveResponse * > dataResponse;
    return  this->sendMOVERequest(presID, m_moveApplicationTitle.c_str(), &dataset, &dataResponse);
}

// ----------------------------------------------------------------------------

OFCondition SeriesEnquirer::sendGetRequest(DcmDataset dataset)
{
    // Be sure that the needed informations are set
    SLM_ASSERT("The path where to store the series is not set.", !m_path.empty());

    // Try to find a presentation context
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_GETStudyRootQueryRetrieveInformationModel);

    if (presID == 0)
    {
        SLM_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    // Fetches all images of this particular study
    OFCondition result;
    OFList< RetrieveResponse * > dataResponse;
    return this->sendCGETRequest(presID, &dataset, &dataResponse);
}

// ----------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByPatientName(const std::string& name)
{
    // Dataset used to store query informations
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, "");

    // Search by patient name
    std::string searchString = "*" + name + "*";
    dataset.putAndInsertOFStringArray(DCM_PatientName, searchString.c_str());

    // Fields needed by DICOMSeries
    //dataset.putAndInsertOFStringArray(DCM_PatientName, "");
    dataset.putAndInsertOFStringArray(DCM_PatientID, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");
    dataset.putAndInsertOFStringArray(DCM_Modality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");

    // Number of instances
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

// ----------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByDate(const std::string& fromDate, const std::string& toDate)
{
    // Dataset used to store query informations
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, "");

    // Search by date
    std::string searchString = fromDate + "-" + toDate;
    dataset.putAndInsertOFStringArray(DCM_StudyDate, searchString.c_str());

    // Fields needed by DICOMSeries
    dataset.putAndInsertOFStringArray(DCM_PatientName, "");
    dataset.putAndInsertOFStringArray(DCM_PatientID, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    //dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");
    dataset.putAndInsertOFStringArray(DCM_Modality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");

    // Number of instances
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

// ----------------------------------------------------------------------------

std::string SeriesEnquirer::findSOPInstanceUID(const std::string& seriesInstanceUID, unsigned int instanceNumber)
{
    // Dataset used to store query informations
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID.c_str());
    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, "");
    std::stringstream ss;
    ss << instanceNumber;
    dataset.putAndInsertOFStringArray(DCM_InstanceNumber, ss.str().c_str());

    OFList< QRResponse* > responses = this->sendFindRequest(dataset);
    OFIterator< QRResponse* > it = responses.begin();
    std::string sopInstanceUID = "";
    if(it != responses.end() && (*it)->m_dataset)
    {
        OFString sop;
        (*it)->m_dataset->findAndGetOFStringArray(DCM_SOPInstanceUID, sop);
        sopInstanceUID = sop.c_str();
    }

    //Release responses
    while (!responses.empty())
    {
        delete responses.front();
        responses.pop_front();
    }

    return sopInstanceUID;

}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pullSeriesUsingMoveRetrieveMethod(InstanceUIDContainer instanceUIDContainer)
{

    DcmDataset dataset;
    OFCondition result;

    BOOST_FOREACH( std::string seriesInstanceUID, instanceUIDContainer )
    {
        dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");
        dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID.c_str());

        // Fetches all images of this particular study
        result = this->sendMoveRequest(dataset);

        if (result.good())
        {
            SLM_TRACE("Received series " + seriesInstanceUID);
        }
        else
        {
            const std::string msg = "Unable to send a C-MOVE request to the server. "
                    "(Series instance UID =" + std::string(seriesInstanceUID.c_str()) +") : "
                    + std::string(result.text());
            throw ::fwDicomIOExt::exceptions::RequestFailure(msg);
        }
    }

}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pullSeriesUsingGetRetrieveMethod(InstanceUIDContainer instanceUIDContainer)
{
    DcmDataset dataset;
    OFCondition result;

    BOOST_FOREACH( std::string seriesInstanceUID, instanceUIDContainer )
    {
        dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");
        dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID.c_str());

        // Fetches all images of this particular study
        result = this->sendGetRequest(dataset);

        if (result.good())
        {
            SLM_TRACE("Received series " + seriesInstanceUID);
        }
        else
        {
            const std::string msg = "Unable to send a C-GET request to the server. "
                    "(Series instance UID =" + std::string(seriesInstanceUID.c_str()) +") : "
                    + std::string(result.text());
            throw ::fwDicomIOExt::exceptions::RequestFailure(msg);
        }
    }
}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pullInstanceUsingMoveRetrieveMethod(const std::string& seriesInstanceUID,
        const std::string& sopInstanceUID)
{
    DcmDataset dataset;
    OFCondition result;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID.c_str());

    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, sopInstanceUID.c_str());
    dataset.putAndInsertOFStringArray(DCM_InstanceNumber, "");

    // Fetches all images of this particular study
    result = this->sendMoveRequest(dataset);

    if (result.good())
    {
        SLM_TRACE("Received instance " + seriesInstanceUID + " - " + sopInstanceUID);
    }
    else
    {
        const std::string msg = "Unable to send a C-MOVE request to the server. "
                "(Series instance UID =" + std::string(seriesInstanceUID.c_str()) +") : "
                + std::string(result.text());
        throw ::fwDicomIOExt::exceptions::RequestFailure(msg);
    }
}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pullInstanceUsingGetRetrieveMethod(const std::string& seriesInstanceUID,
        const std::string& sopInstanceUID)
{
    DcmDataset dataset;
    OFCondition result;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID.c_str());

    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, sopInstanceUID.c_str());
    dataset.putAndInsertOFStringArray(DCM_InstanceNumber, "");

    // Fetches all images of this particular study
    result = this->sendGetRequest(dataset);

    if (result.good())
    {
        SLM_TRACE("Received instance " + seriesInstanceUID + " - " + sopInstanceUID);
    }
    else
    {
        const std::string msg = "Unable to send a C-GET request to the server. "
                "(Series instance UID =" + std::string(seriesInstanceUID.c_str()) +") : "
                + std::string(result.text());
        throw ::fwDicomIOExt::exceptions::RequestFailure(msg);
    }
}

// ----------------------------------------------------------------------------

OFCondition SeriesEnquirer::handleMOVEResponse(
    const T_ASC_PresentationContextID presID, RetrieveResponse *response, OFBool &waitForNextResponse)
{
    OFCondition result = DcmSCU::handleMOVEResponse(presID, response, waitForNextResponse);

    // Compute percentage
    unsigned int total = response->m_numberOfRemainingSubops + response->m_numberOfFailedSubops
            + response->m_numberOfWarningSubops + response->m_numberOfCompletedSubops;
    float percent = (float(response->m_numberOfCompletedSubops) / total) *100;

    // Check error status
    bool error = (response->m_status != STATUS_Success) && (response->m_status != STATUS_Pending);

    // Notify callback
    if(m_progressCallback)
    {
        OSLM_TRACE("Notify Progress Callback:" << percent << "%");
        m_progressCallback->asyncRun(percent, error);
    }

    return result;
}

// ----------------------------------------------------------------------------

OFCondition SeriesEnquirer::handleSTORERequest (
        const T_ASC_PresentationContextID presID, DcmDataset *incomingObject,
        OFBool &continueCGETSession, Uint16 &cStoreReturnStatus)
{
    OFCondition result;

    if (incomingObject != NULL)
    {
        //Find the name of the file
        OFString iname;
        if (incomingObject->findAndGetOFString(DCM_SOPInstanceUID, iname, 0, true).good())
        {
            SLM_TRACE("SOP IUID: " + std::string(iname.c_str()));
        }

        OFString seriesID;
        if(incomingObject->findAndGetOFString(DCM_SeriesInstanceUID, seriesID).good())
        {
            SLM_TRACE("Series Instance UID: " + std::string(seriesID.c_str()));
        }

        //Find the instance number
        long int instanceNumber;
        if(incomingObject->findAndGetLongInt(DCM_InstanceNumber, instanceNumber).good())
        {
            SLM_TRACE("Instance Number: " + instanceNumber);
        }

        //Check if it is a new series
        if(m_progressCallback && instanceNumber == 1)
        {
            // Notify callback
            SLM_TRACE("Notify Progress Callback:0%");
            m_progressCallback->asyncRun(0, !continueCGETSession);
        }

        //Create Folder
        ::boost::filesystem::path seriesPath = ::boost::filesystem::path(m_path.string() + seriesID.c_str() + "/");
        if (!::boost::filesystem::exists(seriesPath))
        {
            ::boost::filesystem::create_directories(seriesPath);
        }

        //Save the file in the specified folder
        std::string filePath = seriesPath.string() + iname.c_str();
        incomingObject->saveFile(filePath.c_str());

        // Notify callback
        if(m_storeInstanceCallback)
        {
            SLM_TRACE("Notify Store Callback:" + instanceNumber);
            m_storeInstanceCallback->asyncRun(seriesID.c_str(), instanceNumber, filePath);
        }
    }

    return result;
}

// ----------------------------------------------------------------------------

} //dcmtk
} //namespace fwDicomIOExt

