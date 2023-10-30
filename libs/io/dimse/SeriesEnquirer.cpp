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

#include "SeriesEnquirer.hpp"

#include "io/dimse/exceptions/NegociateAssociationFailure.hpp"
#include "io/dimse/exceptions/NetworkInitializationFailure.hpp"
#include "io/dimse/exceptions/PresentationContextMissing.hpp"
#include "io/dimse/exceptions/RequestFailure.hpp"
#include "io/dimse/exceptions/TagMissing.hpp"

#include <core/os/temp_path.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmnet/diutil.h>

#include <filesystem>

/**
 * Do not mark `JPEGLS`, `JPIP` as incorrect.
 * cspell:ignore JPEGLS JPIP NOLINT
 */

#ifdef HAVE_STL_LIST
#define DCMTK_EMPLACE_BACK emplace_back
#else
#define DCMTK_EMPLACE_BACK push_back
#endif

namespace sight::io::dimse
{

const core::com::slots::key_t series_enquirer::PROGRESS_CALLBACK_SLOT = "CGetProgressCallback";

//------------------------------------------------------------------------------

series_enquirer::series_enquirer()
= default;

//------------------------------------------------------------------------------

series_enquirer::~series_enquirer()
= default;

//------------------------------------------------------------------------------

void series_enquirer::initialize(
    const std::string& _application_title,
    const std::string& _peer_host_name,
    std::uint16_t _peer_port,
    const std::string& _peer_application_title,
    const std::string& _move_application_title,
    progress_callback_slot_t::sptr _progress_callback
)
{
    // Save move application title for move requests.
    m_move_application_title = _move_application_title;

    // Store Callback.
    m_progress_callback = _progress_callback;

    // Creating folder.
    m_path = core::os::temp_dir::shared_directory() / "dicom/";
    if(!m_path.empty() && !std::filesystem::exists(m_path))
    {
        std::filesystem::create_directories(m_path);
    }

    // Configure network connection.
    this->setAETitle(_application_title.c_str());
    this->setPeerHostName(_peer_host_name.c_str());
    this->setPeerPort(_peer_port);
    this->setPeerAETitle(_peer_application_title.c_str());

    SIGHT_INFO(
        "Initialize connection to ("
        + std::string(this->getPeerAETitle().c_str()) + ") "
        + std::string(this->getPeerHostName().c_str()) + ":"
        + std::to_string(this->getPeerPort())
    )

    // Clear presentation context.
    this->clearPresentationContexts();

    // Use presentation context for C-FIND/C-MOVE in study root, propose all uncompressed transfer syntaxes.
    OFList<OFString> transfer_syntaxes;
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_LittleEndianImplicitTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_LittleEndianExplicitTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_BigEndianExplicitTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess1TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess2_4TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess3_5TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess6_8TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess7_9TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess10_12TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess11_13TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess14TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess15TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess16_18TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess17_19TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess20_22TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess21_23TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess24_26TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess25_27TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess28TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess29TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGProcess14SV1TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGLSLosslessTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEGLSLossyTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_RLELosslessTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_DeflatedExplicitVRLittleEndianTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEG2000LosslessOnlyTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEG2000TransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_MPEG2MainProfileAtMainLevelTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_MPEG2MainProfileAtHighLevelTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnlyTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPEG2000Part2MulticomponentImageCompressionTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPIPReferencedTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_JPIPReferencedDeflateTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_RFC2557MIMEEncapsulationTransferSyntax);
    transfer_syntaxes.DCMTK_EMPLACE_BACK(UID_XMLEncodingTransferSyntax);

    // Add Verification SOP Class presentation context.
    this->addPresentationContext(UID_VerificationSOPClass, transfer_syntaxes);

    // Add study presentation context.
    this->addPresentationContext(UID_FINDStudyRootQueryRetrieveInformationModel, transfer_syntaxes);
    this->addPresentationContext(UID_MOVEStudyRootQueryRetrieveInformationModel, transfer_syntaxes);
    this->addPresentationContext(UID_GETStudyRootQueryRetrieveInformationModel, transfer_syntaxes);

