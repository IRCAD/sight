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

#pragma once

#include "io/dimse/config.hpp"

#include <core/BaseObject.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/memory/BufferObject.hpp>

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
        data::SeriesDB::ContainerType series = io::dimse::helper::Series::toFwMedData(responses);
        io::dimse::helper::Series::releaseResponses(responses);
    }
    catch(io::dimse::exceptions::Base& _e)
    {
        SLM_ERROR("Can't execute query: " + std::string(_e.what()));
    }
    seriesEnquirer->disconnect();
   @endcode
 */
class IO_DIMSE_CLASS_API SeriesEnquirer :
    public core::BaseObject,
    protected DcmSCU
{

public:

    fwCoreClassMacro(SeriesEnquirer, io::dimse::SeriesEnquirer, new SeriesEnquirer)
    fwCoreAllowSharedFromThis();

    IO_DIMSE_API static const core::com::Slots::SlotKeyType s_PROGRESS_CALLBACK_SLOT;

    typedef core::com::Slot<void (const std::string&, unsigned int, const std::string&)> ProgressCallbackSlotType;

    typedef std::vector< std::string > InstanceUIDContainer;

    typedef std::vector< std::filesystem::path > InstancePathContainer;

    typedef std::vector< CSPTR(DcmDataset) > DatasetContainer;

    /// Initializes memnbers.
    IO_DIMSE_API SeriesEnquirer();

    /// Destroyes the instance.
    IO_DIMSE_API ~SeriesEnquirer() override;

    /**
     * @brief Initializes the connection.
     * @param _applicationTitle The local application title.
     * @param _peerHostName The pacs host name.
     * @param _peerPort The pacs port.
     * @param _peerApplicationTitle The pacs application title.
     * @param _path The local dicom folder path.
     * @param _moveApplicationTitle The move application title.
     * @param _progressCallback The progress callback.
     */
    IO_DIMSE_API void initialize(const std::string& _applicationTitle,
                                 const std::string& _peerHostName,
                                 unsigned short _peerPort,
                                 const std::string& _peerApplicationTitle,
                                 const std::string& _moveApplicationTitle         = "",
                                 ProgressCallbackSlotType::sptr _progressCallback = ProgressCallbackSlotType::sptr());

    /// Initializes the network and negotiates association.
    IO_DIMSE_API bool connect();

    /// Return true if there is an existing association
    IO_DIMSE_API bool isConnectedToPacs() const;

    /// Assembles and send C-ECHO request.
    IO_DIMSE_API bool pingPacs();

    /// Releases association.
    IO_DIMSE_API void disconnect();

    /**
     * @brief Finds series by patient name.
     * @param _name The patient name.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList< QRResponse* > findSeriesByPatientName(const std::string& _name);

    /**
     * @brief Finds series by patient UID.
     * @param _uid The patient UID.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList< QRResponse* > findSeriesByPatientUID(const std::string& _uid);

    /**
     * @brief Finds series by birthdate.
     * @param _birthDate birthdate of the series.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList< QRResponse* > findSeriesByPatientBirthDate(const std::string& _birthDate);

    /**
     * @brief Finds series by study date.
     * @param _fromDate The beginning date.
     * @param _toDate The ending date.
     * @return The dcmtk response list.
     *
     * @pre _fromDate and _toDate must match the format YYYYMMDD.
     */
    IO_DIMSE_API OFList< QRResponse* > findSeriesByDate(const std::string& _fromDate, const std::string& _toDate);

    /**
     * @brief Finds series by series UID.
     * @param _uid The series UID.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList< QRResponse* > findSeriesByUID(const std::string& _uid);

    /**
     * @brief Finds series by modality.
     * @param _modality The modality.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList< QRResponse* > findSeriesByModality(const std::string& _modality);

    /**
     * @brief Finds series by description.
     * @param _description The desription.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList< QRResponse* > findSeriesByDescription(const std::string& _description);

    /**
     * @brief Finds SOPInstanceUID of the specified instance.
     * @param _seriesInstanceUID The series instance UID.
     * @param _instanceNumber The instance number.
     * @return The SOPInstanceUID corresponding to the instanceNumber.
     */
    IO_DIMSE_API std::string findSOPInstanceUID(const std::string& _seriesInstanceUID, unsigned int _instanceNumber);

    /**
     * @brief Pulls series using C-MOVE requests.
     * @param _instanceUIDContainer The series instance UID container.
     */
    IO_DIMSE_API void pullSeriesUsingMoveRetrieveMethod(InstanceUIDContainer _instanceUIDContainer);

    /**
     * @brief Pulls series using C-GET requests.
     * @param _instanceUIDContainer The series instance UID container.
     */
    IO_DIMSE_API void pullSeriesUsingGetRetrieveMethod(InstanceUIDContainer _instanceUIDContainer);

    /**
     * @brief Pulls instance using C-MOVE requests.
     * @param _seriesInstanceUID The series instance UID.
     * @param _sopInstanceUID The sOP Instance UID.
     */
    IO_DIMSE_API void pullInstanceUsingMoveRetrieveMethod(const std::string& _seriesInstanceUID,
                                                          const std::string& _sopInstanceUID);

    /**
     * @brief Pulls instance using C-GET requests.
     * @param _seriesInstanceUID The series instance UID.
     * @param _sopInstanceUID The sOP Instance UID.
     */
    IO_DIMSE_API void pullInstanceUsingGetRetrieveMethod(const std::string& _seriesInstanceUID,
                                                         const std::string& _sopInstanceUID);

    /**
     * @brief Pushs instances using C-STORE requests.
     * @param _pathContainer The instance paths container.
     */
    IO_DIMSE_API void pushSeries(const InstancePathContainer& _pathContainer);

    /**
     * @brief Pushs instances using C-STORE requests.
     * @param _DatasetContainer The dICOM dataset container.
     */
    IO_DIMSE_API void pushSeries(const DatasetContainer& _datasetContainer);

protected:

    /**
     * @brief Sends a C-FIND Request.
     * @param _dataset The request dataset.
     * @return The dcmtk response list.
     */
    IO_DIMSE_API OFList< QRResponse* > sendFindRequest(DcmDataset _dataset);

    /**
     * @brief Sends a C-MOVE Request.
     * @param _dataset The request dataset.
     * @return OFTrue on success.
     */
    IO_DIMSE_API OFCondition sendMoveRequest(DcmDataset _dataset);

    /**
     * @brief Sends a C-GET Request.
     * @param _dataset The request dataset.
     * @return OFTrue on success.
     */
    IO_DIMSE_API OFCondition sendGetRequest(DcmDataset _dataset);

    /**
     * @brief Sends a C-STORE Request.
     * @param _path The file path.
     * @return OFTrue on success.
     */
    IO_DIMSE_API OFCondition sendStoreRequest(const std::filesystem::path& _path);

    /**
     * @brief Sends a C-STORE Request.
     * @param _dataset The dicom dataset.
     * @return OFTrue on success.
     */
    IO_DIMSE_API OFCondition sendStoreRequest(const CSPTR(DcmDataset)& _dataset);

    /**
     * @brief Handles a C-MOVE response.
     * @param _presID
     * @param _response
     * @param _waitForNextResponse
     * @return
     */
    IO_DIMSE_API OFCondition handleMOVEResponse(const T_ASC_PresentationContextID _presID,
                                                RetrieveResponse* _response,
                                                OFBool& _waitForNextResponse) override;

    /**
     * @brief Handles a C-STORE response.
     * @param _presID
     * @param _incomingObject
     * @param _continueCGETSession
     * @param _cStoreReturnStatus
     * @return
     */
    IO_DIMSE_API OFCondition handleSTORERequest(const T_ASC_PresentationContextID,
                                                DcmDataset* _incomingObject,
                                                OFBool&,
                                                Uint16&) override;

    /**
     * @brief Finds uncompressed presentation context.
     * @param _sopClass The sOP class which needs the presentation context.
     */
    IO_DIMSE_API Uint8 findUncompressedPC(const OFString& _sopClass);

private:

    /// Defines the MOVE destination AE Title.
    std::string m_moveApplicationTitle;

    /// Defines the path where the files must be saved.
    std::filesystem::path m_path;

    /// Contains the progress callback slot.
    ProgressCallbackSlotType::sptr m_progressCallback;

    /// Sets the dowloaded instance index.
    unsigned int m_instanceIndex { 0 };

};

} // namespace sight::io::dimse.
