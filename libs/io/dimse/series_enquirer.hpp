/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include "io/dimse/config.hpp"

#include <core/base_object.hpp>
#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/memory/buffer_object.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scu.h>

#include <filesystem>

namespace sight::io::dimse
{

/**
 * @brief Reads DICOM series from PACS.
 *
 * Example:
 * @code{.cpp}
    io::dimse::SeriesEnquirer::sptr seriesEnquirer= io::dimse::SeriesEnquirer::New();
    try
    {
        seriesEnquirer->initialize(
            "AET,
            "192.168.0.1",
            "104",
            "ORTHANC";
        seriesEnquirer->connect();
        OFList< QRResponse* > responses = seriesEnquire->findSeriesByModality("CT");
        data::series_set::container_t series = io::dimse::helper::Series::toFwMedData(responses);
        io::dimse::helper::Series::releaseResponses(responses);
    }
    catch(io::dimse::exceptions::Base& _e)
    {
        SIGHT_ERROR("Can't execute query: " + std::string(_e.what()));
    }
    seriesEnquirer->disconnect();
   @endcode
 */
class IO_DIMSE_CLASS_API series_enquirer :
    public core::base_object,
    protected DcmSCU
{
public:

    SIGHT_DECLARE_CLASS(series_enquirer, io::dimse::series_enquirer, std::make_shared<series_enquirer>);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    IO_DIMSE_API static const core::com::slots::key_t PROGRESS_CALLBACK_SLOT;

    using progress_callback_slot_t = core::com::slot<void (const std::string&, unsigned int, const std::string&)>;

    using InstanceUIDContainer = std::vector<std::string>;

    using instance_path_container_t = std::vector<std::filesystem::path>;

    using dataset_container_t = std::vector<std::shared_ptr<const DcmDataset> >;

    /// Initializes members.
    IO_DIMSE_API series_enquirer();

    /// Destroyes the instance.
    IO_DIMSE_API ~series_enquirer() override;

    /**
     * @brief Initializes the connection.
     * @param _application_title The local application title.
     * @param _peer_host_name The pacs host name.
     * @param _peer_port The pacs port.
     * @param _peer_application_title The pacs application title.
     * @param _move_application_title The move application title.
     * @param _progress_callback The progress callback.
     */
    IO_DIMSE_API void initialize(
        const std::string& _application_title,
        const std::string& _peer_host_name,
        std::uint16_t _peer_port,
        const std::string& _peer_application_title,
        const std::string& _move_application_title        = "",
        progress_callback_slot_t::sptr _progress_callback = progress_callback_slot_t::sptr()
    );

    /// Initializes the network and negotiates association.
    IO_DIMSE_API bool connect();

    /// Return true if there is an existing association
    IO_DIMSE_API bool is_connected_to_pacs() const;

    /// Assembles and send C-ECHO request.
    IO_DIMSE_API bool ping_pacs();

    /// Releases association.
    IO_DIMSE_API void disconnect();

    /**
     * @brief Finds series by patient name.
     * @param _name The patient name.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList<QRResponse*> find_series_by_patient_name(const std::string& _name);

    /**
     * @brief Finds series by patient UID.
     * @param _uid The patient UID.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList<QRResponse*> find_series_by_patient_uid(const std::string& _uid);

    /**
     * @brief Finds series by birthdate.
     * @param _birth_date birthdate of the series.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList<QRResponse*> find_series_by_patient_birth_date(const std::string& _birth_date);

    /**
     * @brief Finds series by study date.
     * @param _from_date The beginning date.
     * @param _to_date The ending date.
     * @return The dcmtk response list.
     *
     * @pre _fromDate and _toDate must match the format YYYYMMDD.
     */
    IO_DIMSE_API OFList<QRResponse*> find_series_by_date(const std::string& _from_date, const std::string& _to_date);

    /**
     * @brief Finds series by series UID.
     * @param _uid The series UID.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList<QRResponse*> find_series_by_uid(const std::string& _uid);

    /**
     * @brief Finds series by modality.
     * @param _modality The modality.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList<QRResponse*> find_series_by_modality(const std::string& _modality);

    /**
     * @brief Finds series by description.
     * @param _description The description.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList<QRResponse*> find_series_by_description(const std::string& _description);

    /**
     * @brief Finds SOPInstanceUID of the specified instance.
     * @param _series_instance_uid The series instance UID.
     * @param _instance_number The instance number.
     * @return The SOPInstanceUID corresponding to the instanceNumber.
     */
    IO_DIMSE_API std::string find_sop_instance_uid(
        const std::string& _series_instance_uid,
        unsigned int _instance_number
    );

    /**
     * @brief Pulls series using C-MOVE requests.
     * @param _instance_uid_container The series instance UID container.
     */
    IO_DIMSE_API void pull_series_using_move_retrieve_method(InstanceUIDContainer _instance_uid_container);

    /**
     * @brief Pulls series using C-GET requests.
     * @param _instance_uid_container The series instance UID container.
     */
    IO_DIMSE_API void pull_series_using_get_retrieve_method(InstanceUIDContainer _instance_uid_container);

    /**
     * @brief Pulls instance using C-MOVE requests.
     * @param _series_instance_uid The series instance UID.
     * @param _sop_instance_uid The sOP Instance UID.
     */
    IO_DIMSE_API void pull_instance_using_move_retrieve_method(
        const std::string& _series_instance_uid,
        const std::string& _sop_instance_uid
    );

    /**
     * @brief Pulls instance using C-GET requests.
     * @param _series_instance_uid The series instance UID.
     * @param _sop_instance_uid The sOP Instance UID.
     */
    IO_DIMSE_API void pull_instance_using_get_retrieve_method(
        const std::string& _series_instance_uid,
        const std::string& _sop_instance_uid
    );

    /**
     * @brief Pushs instances using C-STORE requests.
     * @param _path_container The instance paths container.
     */
    IO_DIMSE_API void push_series(const instance_path_container_t& _path_container);

    /**
     * @brief Pushs instances using C-STORE requests.
     * @param _dataset_container The dICOM dataset container.
     */
    IO_DIMSE_API void push_series(const dataset_container_t& _dataset_container);

protected:

    /**
     * @brief Sends a C-FIND Request.
     * @param _dataset The request dataset.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList<QRResponse*> send_find_request(DcmDataset _dataset);

    /**
     * @brief Sends a C-MOVE Request.
     * @param _dataset The request dataset.
     * @return OFTrue on success.
     */
    IO_DIMSE_API OFCondition send_move_request(DcmDataset _dataset);

    /**
     * @brief Sends a C-GET Request.
     * @param _dataset The request dataset.
     * @return OFTrue on success.
     */
    IO_DIMSE_API OFCondition send_get_request(DcmDataset _dataset);

    /**
     * @brief Sends a C-STORE Request.
     * @param _path The file path.
     * @return OFTrue on success.
     */
    IO_DIMSE_API OFCondition send_store_request(const std::filesystem::path& _path);

    /**
     * @brief Sends a C-STORE Request.
     * @param _dataset The dicom dataset.
     * @return OFTrue on success.
     */
    IO_DIMSE_API OFCondition send_store_request(const CSPTR(DcmDataset)& _dataset);

    /**
     * @brief Handles a C-MOVE response.
     * @param _pres_id
     * @param _response
     * @param _wait_for_next_response
     * @return
     */
    IO_DIMSE_API OFCondition handleMOVEResponse(
        T_ASC_PresentationContextID _pres_id,
        RetrieveResponse* _response,
        OFBool& _wait_for_next_response
    ) override;

    /**
     * @brief Handles a C-STORE response.
     * @param _incoming_object
     * @return
     */
    IO_DIMSE_API OFCondition handleSTORERequest(
        T_ASC_PresentationContextID /*presID*/,
        DcmDataset* _incoming_object,
        OFBool& /*continueCGETSession*/,
        Uint16&
        /*cStoreReturnStatus*/
    ) override;

    /**
     * @brief Finds uncompressed presentation context.
     * @param _sop_class The sOP class which needs the presentation context.
     */
    IO_DIMSE_API Uint8 find_uncompressed_pc(const OFString& _sop_class);

private:

    /// Defines the MOVE destination AE Title.
    std::string m_move_application_title;

    /// Defines the path where the files must be saved.
    std::filesystem::path m_path;

    /// Contains the progress callback slot.
    progress_callback_slot_t::sptr m_progress_callback;

    /// Sets the dowloaded instance index.
    unsigned int m_instance_index {0};
};

} // namespace sight::io::dimse.