    // Add presentation context for C-GET store requests.
    for(Uint16 j = 0 ; j < numberOfDcmLongSCUStorageSOPClassUIDs ; j++)
    {
        this->addPresentationContext(dcmLongSCUStorageSOPClassUIDs[j], transfer_syntaxes, ASC_SC_ROLE_SCP);
    }
}

//------------------------------------------------------------------------------

bool series_enquirer::connect()
{
    SIGHT_INFO(
        "Connect to ("
        + std::string(this->getPeerAETitle().c_str()) + ") "
        + std::string(this->getPeerHostName().c_str()) + ":"
        + std::to_string(this->getPeerPort())
    )

    // Initialize network.
    OFCondition result = this->initNetwork();
    if(result.bad())
    {
        const std::string msg = "Unable to set up the network: " + std::string(result.text());
        throw io::dimse::exceptions::network_initialization_failure(msg);
    }

    // Negotiate association.
    result = this->negotiateAssociation();
    if(result.bad())
    {
        const std::string msg = "Unable to negotiate association: " + std::string(result.text());
        throw io::dimse::exceptions::negociate_association_failure(msg);
    }

    return true;
}

//------------------------------------------------------------------------------

bool series_enquirer::is_connected_to_pacs() const
{
    return this->isConnected();
}

//------------------------------------------------------------------------------

bool series_enquirer::ping_pacs()
{
    SIGHT_INFO("Send C-ECHO request")
    return this->sendECHORequest(0).good();
}

//------------------------------------------------------------------------------

void series_enquirer::disconnect()
{
    SIGHT_INFO(
        "Disconnect from ("
        + std::string(this->getPeerAETitle().c_str()) + ") "
        + std::string(this->getPeerHostName().c_str()) + ":"
        + std::to_string(this->getPeerPort())
    )
    this->releaseAssociation();
}

//------------------------------------------------------------------------------

OFList<QRResponse*> series_enquirer::send_find_request(DcmDataset _dataset)
{
    OFList<QRResponse*> find_responses;

    // Try to find a presentation context.
    T_ASC_PresentationContextID pres_id = this->find_uncompressed_pc(UID_FINDStudyRootQueryRetrieveInformationModel);
    if(pres_id == 0)
    {
        const std::string msg = "There is no uncompressed presentation context for Study Root FIND";
        throw io::dimse::exceptions::presentation_context_missing(msg);
    }

    // Send the request
    std::ostringstream stream;
    _dataset.print(stream);
    SIGHT_INFO("Send C-FIND request : " + stream.str())
    this->sendFINDRequest(pres_id, &_dataset, &find_responses);

    return find_responses;
}

//------------------------------------------------------------------------------

OFCondition series_enquirer::send_move_request(DcmDataset _dataset)
{
    // Be sure that the needed informations are set.
    SIGHT_ASSERT("The path where to store the series is not set.", !m_path.empty());
    SIGHT_ASSERT("The move application title is not set.", !m_move_application_title.empty());

    // Try to find a presentation context.
    T_ASC_PresentationContextID pres_id = this->find_uncompressed_pc(UID_MOVEStudyRootQueryRetrieveInformationModel);
    SIGHT_WARN_IF("There is no uncompressed presentation context for Study Root MOVE", pres_id == 0);

    // Fetches all images of this particular study.
    OFList<RetrieveResponse*> data_response;
    std::ostringstream stream;
    _dataset.print(stream);
    SIGHT_INFO("Send C-MOVE request : " + stream.str())
    return this->sendMOVERequest(pres_id, m_move_application_title.c_str(), &_dataset, &data_response);
}

//------------------------------------------------------------------------------

