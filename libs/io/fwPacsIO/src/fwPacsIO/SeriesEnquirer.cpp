/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwPacsIO/SeriesEnquirer.hpp"

#include "fwPacsIO/exceptions/NegociateAssociationFailure.hpp"
#include "fwPacsIO/exceptions/NetworkInitializationFailure.hpp"
#include "fwPacsIO/exceptions/PresentationContextMissing.hpp"
#include "fwPacsIO/exceptions/RequestFailure.hpp"
#include "fwPacsIO/exceptions/TagMissing.hpp"

#include <fwTools/System.hpp>

#include <boost/foreach.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmnet/diutil.h>

#include <filesystem>

namespace fwPacsIO
{

const ::fwCom::Slots::SlotKeyType SeriesEnquirer::s_PROGRESS_CALLBACK_SLOT = "CGetProgressCallback";

//------------------------------------------------------------------------------

SeriesEnquirer::SeriesEnquirer() :
    m_progressCallback(ProgressCallbackSlotType::sptr())
{

}

//------------------------------------------------------------------------------

SeriesEnquirer::~SeriesEnquirer()
{
}

//------------------------------------------------------------------------------

void SeriesEnquirer::initialize(const std::string& _applicationTitle,
                                const std::string& _peerHostName,
                                unsigned short _peerPort,
                                const std::string& _peerApplicationTitle,
                                const std::string& _moveApplicationTitle,
                                ProgressCallbackSlotType::sptr _progressCallback)
{
    // Save move application title for move requests.
    m_moveApplicationTitle = _moveApplicationTitle;

    // Store Callback.
    m_progressCallback = _progressCallback;

    // Creating folder.
    m_path = ::fwTools::System::getTemporaryFolder() / "dicom/";
    if (!m_path.empty() && !std::filesystem::exists(m_path))
    {
        std::filesystem::create_directories(m_path);
    }

    // Configure network connection.
    this->setAETitle(_applicationTitle.c_str());
    this->setPeerHostName(_peerHostName.c_str());
    this->setPeerPort(_peerPort);
    this->setPeerAETitle(_peerApplicationTitle.c_str());

    SLM_INFO("Initialize connection to (" +
             std::string(this->getPeerAETitle().c_str()) + ") " +
             std::string(this->getPeerHostName().c_str()) + ":" +
             std::to_string(this->getPeerPort()))

    // Clear presentation context.
    this->clearPresentationContexts();

    // Use presentation context for C-FIND/C-MOVE in study root, propose all uncompressed transfer syntaxes.
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

    // Add Verification SOP Class presentation context.
    this->addPresentationContext(UID_VerificationSOPClass, transferSyntaxes);

    // Add study presentation context.
    this->addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, transferSyntaxes);
    this->addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, transferSyntaxes);
    this->addPresentationContext(UID_GETStudyRootQueryRetrieveInformationModel, transferSyntaxes);

    // Add presentation context for C-GET store requests.
    for (Uint16 j = 0; j < numberOfDcmLongSCUStorageSOPClassUIDs; j++)
    {
        this->addPresentationContext(dcmLongSCUStorageSOPClassUIDs[j], transferSyntaxes, ASC_SC_ROLE_SCP);
    }
}

//------------------------------------------------------------------------------

bool SeriesEnquirer::connect()
{
    SLM_INFO("Connect to (" +
             std::string(this->getPeerAETitle().c_str()) + ") " +
             std::string(this->getPeerHostName().c_str()) + ":" +
             std::to_string(this->getPeerPort()))

    // Initialize network.
    OFCondition result = this->initNetwork();
    if (result.bad())
    {
        const std::string msg = "Unable to set up the network: " + std::string(result.text());
        throw ::fwPacsIO::exceptions::NetworkInitializationFailure(msg);
    }

    // Negotiate association.
    result = this->negotiateAssociation();
    if (result.bad())
    {
        const std::string msg = "Unable to negotiate association: " + std::string(result.text());
        throw ::fwPacsIO::exceptions::NegociateAssociationFailure(msg);
    }

    return true;

}

