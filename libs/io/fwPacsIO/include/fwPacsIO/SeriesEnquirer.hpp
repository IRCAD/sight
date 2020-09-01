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

#pragma once

#include "fwPacsIO/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwCore/BaseObject.hpp>

#include <fwMemory/BufferObject.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scu.h>

#include <filesystem>

namespace fwPacsIO
{

/**
 * @brief Reads DICOM series from PACS.
 *
 * Example:
 * @code{.cpp}
    ::fwPacsIO::SeriesEnquirer::sptr seriesEnquirer= ::fwPacsIO::SeriesEnquirer::New();
    try
    {
        seriesEnquirer->initialize(
            "AET,
            "192.168.0.1",
            "104",
            "ORTHANC";
        seriesEnquirer->connect();
        OFList< QRResponse* > responses = seriesEnquire->findSeriesByModality("CT");
        ::fwMedData::SeriesDB::ContainerType series = ::fwPacsIO::helper::Series::toFwMedData(responses);
        ::fwPacsIO::helper::Series::releaseResponses(responses);
    }
    catch(::fwPacsIO::exceptions::Base& _e)
    {
        SLM_ERROR("Can't execute query: " + std::string(_e.what()));
    }
    seriesEnquirer->disconnect();
   @endcode
 */
class FWPACSIO_CLASS_API SeriesEnquirer :
    public ::fwCore::BaseObject,
    protected DcmSCU
{

public:

    fwCoreClassMacro(SeriesEnquirer, ::fwPacsIO::SeriesEnquirer, new SeriesEnquirer)
    fwCoreAllowSharedFromThis();

    FWPACSIO_API static const ::fwCom::Slots::SlotKeyType s_PROGRESS_CALLBACK_SLOT;

    typedef ::fwCom::Slot<void (const std::string&, unsigned int, const std::string&)> ProgressCallbackSlotType;

    typedef std::vector< std::string > InstanceUIDContainer;

    typedef std::vector< std::filesystem::path > InstancePathContainer;

    typedef std::vector< CSPTR(DcmDataset) > DatasetContainer;

    /// Initializes memnbers.
    FWPACSIO_API SeriesEnquirer();

    /// Destroyes the instance.
    FWPACSIO_API virtual ~SeriesEnquirer();

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
    FWPACSIO_API void initialize(const std::string& _applicationTitle,
                                 const std::string& _peerHostName,
                                 unsigned short _peerPort,
                                 const std::string& _peerApplicationTitle,
                                 const std::string& _moveApplicationTitle         = "",
                                 ProgressCallbackSlotType::sptr _progressCallback = ProgressCallbackSlotType::sptr());

    /// Initializes the network and negotiates association.
    FWPACSIO_API bool connect();

    /// Return true if there is an existing association
    FWPACSIO_API bool isConnectedToPacs() const;

    /// Assembles and send C-ECHO request.
    FWPACSIO_API bool pingPacs();

    /// Releases association.
    FWPACSIO_API void disconnect();

    /**
     * @brief Finds series by patient name.
     * @param _name The patient name.
     * @return The dcmtk response list.
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByPatientName(const std::string& _name);

    /**
     * @brief Finds series by patient UID.
     * @param _uid The patient UID.
     * @return The dcmtk response list.
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByPatientUID(const std::string& _uid);

    /**
     * @brief Finds series by study date.
     * @param _fromDate The beginning date.
     * @param _toDate The ending date.
     * @return The dcmtk response list.
     *
     * @pre _fromDate and _toDate must match the format YYYYMMDD.
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByDate(const std::string& _fromDate, const std::string& _toDate);

    /**
     * @brief Finds series by series UID.
     * @param _uid The series UID.
     * @return The dcmtk response list.
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByUID(const std::string& _uid);

    /**
     * @brief Finds series by modality.
     * @param _modality The modality.
     * @return The dcmtk response list.
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByModality(const std::string& _modality);

    /**
     * @brief Finds series by description.
     * @param _description The desription.
     * @return The dcmtk response list.
     */
    FWPACSIO_API OFList< QRResponse* > findSeriesByDescription(const std::string& _description);

    /**
     * @brief Finds SOPInstanceUID of the specified instance.
     * @param _seriesInstanceUID The series instance UID.
     * @param _instanceNumber The instance number.
     * @return The SOPInstanceUID corresponding to the instanceNumber.
     */
    FWPACSIO_API std::string findSOPInstanceUID(const std::string& _seriesInstanceUID, unsigned int _instanceNumber);

    /**
     * @brief Pulls series using C-MOVE requests.
     * @param _instanceUIDContainer The series instance UID container.
     */
    FWPACSIO_API void pullSeriesUsingMoveRetrieveMethod(InstanceUIDContainer _instanceUIDContainer);

    /**
     * @brief Pulls series using C-GET requests.
     * @param _instanceUIDContainer The series instance UID container.
     */
    FWPACSIO_API void pullSeriesUsingGetRetrieveMethod(InstanceUIDContainer _instanceUIDContainer);

    /**
     * @brief Pulls instance using C-MOVE requests.
     * @param _seriesInstanceUID The series instance UID.
     * @param _sopInstanceUID The sOP Instance UID.
     */
    FWPACSIO_API void pullInstanceUsingMoveRetrieveMethod(const std::string& _seriesInstanceUID,
                                                          const std::string& _sopInstanceUID);

    /**
     * @brief Pulls instance using C-GET requests.
     * @param _seriesInstanceUID The series instance UID.
     * @param _sopInstanceUID The sOP Instance UID.
     */
    FWPACSIO_API void pullInstanceUsingGetRetrieveMethod(const std::string& _seriesInstanceUID,
                                                         const std::string& _sopInstanceUID);

    /**
     * @brief Pushs instances using C-STORE requests.
     * @param _pathContainer The instance paths container.
     */
    FWPACSIO_API void pushSeries(const InstancePathContainer& _pathContainer);

    /**
     * @brief Pushs instances using C-STORE requests.
     * @param _DatasetContainer The dICOM dataset container.
     */
    FWPACSIO_API void pushSeries(const DatasetContainer& _datasetContainer);

protected:

    /**
     * @brief Sends a C-FIND Request.
     * @param _dataset The request dataset.
     * @return The dcmtk response list.
     */
    FWPACSIO_API OFList< QRResponse* > sendFindRequest(DcmDataset _dataset);

    /**
     * @brief Sends a C-MOVE Request.
     * @param _dataset The request dataset.
     * @return OFTrue on success.
     */
    FWPACSIO_API OFCondition sendMoveRequest(DcmDataset _dataset);

    /**
     * @brief Sends a C-GET Request.
     * @param _dataset The request dataset.
     * @return OFTrue on success.
     */
    FWPACSIO_API OFCondition sendGetRequest(DcmDataset _dataset);

    /**
     * @brief Sends a C-STORE Request.
     * @param _path The file path.
     * @return OFTrue on success.
     */
    FWPACSIO_API OFCondition sendStoreRequest(const std::filesystem::path& _path);

    /**
     * @brief Sends a C-STORE Request.
     * @param _dataset The dicom dataset.
     * @return OFTrue on success.
     */
    FWPACSIO_API OFCondition sendStoreRequest(const CSPTR(DcmDataset)& _dataset);

    /**
     * @brief Handles a C-MOVE response.
     * @param _presID
     * @param _response
     * @param _waitForNextResponse
     * @return
     */
    FWPACSIO_API virtual OFCondition handleMOVEResponse(const T_ASC_PresentationContextID _presID,
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
    FWPACSIO_API virtual OFCondition handleSTORERequest(const T_ASC_PresentationContextID,
                                                        DcmDataset* _incomingObject,
                                                        OFBool&,
                                                        Uint16&) override;

    /**
     * @brief Finds uncompressed presentation context.
     * @param _sopClass The sOP class which needs the presentation context.
     */
    FWPACSIO_API Uint8 findUncompressedPC(const OFString& _sopClass);

    /// Defines the MOVE destination AE Title.
    std::string m_moveApplicationTitle;

    /// Defines the path where the files must be saved.
    std::filesystem::path m_path;

    /// Contains the progress callback slot.
    ProgressCallbackSlotType::sptr m_progressCallback;

    /// Sets the dowloaded instance index.
    unsigned int m_instanceIndex {0};
};

} // namespace fwPacsIO