OFCondition series_enquirer::send_get_request(DcmDataset _dataset)
{
    // Be sure that the needed informations are set.
    SIGHT_ASSERT("The path where to store the series is not set.", !m_path.empty());

    // Try to find a presentation context.
    T_ASC_PresentationContextID pres_id = this->find_uncompressed_pc(UID_GETStudyRootQueryRetrieveInformationModel);

    if(pres_id == 0)
    {
        SIGHT_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    // Fetches all images of this particular study.
    OFList<RetrieveResponse*> data_response;
    std::ostringstream stream;
    _dataset.print(stream);
    SIGHT_INFO("Send C-GET request : " + stream.str())
    return this->sendCGETRequest(pres_id, &_dataset, &data_response);
}

//------------------------------------------------------------------------------

OFCondition series_enquirer::send_store_request(const std::filesystem::path& _path)
{
    // Try to find a presentation context.
    T_ASC_PresentationContextID pres_id = this->find_uncompressed_pc(UID_MOVEStudyRootQueryRetrieveInformationModel);

    if(pres_id == 0)
    {
        SIGHT_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    Uint16 rsp_status_code = 0;
    SIGHT_INFO("Send C-STORE request")
    OFCondition result = this->sendSTORERequest(pres_id, OFString(_path.string().c_str()), nullptr, rsp_status_code);
    return result;
}

//------------------------------------------------------------------------------

OFCondition series_enquirer::send_store_request(const CSPTR(DcmDataset)& _dataset)
{
    // Try to find a presentation context.
    T_ASC_PresentationContextID pres_id = this->find_uncompressed_pc(UID_MOVEStudyRootQueryRetrieveInformationModel);

    if(pres_id == 0)
    {
        SIGHT_WARN("There is no uncompressed presentation context for Study Root GET");
    }

    Uint16 rsp_status_code = 0;

    // const_cast required to use bad DCMTK sendSTORERequest API
    auto* dataset_ptr = const_cast<DcmDataset*>(_dataset.get()); // NOLINT(cppcoreguidelines-pro-type-const-cast)
    SIGHT_INFO("Send C-STORE request")
    OFCondition result = this->sendSTORERequest(pres_id, OFString(""), dataset_ptr, rsp_status_code);
    return result;
}

//------------------------------------------------------------------------------

OFList<QRResponse*> series_enquirer::find_series_by_patient_name(const std::string& _name)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string search_string = "*" + _name + "*";
    dataset.putAndInsertOFStringArray(DCM_PatientName, search_string.c_str());

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

    return this->send_find_request(dataset);
}

//------------------------------------------------------------------------------

OFList<QRResponse*> series_enquirer::find_series_by_patient_uid(const std::string& _uid)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string search_string = "*" + _uid + "*";
    dataset.putAndInsertOFStringArray(DCM_PatientID, search_string.c_str());

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

    return this->send_find_request(dataset);
}

//------------------------------------------------------------------------------

OFList<QRResponse*> series_enquirer::find_series_by_patient_birth_date(const std::string& _birth_date)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series birthdate.
    dataset.putAndInsertOFStringArray(DCM_PatientBirthDate, _birth_date.c_str());

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

    return this->send_find_request(dataset);
}

//------------------------------------------------------------------------------

OFList<QRResponse*> series_enquirer::find_series_by_date(const std::string& _from_date, const std::string& _to_date)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string search_string = _from_date + "-" + _to_date;
    dataset.putAndInsertOFStringArray(DCM_StudyDate, search_string.c_str());

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
    dataset.putAndInsertOFStringArray(DCM_SeriesDate, "");
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

    return this->send_find_request(dataset);
}

//------------------------------------------------------------------------------

OFList<QRResponse*> series_enquirer::find_series_by_uid(const std::string& _uid)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string search_string = "*" + _uid + "*";
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, search_string.c_str());

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

    return this->send_find_request(dataset);
}

//------------------------------------------------------------------------------

OFList<QRResponse*> series_enquirer::find_series_by_modality(const std::string& _modality)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string search_string = "*" + _modality + "*";
    dataset.putAndInsertOFStringArray(DCM_Modality, search_string.c_str());

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

    return this->send_find_request(dataset);
}

//------------------------------------------------------------------------------

OFList<QRResponse*> series_enquirer::find_series_by_description(const std::string& _description)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");

    // Search by series UID.
    const std::string search_string = "*" + _description + "*";
    dataset.putAndInsertOFStringArray(DCM_SeriesDescription, search_string.c_str());

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

    return this->send_find_request(dataset);
}

//------------------------------------------------------------------------------