//------------------------------------------------------------------------------

bool SeriesEnquirer::isConnectedToPacs() const
{
    return this->isConnected();
}

//------------------------------------------------------------------------------

bool SeriesEnquirer::pingPacs()
{
    SLM_INFO("Send C-ECHO request")
    return this->sendECHORequest(0).good();
}

//------------------------------------------------------------------------------

void SeriesEnquirer::disconnect()
{
    SLM_INFO("Disconnect from (" +
             std::string(this->getPeerAETitle().c_str()) + ") " +
             std::string(this->getPeerHostName().c_str()) + ":" +
             std::to_string(this->getPeerPort()))
    this->releaseAssociation();
}

//------------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::sendFindRequest(DcmDataset _dataset)
{
    OFList< QRResponse* > findResponses;

    // Try to find a presentation context.
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_FINDStudyRootQueryRetrieveInformationModel);
    if (presID == 0)
    {
        const std::string msg = "There is no uncompressed presentation context for Study Root FIND";
        throw ::fwPacsIO::exceptions::PresentationContextMissing(msg);
    }

    // Send the request
    std::ostringstream stream;
    _dataset.print(stream);
    SLM_INFO("Send C-FIND request : " + stream.str())
    this->sendFINDRequest(presID, &_dataset, &findResponses);

    return findResponses;
}

//------------------------------------------------------------------------------

OFCondition SeriesEnquirer::sendMoveRequest(DcmDataset _dataset)
{
    // Be sure that the needed informations are set.
    SLM_ASSERT("The path where to store the series is not set.", !m_path.empty());
    SLM_ASSERT("The move application title is not set.", !m_moveApplicationTitle.empty());

    // Try to find a presentation context.
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel);
    SLM_WARN_IF("There is no uncompressed presentation context for Study Root MOVE", presID == 0);

    // Fetches all images of this particular study.
    OFList< RetrieveResponse* > dataResponse;
    std::ostringstream stream;
    _dataset.print(stream);
    SLM_INFO("Send C-MOVE request : " + stream.str())
    return this->sendMOVERequest(presID, m_moveApplicationTitle.c_str(), &_dataset, &dataResponse);
}

//------------------------------------------------------------------------------

OFCondition SeriesEnquirer::sendGetRequest(DcmDataset _dataset)
{
    // Be sure that the needed informations are set.
    SLM_ASSERT("The path where to store the series is not set.", !m_path.empty());

    // Try to find a presentation context.
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_GETStudyRootQueryRetrieveInformationModel);

    if (presID == 0)
    {
        SLM_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    // Fetches all images of this particular study.
    OFList< RetrieveResponse* > dataResponse;
    std::ostringstream stream;
    _dataset.print(stream);
    SLM_INFO("Send C-GET request : " + stream.str())
    return this->sendCGETRequest(presID, &_dataset, &dataResponse);
}

//------------------------------------------------------------------------------

OFCondition SeriesEnquirer::sendStoreRequest(const std::filesystem::path& _path)
{
    // Try to find a presentation context.
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel);

    if (presID == 0)
    {
        SLM_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    Uint16 rspStatusCode;
    SLM_INFO("Send C-STORE request")
    OFCondition result = this->sendSTORERequest(presID, OFString(_path.string().c_str()), 0, rspStatusCode);
    return result;
}

//------------------------------------------------------------------------------

