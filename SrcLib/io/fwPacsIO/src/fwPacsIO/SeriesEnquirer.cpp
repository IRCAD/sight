/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwPacsIO/SeriesEnquirer.hpp"

#include "fwPacsIO/exceptions/NegociateAssociationFailure.hpp"
#include "fwPacsIO/exceptions/NetworkInitializationFailure.hpp"
#include "fwPacsIO/exceptions/PresentationContextMissing.hpp"
#include "fwPacsIO/exceptions/RequestFailure.hpp"
#include "fwPacsIO/exceptions/TagMissing.hpp"

#include <fwCore/spyLog.hpp>

#include <fwDcmtkTools/Dictionary.hpp>

#include <fwTools/System.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/foreach.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmnet/diutil.h>

namespace fwPacsIO
{

const ::fwCom::Slots::SlotKeyType SeriesEnquirer::s_PROGRESS_CALLBACK_SLOT = "CGetProgressCallback";

SeriesEnquirer::SeriesEnquirer() :
    m_moveApplicationTitle(""),
    m_path(""),
    m_progressCallback(ProgressCallbackSlotType::sptr()),
    m_instanceIndex(0)
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
                                unsigned short peerPort, const std::string& peerApplicationTitle,
                                const std::string& moveApplicationTitle,
                                ProgressCallbackSlotType::sptr progressCallback)
{
    //Save move application title for move requests
    m_moveApplicationTitle = moveApplicationTitle;

    //Store Callback
    m_progressCallback = progressCallback;

    //Creating folder
    m_path = ::fwTools::System::getTemporaryFolder() / "dicom/";
    if (!m_path.empty() && !::boost::filesystem::exists(m_path))
    {
        ::boost::filesystem::create_directories(m_path);
    }

    //Load dictionary
    ::fwDcmtkTools::Dictionary::loadDictionary();

    //Configure network connection
    this->setAETitle(applicationTitle.c_str());
    this->setPeerHostName(peerHostName.c_str());
    this->setPeerPort(peerPort);
    this->setPeerAETitle(peerApplicationTitle.c_str());

    // Clear presentation context
    this->clearPresentationContexts();

    // Use presentation context for FIND/MOVE in study root, propose all uncompressed transfer syntaxes
    OFList < OFString > transferSyntaxes;
    transferSyntaxes.push_back(UID_LittleEndianImplicitTransferSyntax);
    transferSyntaxes.push_back(UID_LittleEndianExplicitTransferSyntax);
    transferSyntaxes.push_back(UID_BigEndianExplicitTransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess1TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess2_4TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess3_5TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess6_8TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess7_9TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess10_12TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess11_13TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess14TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess15TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess16_18TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess17_19TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess20_22TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess21_23TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess24_26TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess25_27TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess28TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess29TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGProcess14SV1TransferSyntax);
    transferSyntaxes.push_back(UID_JPEGLSLosslessTransferSyntax);
    transferSyntaxes.push_back(UID_JPEGLSLossyTransferSyntax);
    transferSyntaxes.push_back(UID_RLELosslessTransferSyntax);
    transferSyntaxes.push_back(UID_DeflatedExplicitVRLittleEndianTransferSyntax);
    transferSyntaxes.push_back(UID_JPEG2000LosslessOnlyTransferSyntax);
    transferSyntaxes.push_back(UID_JPEG2000TransferSyntax);
    transferSyntaxes.push_back(UID_MPEG2MainProfileAtMainLevelTransferSyntax);
    transferSyntaxes.push_back(UID_MPEG2MainProfileAtHighLevelTransferSyntax);
    transferSyntaxes.push_back(UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax);
    transferSyntaxes.push_back(UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax);
    transferSyntaxes.push_back(UID_JPIPReferencedTransferSyntax);
    transferSyntaxes.push_back(UID_JPIPReferencedDeflateTransferSyntax);
    transferSyntaxes.push_back(UID_RFC2557MIMEEncapsulationTransferSyntax);
    transferSyntaxes.push_back(UID_XMLEncodingTransferSyntax);

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
        throw ::fwPacsIO::exceptions::NetworkInitializationFailure(msg);
    }

    // Negotiate Association
    result = this->negotiateAssociation();
    if (result.bad())
    {
        const std::string msg = "Unable to negotiate association: " + std::string(result.text());
        throw ::fwPacsIO::exceptions::NegociateAssociationFailure(msg);
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
        throw ::fwPacsIO::exceptions::PresentationContextMissing(msg);
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
    OFList< RetrieveResponse* > dataResponse;
    return this->sendMOVERequest(presID, m_moveApplicationTitle.c_str(), &dataset, &dataResponse);
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
    OFList< RetrieveResponse* > dataResponse;
    return this->sendCGETRequest(presID, &dataset, &dataResponse);
}

// ----------------------------------------------------------------------------

OFCondition SeriesEnquirer::sendStoreRequest(const ::boost::filesystem::path& path)
{
    // Try to find a presentation context
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel);

    if (presID == 0)
    {
        SLM_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    Uint16 rspStatusCode;
    OFCondition result = this->sendSTORERequest(presID, OFString(path.string().c_str()), 0, rspStatusCode);
    OSLM_WARN("PACS RESPONSE :" << rspStatusCode);
    return result;
}

// ----------------------------------------------------------------------------