std::string series_enquirer::find_sop_instance_uid(
    const std::string& _series_instance_uid,
    unsigned int _instance_number
)
{
    // Dataset used to store query informations.
    DcmDataset dataset;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, _series_instance_uid.c_str());
    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, "");
    std::stringstream ss;
    ss << _instance_number;
    dataset.putAndInsertOFStringArray(DCM_InstanceNumber, ss.str().c_str());

    OFList<QRResponse*> responses = this->send_find_request(dataset);
    OFListIterator(QRResponse*) it = responses.begin();
    std::string sop_instance_uid;
    if(it != responses.end() && ((*it)->m_dataset != nullptr))
    {
        OFString sop;
        (*it)->m_dataset->findAndGetOFStringArray(DCM_SOPInstanceUID, sop);
        sop_instance_uid = sop.c_str();
    }

    //Release responses.
    while(!responses.empty())
    {
        delete responses.front();
        responses.pop_front();
    }

    return sop_instance_uid;
}

//------------------------------------------------------------------------------

void series_enquirer::pull_series_using_move_retrieve_method(InstanceUIDContainer _instance_uid_container)
{
    // Reset instance count.
    m_instance_index = 0;

    DcmDataset dataset;
    OFCondition result;

    for(const std::string& series_instance_uid : _instance_uid_container)
    {
        dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");
        dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, series_instance_uid.c_str());

        // Fetches all images of this particular study.
        result = this->send_move_request(dataset);

        if(result.good())
        {
        }
        else
        {
            const std::string msg = "Unable to send a C-MOVE request to the server. "
                                    "(Series instance UID =" + std::string(series_instance_uid) + ") : "
                                    + std::string(result.text());
            throw io::dimse::exceptions::request_failure(msg);
        }
    }
}

//------------------------------------------------------------------------------

void series_enquirer::pull_series_using_get_retrieve_method(InstanceUIDContainer _instance_uid_container)
{
    // Reset instance count.
    m_instance_index = 0;

    DcmDataset dataset;
    OFCondition result;

    for(const std::string& series_instance_uid : _instance_uid_container)
    {
        dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "SERIES");
        dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, series_instance_uid.c_str());

        // Fetches all images of this particular study.
        result = this->send_get_request(dataset);

        if(result.good())
        {
        }
        else
        {
            const std::string msg = "Unable to send a C-GET request to the server. "
                                    "(Series instance UID =" + std::string(series_instance_uid) + ") : "
                                    + std::string(result.text());
            throw io::dimse::exceptions::request_failure(msg);
        }
    }
}

//------------------------------------------------------------------------------

void series_enquirer::pull_instance_using_move_retrieve_method(
    const std::string& _series_instance_uid,
    const std::string& _sop_instance_uid
)
{
    // Reset instance count.
    m_instance_index = 0;

    DcmDataset dataset;
    OFCondition result;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, _series_instance_uid.c_str());

    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, _sop_instance_uid.c_str());
    dataset.putAndInsertOFStringArray(DCM_InstanceNumber, "");

    // Fetches all images of this particular study.
    result = this->send_move_request(dataset);

    if(result.good())
    {
    }
    else
    {
        const std::string msg = "Unable to send a C-MOVE request to the server. "
                                "(Series instance UID =" + std::string(_series_instance_uid) + ") : "
                                + std::string(result.text());
        throw io::dimse::exceptions::request_failure(msg);
    }
}

//------------------------------------------------------------------------------

void series_enquirer::pull_instance_using_get_retrieve_method(
    const std::string& _series_instance_uid,
    const std::string& _sop_instance_uid
)
{
    // Reset instance count.
    m_instance_index = 0;

    DcmDataset dataset;
    OFCondition result;
    dataset.putAndInsertOFStringArray(DCM_QueryRetrieveLevel, "IMAGE");
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, _series_instance_uid.c_str());

    dataset.putAndInsertOFStringArray(DCM_SOPInstanceUID, _sop_instance_uid.c_str());
    dataset.putAndInsertOFStringArray(DCM_InstanceNumber, "");

    // Fetches all images of this particular study.
    result = this->send_get_request(dataset);

    if(result.good())
    {
    }
    else
    {
        const std::string msg = "Unable to send a C-GET request to the server. "
                                "(Series instance UID =" + std::string(_series_instance_uid) + ") : "
                                + std::string(result.text());
        throw io::dimse::exceptions::request_failure(msg);
    }
}