OFCondition SeriesEnquirer::sendStoreRequest(const CSPTR(DcmDataset)& _dataset)
{
    // Try to find a presentation context.
    T_ASC_PresentationContextID presID = this->findUncompressedPC(UID_MOVEStudyRootQueryRetrieveInformationModel);

    if (presID == 0)
    {
        SLM_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    Uint16 rspStatusCode;
    // const_cast required to use bad DCMTK sendSTORERequest API
    DcmDataset* datasetPtr = const_cast<DcmDataset*>(_dataset.get());
    SLM_INFO("Send C-STORE request")
    OFCondition result = this->sendSTORERequest(presID, OFString(""), datasetPtr, rspStatusCode);
    return result;
}

//------------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByPatientName(const std::string& _name)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string searchString = "*" + _name + "*";
    dataset.putAndInsertOFStringArray(DCM_PatientName, searchString.c_str());

    // Fields needed by Series.
    dataset.putAndInsertOFStringArray(DCM_Modality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesNumber, "");
    dataset.putAndInsertOFStringArray(DCM_Laterality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ProtocolName, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, "");
    dataset.putAndInsertOFStringArray(DCM_BodyPartExamined, "");
    dataset.putAndInsertOFStringArray(DCM_PatientPosition, "");
    dataset.putAndInsertOFStringArray(DCM_AnatomicalOrientationType, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepID, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepDescription, "");
    dataset.putAndInsertOFStringArray(DCM_CommentsOnThePerformedProcedureStep, "");

    // Fields needed by Study.
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ConsultingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSize, "");
    dataset.putAndInsertOFStringArray(DCM_PatientWeight, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBodyMassIndex, "");

    // Fields needed by Patient.
    dataset.putAndInsertOFStringArray(DCM_PatientID, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");

    // Fields needed by Equipment.
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");

    // Number of instances.
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

//------------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByPatientUID(const std::string& _uid)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string searchString = "*" + _uid + "*";
    dataset.putAndInsertOFStringArray(DCM_PatientID, searchString.c_str());

    // Fields needed by Series.
    dataset.putAndInsertOFStringArray(DCM_Modality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesNumber, "");
    dataset.putAndInsertOFStringArray(DCM_Laterality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ProtocolName, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, "");
    dataset.putAndInsertOFStringArray(DCM_BodyPartExamined, "");
    dataset.putAndInsertOFStringArray(DCM_PatientPosition, "");
    dataset.putAndInsertOFStringArray(DCM_AnatomicalOrientationType, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepID, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepDescription, "");
    dataset.putAndInsertOFStringArray(DCM_CommentsOnThePerformedProcedureStep, "");

    // Fields needed by Study.
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ConsultingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSize, "");
    dataset.putAndInsertOFStringArray(DCM_PatientWeight, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBodyMassIndex, "");

    // Fields needed by Patient.
    dataset.putAndInsertOFStringArray(DCM_PatientName, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");

    // Fields needed by Equipment.
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");

    // Number of instances.
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

//------------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByPatientBirthDate(const std::string& _birthDate)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series birthdate.
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, _birthDate.c_str());

    // Fields needed by Series.
    dataset.putAndInsertOFStringArray(DCM_Modality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesNumber, "");
    dataset.putAndInsertOFStringArray(DCM_Laterality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ProtocolName, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, "");
    dataset.putAndInsertOFStringArray(DCM_BodyPartExamined, "");
    dataset.putAndInsertOFStringArray(DCM_PatientPosition, "");
    dataset.putAndInsertOFStringArray(DCM_AnatomicalOrientationType, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepID, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepDescription, "");
    dataset.putAndInsertOFStringArray(DCM_CommentsOnThePerformedProcedureStep, "");

    // Fields needed by Study.
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ConsultingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSize, "");
    dataset.putAndInsertOFStringArray(DCM_PatientWeight, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBodyMassIndex, "");

    // Fields needed by Patient.
    dataset.putAndInsertOFStringArray(DCM_PatientName, "");
    dataset.putAndInsertOFStringArray(DCM_PatientID, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");

    // Fields needed by Equipment.
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");

    // Number of instances.
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

//------------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByDate(const std::string& _fromDate, const std::string& _toDate)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string searchString = _fromDate +"-" + _toDate;
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, searchString.c_str());

    // Fields needed by Series.
    dataset.putAndInsertOFStringArray(DCM_Modality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesNumber, "");
    dataset.putAndInsertOFStringArray(DCM_Laterality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ProtocolName, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, "");
    dataset.putAndInsertOFStringArray(DCM_BodyPartExamined, "");
    dataset.putAndInsertOFStringArray(DCM_PatientPosition, "");
    dataset.putAndInsertOFStringArray(DCM_AnatomicalOrientationType, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepID, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepDescription, "");
    dataset.putAndInsertOFStringArray(DCM_CommentsOnThePerformedProcedureStep, "");

    // Fields needed by Study.
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ConsultingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSize, "");
    dataset.putAndInsertOFStringArray(DCM_PatientWeight, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBodyMassIndex, "");

    // Fields needed by Patient.
    dataset.putAndInsertOFStringArray(DCM_PatientName, "");
    dataset.putAndInsertOFStringArray(DCM_PatientID, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");

    // Fields needed by Equipment.
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");

    // Number of instances.
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

//------------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByUID(const std::string& _uid)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string searchString = "*" + _uid + "*";
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, searchString.c_str());

    // Fields needed by Series.
    dataset.putAndInsertOFStringArray(DCM_Modality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesNumber, "");
    dataset.putAndInsertOFStringArray(DCM_Laterality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ProtocolName, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, "");
    dataset.putAndInsertOFStringArray(DCM_BodyPartExamined, "");
    dataset.putAndInsertOFStringArray(DCM_PatientPosition, "");
    dataset.putAndInsertOFStringArray(DCM_AnatomicalOrientationType, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepID, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepDescription, "");
    dataset.putAndInsertOFStringArray(DCM_CommentsOnThePerformedProcedureStep, "");

    // Fields needed by Study.
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ConsultingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSize, "");
    dataset.putAndInsertOFStringArray(DCM_PatientWeight, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBodyMassIndex, "");

    // Fields needed by Patient.
    dataset.putAndInsertOFStringArray(DCM_PatientName, "");
    dataset.putAndInsertOFStringArray(DCM_PatientID, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");

    // Fields needed by Equipment.
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");

    // Number of instances.
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

//------------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByModality(const std::string& _modality)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string searchString = "*" + _modality + "*";
    dataset.putAndInsertOFStringArray(DCM_Modality, searchString.c_str());

    // Fields needed by Series.
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesNumber, "");
    dataset.putAndInsertOFStringArray(DCM_Laterality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ProtocolName, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, "");
    dataset.putAndInsertOFStringArray(DCM_BodyPartExamined, "");
    dataset.putAndInsertOFStringArray(DCM_PatientPosition, "");
    dataset.putAndInsertOFStringArray(DCM_AnatomicalOrientationType, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepID, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepDescription, "");
    dataset.putAndInsertOFStringArray(DCM_CommentsOnThePerformedProcedureStep, "");

    // Fields needed by Study.
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ConsultingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSize, "");
    dataset.putAndInsertOFStringArray(DCM_PatientWeight, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBodyMassIndex, "");

    // Fields needed by Patient.
    dataset.putAndInsertOFStringArray(DCM_PatientName, "");
    dataset.putAndInsertOFStringArray(DCM_PatientID, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");

    // Fields needed by Equipment.
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");

    // Number of instances.
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

//------------------------------------------------------------------------------

OFList< QRResponse* > SeriesEnquirer::findSeriesByDescription(const std::string& _description)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string searchString = "*" + _description + "*";
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, searchString.c_str());

    // Fields needed by Series.
    dataset.putAndInsertOFStringArray(DCM_Modality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesNumber, "");
    dataset.putAndInsertOFStringArray(DCM_Laterality, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
    dataset.putAndInsertOFStringArray(DCM_SeriesTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ProtocolName, "");
    dataset.putAndInsertOFStringArray(DCM_BodyPartExamined, "");
    dataset.putAndInsertOFStringArray(DCM_PatientPosition, "");
    dataset.putAndInsertOFStringArray(DCM_AnatomicalOrientationType, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepID, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepStartTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndDate, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepEndTime, "");
    dataset.putAndInsertOFStringArray(DCM_PerformedProcedureStepDescription, "");
    dataset.putAndInsertOFStringArray(DCM_CommentsOnThePerformedProcedureStep, "");

    // Fields needed by Study.
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyID, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDate, "");
    dataset.putAndInsertOFStringArray(DCM_StudyTime, "");
    dataset.putAndInsertOFStringArray(DCM_ReferringPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_ConsultingPhysicianName, "");
    dataset.putAndInsertOFStringArray(DCM_StudyDescription, "");
    dataset.putAndInsertOFStringArray(DCM_PatientAge, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSize, "");
    dataset.putAndInsertOFStringArray(DCM_PatientWeight, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBodyMassIndex, "");

    // Fields needed by Patient.
    dataset.putAndInsertOFStringArray(DCM_PatientName, "");
    dataset.putAndInsertOFStringArray(DCM_PatientID, "");
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, "");
    dataset.putAndInsertOFStringArray(DCM_PatientSex, "");

    // Fields needed by Equipment.
    dataset.putAndInsertOFStringArray(DCM_InstitutionName, "");

    // Number of instances.
    dataset.putAndInsertOFStringArray(DCM_NumberOfSeriesRelatedInstances, "");

    return this->sendFindRequest(dataset);
}