OFCondition SeriesEnquirer::sendStoreRequest(const CSPTR(DcmDataset)& dataset)
{
    // Try to find a presentation context
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel);

    if (presID == 0)
    {
        SLM_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    Uint16 rspStatusCode;
    // const_cast required to use bad DCMTK sendSTORERequest API
    DcmDataset* datasetPtr = const_cast<DcmDataset*>(dataset.get());
    OFCondition result     = this->sendSTORERequest(presID, OFString(""), datasetPtr, rspStatusCode);
    OSLM_WARN("PACS RESPONSE :" << rspStatusCode);
    return result;
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

OFList< QRResponse* > SeriesEnquirer::findSeriesByUID(const std::string& uid)
{
    // Dataset used to store query informations
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, uid.c_str());

    // Fields needed by DICOMSeries
    dataset.putAndInsertOFStringArray(DCM_PatientName, "");
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
    OFIterator< QRResponse* > it    = responses.begin();
    std::string sopInstanceUID      = "";
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
    // Reset instance count
    m_instanceIndex = 0;

    DcmDataset dataset;
    OFCondition result;

    for( std::string seriesInstanceUID: instanceUIDContainer )
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
            throw ::fwPacsIO::exceptions::RequestFailure(msg);
        }
    }

}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pullSeriesUsingGetRetrieveMethod(InstanceUIDContainer instanceUIDContainer)
{
    // Reset instance count
    m_instanceIndex = 0;

    DcmDataset dataset;
    OFCondition result;

    for( std::string seriesInstanceUID: instanceUIDContainer )
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
            throw ::fwPacsIO::exceptions::RequestFailure(msg);
        }
    }
}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pullInstanceUsingMoveRetrieveMethod(const std::string& seriesInstanceUID,
                                                         const std::string& sopInstanceUID)
{
    // Reset instance count
    m_instanceIndex = 0;

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
        throw ::fwPacsIO::exceptions::RequestFailure(msg);
    }
}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pullInstanceUsingGetRetrieveMethod(const std::string& seriesInstanceUID,
                                                        const std::string& sopInstanceUID)
{
    // Reset instance count
    m_instanceIndex = 0;

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
        throw ::fwPacsIO::exceptions::RequestFailure(msg);
    }
}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pushSeries(const InstancePathContainer& pathContainer)
{
    // Reset instance count
    m_instanceIndex = 0;

    OFCondition result;

    // Send images to pacs
    for(const ::boost::filesystem::path& path: pathContainer)
    {
        result = this->sendStoreRequest(path);

        if (result.good())
        {
            SLM_TRACE("Instance sent.");
        }
        else
        {
            const std::string msg = "Unable to send a C-STORE request to the server : " + std::string(result.text());
            throw ::fwPacsIO::exceptions::RequestFailure(msg);
        }

        // Notify callback
        if(m_progressCallback)
        {
            m_progressCallback->asyncRun("", ++m_instanceIndex, path.string());
        }

    }
}

// ----------------------------------------------------------------------------

void SeriesEnquirer::pushSeries(const DatasetContainer& datasetContainer)
{
    // Reset instance count
    m_instanceIndex = 0;
    OFCondition result;
    // Send images to pacs
    for(const auto& dataset : datasetContainer)
    {
        result = this->sendStoreRequest(dataset);

        if (result.good())
        {
            SLM_TRACE("Instance sent.");
        }
        else
        {
            const std::string msg = "Unable to send a C-STORE request to the server : " + std::string(result.text());
            throw ::fwPacsIO::exceptions::RequestFailure(msg);
        }

        // Notify callback
        if(m_progressCallback)
        {
            m_progressCallback->asyncRun("", ++m_instanceIndex, "");
        }
    }
}

// ----------------------------------------------------------------------------

OFCondition SeriesEnquirer::handleMOVEResponse(
    const T_ASC_PresentationContextID presID, RetrieveResponse* response, OFBool& waitForNextResponse)
{
    OFCondition result = DcmSCU::handleMOVEResponse(presID, response, waitForNextResponse);

    // Check error status
    bool error = (response->m_status != STATUS_Success) && (response->m_status != STATUS_Pending);

    // Notify error
    if(error)
    {
        const std::string msg = "Unable to perform a C-MOVE operation.";
        throw ::fwPacsIO::exceptions::RequestFailure(msg);
    }

    return result;
}

// ----------------------------------------------------------------------------

OFCondition SeriesEnquirer::handleSTORERequest (
    const T_ASC_PresentationContextID presID, DcmDataset* incomingObject,
    OFBool& continueCGETSession, Uint16& cStoreReturnStatus)
{
    OFCondition result;

    if (incomingObject != NULL)
    {
        //Find the series UID
        OFString seriesID;
        if(incomingObject->findAndGetOFStringArray(DCM_SeriesInstanceUID, seriesID).good())
        {
            SLM_TRACE("Series Instance UID: " + std::string(seriesID.c_str()));
        }

        //Find the instance UID
        OFString iname;
        if (incomingObject->findAndGetOFStringArray(DCM_SOPInstanceUID, iname).good())
        {
            SLM_TRACE("SOP Instance UID: " + std::string(iname.c_str()));
        }

        //Create Folder
        ::boost::filesystem::path seriesPath = ::boost::filesystem::path(m_path.string() + seriesID.c_str() + "/");
        if (!::boost::filesystem::exists(seriesPath))
        {
            ::boost::filesystem::create_directories(seriesPath);
        }

        //Save the file in the specified folder (Create new meta header for gdcm reader)
        std::string filePath = seriesPath.string() + iname.c_str();
        DcmFileFormat fileFormat(incomingObject);
        fileFormat.saveFile(filePath.c_str(), EXS_Unknown, EET_UndefinedLength,
                            EGL_recalcGL, EPD_noChange, 0, 0, EWM_createNewMeta);

        // Notify callback
        if(m_progressCallback)
        {
            m_progressCallback->asyncRun(seriesID.c_str(), ++m_instanceIndex, filePath);
        }
    }

    return result;
}

// ----------------------------------------------------------------------------

} //namespace fwPacsIO