//------------------------------------------------------------------------------

void series_enquirer::push_series(const instance_path_container_t& _path_container)
{
    // Reset instance count.
    m_instance_index = 0;

    OFCondition result;

    // Send images to pacs.
    for(const std::filesystem::path& path : _path_container)
    {
        result = this->send_store_request(path);

        if(result.good())
        {
        }
        else
        {
            const std::string msg = "Unable to send a C-STORE request to the server : " + std::string(result.text());
            throw io::dimse::exceptions::request_failure(msg);
        }

        // Notify callback.
        if(m_progress_callback)
        {
            m_progress_callback->async_run("", ++m_instance_index, path.string());
        }
    }
}

//------------------------------------------------------------------------------

void series_enquirer::push_series(const dataset_container_t& _dataset_container)
{
    // Reset instance count.
    m_instance_index = 0;
    OFCondition result;

    // Send images to pacs.
    for(const auto& dataset : _dataset_container)
    {
        result = this->send_store_request(dataset);

        if(result.good())
        {
        }
        else
        {
            const std::string msg = "Unable to send a C-STORE request to the server : " + std::string(result.text());
            throw io::dimse::exceptions::request_failure(msg);
        }

        // Notify callback.
        if(m_progress_callback)
        {
            m_progress_callback->async_run("", ++m_instance_index, "");
        }
    }
}

//------------------------------------------------------------------------------

OFCondition series_enquirer::handleMOVEResponse(
    const T_ASC_PresentationContextID _pres_id,
    RetrieveResponse* _response,
    OFBool& _wait_for_next_response
)
{
    OFCondition result = DcmSCU::handleMOVEResponse(_pres_id, _response, _wait_for_next_response);

    // Check error status.
    const bool error = (_response->m_status != STATUS_Success) && (_response->m_status != STATUS_Pending);

    // Notify error.
    if(error)
    {
        const std::string msg = "Unable to perform a C-MOVE operation.";
        throw io::dimse::exceptions::request_failure(msg);
    }

    return result;
}

//------------------------------------------------------------------------------

OFCondition series_enquirer::handleSTORERequest(
    const T_ASC_PresentationContextID /*presID*/,
    DcmDataset* _incoming_object,
    OFBool& /*continueCGETSession*/,
    Uint16&
    /*cStoreReturnStatus*/
)
{
    OFCondition result;

    if(_incoming_object != nullptr)
    {
        // Find the series UID.
        OFString series_id;
        if(_incoming_object->findAndGetOFStringArray(DCM_SeriesInstanceUID, series_id).good())
        {
        }

        // Find the instance UID.
        OFString instance_id;
        if(_incoming_object->findAndGetOFStringArray(DCM_SOPInstanceUID, instance_id).good())
        {
        }

        // Create Folder.
        std::filesystem::path series_path = std::filesystem::path(m_path.string() + series_id.c_str() + "/");
        if(!std::filesystem::exists(series_path))
        {
            std::filesystem::create_directories(series_path);
        }

        // Save the file in the specified folder (Create new meta header for gdcm reader).
        std::string file_path = series_path.string() + instance_id.c_str();
        DcmFileFormat file_format(_incoming_object);
        file_format.saveFile(
            file_path.c_str(),
            EXS_Unknown,
            EET_UndefinedLength,
            EGL_recalcGL,
            EPD_noChange,
            0,
            0,
            EWM_createNewMeta
        );

        // Notify callback.
        if(m_progress_callback)
        {
            m_progress_callback->async_run(series_id.c_str(), ++m_instance_index, file_path);
        }
    }

    return result;
}

//------------------------------------------------------------------------------

Uint8 series_enquirer::find_uncompressed_pc(const OFString& _sop_class)
{
    Uint8 pc = 0;
    pc = this->findPresentationContextID(_sop_class, UID_LittleEndianExplicitTransferSyntax);
    if(pc == 0)
    {
        pc = this->findPresentationContextID(_sop_class, UID_BigEndianExplicitTransferSyntax);
    }

    if(pc == 0)
    {
        pc = this->findPresentationContextID(_sop_class, UID_LittleEndianImplicitTransferSyntax);
    }

    return pc;
}

} // namespace sight::io::dimse.