//------------------------------------------------------------------------------

std::string SeriesEnquirer::findSOPInstanceUID(const std::string& _seriesInstanceUID, unsigned int _instanceNumber)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, _seriesInstanceUID.c_str());
    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, "");
    std::stringstream ss;
    ss << _instanceNumber;
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

    //Release responses.
    while (!responses.empty())
    {
        delete responses.front();
        responses.pop_front();
    }

    return sopInstanceUID;

}

//------------------------------------------------------------------------------

void SeriesEnquirer::pullSeriesUsingMoveRetrieveMethod(InstanceUIDContainer _instanceUIDContainer)
{
    // Reset instance count.
    m_instanceIndex = 0;

    DcmDataset dataset;
    OFCondition result;

    for( std::string seriesInstanceUID: _instanceUIDContainer )
    {
        dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");
        dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID.c_str());

        // Fetches all images of this particular study.
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

//------------------------------------------------------------------------------

void SeriesEnquirer::pullSeriesUsingGetRetrieveMethod(InstanceUIDContainer _instanceUIDContainer)
{
    // Reset instance count.
    m_instanceIndex = 0;

    DcmDataset dataset;
    OFCondition result;

    for( std::string seriesInstanceUID: _instanceUIDContainer )
    {
        dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");
        dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID.c_str());

        // Fetches all images of this particular study.
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

//------------------------------------------------------------------------------

void SeriesEnquirer::pullInstanceUsingMoveRetrieveMethod(const std::string& _seriesInstanceUID,
                                                         const std::string& _sopInstanceUID)
{
    // Reset instance count.
    m_instanceIndex = 0;

    DcmDataset dataset;
    OFCondition result;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, _seriesInstanceUID.c_str());

    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, _sopInstanceUID.c_str());
    dataset.putAndInsertOFStringArray(DCM_InstanceNumber, "");

    // Fetches all images of this particular study.
    result = this->sendMoveRequest(dataset);

    if (result.good())
    {
        SLM_TRACE("Received instance " + _seriesInstanceUID + " - " + _sopInstanceUID);
    }
    else
    {
        const std::string msg = "Unable to send a C-MOVE request to the server. "
                                "(Series instance UID =" + std::string(_seriesInstanceUID.c_str()) +") : "
                                + std::string(result.text());
        throw ::fwPacsIO::exceptions::RequestFailure(msg);
    }
}

//------------------------------------------------------------------------------

void SeriesEnquirer::pullInstanceUsingGetRetrieveMethod(const std::string& _seriesInstanceUID,
                                                        const std::string& _sopInstanceUID)
{
    // Reset instance count.
    m_instanceIndex = 0;

    DcmDataset dataset;
    OFCondition result;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, _seriesInstanceUID.c_str());

    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, _sopInstanceUID.c_str());
    dataset.putAndInsertOFStringArray(DCM_InstanceNumber, "");

    // Fetches all images of this particular study.
    result = this->sendGetRequest(dataset);

    if (result.good())
    {
        SLM_TRACE("Received instance " + _seriesInstanceUID + " - " + _sopInstanceUID);
    }
    else
    {
        const std::string msg = "Unable to send a C-GET request to the server. "
                                "(Series instance UID =" + std::string(_seriesInstanceUID.c_str()) +") : "
                                + std::string(result.text());
        throw ::fwPacsIO::exceptions::RequestFailure(msg);
    }
}

//------------------------------------------------------------------------------

void SeriesEnquirer::pushSeries(const InstancePathContainer& _pathContainer)
{
    // Reset instance count.
    m_instanceIndex = 0;

    OFCondition result;

    // Send images to pacs.
    for(const std::filesystem::path& path: _pathContainer)
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

        // Notify callback.
        if(m_progressCallback)
        {
            m_progressCallback->asyncRun("", ++m_instanceIndex, path.string());
        }

    }
}

//------------------------------------------------------------------------------

void SeriesEnquirer::pushSeries(const DatasetContainer& _datasetContainer)
{
    // Reset instance count.
    m_instanceIndex = 0;
    OFCondition result;
    // Send images to pacs.
    for(const auto& dataset : _datasetContainer)
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

        // Notify callback.
        if(m_progressCallback)
        {
            m_progressCallback->asyncRun("", ++m_instanceIndex, "");
        }
    }
}

//------------------------------------------------------------------------------

OFCondition SeriesEnquirer::handleMOVEResponse(const T_ASC_PresentationContextID _presID,
                                               RetrieveResponse* _response,
                                               OFBool& _waitForNextResponse)
{
    OFCondition result = DcmSCU::handleMOVEResponse(_presID, _response, _waitForNextResponse);

    // Check error status.
    const bool error = (_response->m_status != STATUS_Success) && (_response->m_status != STATUS_Pending);

    // Notify error.
    if(error)
    {
        const std::string msg = "Unable to perform a C-MOVE operation.";
        throw ::fwPacsIO::exceptions::RequestFailure(msg);
    }

    return result;
}

//------------------------------------------------------------------------------

OFCondition SeriesEnquirer::handleSTORERequest(const T_ASC_PresentationContextID,
                                               DcmDataset* _incomingObject,
                                               OFBool&,
                                               Uint16&)
{
    OFCondition result;

    if(_incomingObject != NULL)
    {
        // Find the series UID.
        OFString seriesID;
        if(_incomingObject->findAndGetOFStringArray(DCM_SeriesInstanceUID, seriesID).good())
        {
            SLM_TRACE("Series Instance UID: " + std::string(seriesID.c_str()));
        }

        // Find the instance UID.
        OFString iname;
        if(_incomingObject->findAndGetOFStringArray(DCM_SOPInstanceUID, iname).good())
        {
            SLM_TRACE("SOP Instance UID: " + std::string(iname.c_str()));
        }

        // Create Folder.
        std::filesystem::path seriesPath = std::filesystem::path(m_path.string() + seriesID.c_str() + "/");
        if(!std::filesystem::exists(seriesPath))
        {
            std::filesystem::create_directories(seriesPath);
        }

        // Save the file in the specified folder (Create new meta header for gdcm reader).
        std::string filePath = seriesPath.string() + iname.c_str();
        DcmFileFormat fileFormat(_incomingObject);
        fileFormat.saveFile(filePath.c_str(), EXS_Unknown, EET_UndefinedLength,
                            EGL_recalcGL, EPD_noChange, 0, 0, EWM_createNewMeta);

        // Notify callback.
        if(m_progressCallback)
        {
            m_progressCallback->asyncRun(seriesID.c_str(), ++m_instanceIndex, filePath);
        }
    }

    return result;
}

//------------------------------------------------------------------------------

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

} // namespace fwPacsIO.
