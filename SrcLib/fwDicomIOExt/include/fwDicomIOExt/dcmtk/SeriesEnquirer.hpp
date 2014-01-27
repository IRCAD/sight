/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_SERIESENQUIRER_HPP__
#define __FWDICOMIOEXT_DCMTK_SERIESENQUIRER_HPP__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scu.h>

#include <fwCore/BaseObject.hpp>
#include <fwCore/macros.hpp>
#include <fwTools/macros.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{

/**
 * @class SeriesEnquirer
 * @brief Reads DICOM series from pacs.
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API SeriesEnquirer : public ::fwCore::BaseObject, protected DcmSCU
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (SeriesEnquirer)(::fwDicomIOExt::dcmtk::SeriesEnquirer), (()), new SeriesEnquirer);
    fwCoreAllowSharedFromThis();

    FWDICOMIOEXT_API static const ::fwCom::Slots::SlotKeyType s_PROGRESS_CALLBACK_SLOT;
    typedef ::fwCom::Slot<void(float, bool)> ProgressCallbackSlotType;

    FWDICOMIOEXT_API static const ::fwCom::Slots::SlotKeyType s_STORE_INSTANCE_CALLBACK_SLOT;
    typedef ::fwCom::Slot<void(const std::string&, unsigned int, const std::string&)> StoreInstanceCallbackSlotType;

    typedef std::vector< std::string > InstanceUIDContainer;

    /// Constructor
    FWDICOMIOEXT_API SeriesEnquirer();

    /// Destructor
    FWDICOMIOEXT_API ~SeriesEnquirer();

    /**
     * @brief Initialize the connection
     * @param[in] applicationTitle Local application title
     * @param[in] peerHostName Pacs host name
     * @param[in] peerPort Pacs port
     * @param[in] peerApplicationTitle Pacs application title
     * @param[in] path Local dicom folder path
     * @param[in] moveApplicationTitle Move application title
     * @param[in] storeCallback Store callback
     * @param[in] progressCallback Progress callback
     */
    FWDICOMIOEXT_API void initialize(const std::string& applicationTitle,
            const std::string& peerHostName, unsigned int peerPort,
            const std::string& peerApplicationTitle,
            const std::string& moveApplicationTitle = "",
            StoreInstanceCallbackSlotType::sptr storeCallback = StoreInstanceCallbackSlotType::sptr(),
            ProgressCallbackSlotType::sptr progressCallback = ProgressCallbackSlotType::sptr());

    /// Negotiate Association
    FWDICOMIOEXT_API bool connect();

    /// Release association
    FWDICOMIOEXT_API void disconnect();

    /// Assemble and send C-ECHO request
    FWDICOMIOEXT_API bool pingPacs();

    /// Return true if there is an existing association
    FWDICOMIOEXT_API bool isConnectedToPacs() const;

    /**
     * @brief Find series by patient name
     * @param[in] name Patient name
     * @return dcmtk response list
     */
    FWDICOMIOEXT_API OFList< QRResponse* > findSeriesByPatientName(const std::string& name);

    /**
     * @brief Find series by study date
     * @param[in] fromDate Beginning date
     * @param[in] toDate Ending date
     * @return dcmtk response list
     */
    FWDICOMIOEXT_API OFList< QRResponse* > findSeriesByDate(const std::string& fromDate, const std::string& toDate);

    /**
     * @brief Find SOPInstanceUID of the specified instance
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] instanceNumber Instance number
     * @return SOPInstanceUID corresponding to the instanceNumber
     */
    FWDICOMIOEXT_API std::string findSOPInstanceUID(const std::string& seriesInstanceUID, unsigned int instanceNumber);

    /**
     * @brief Pull series using C-MOVE requests
     * @param[in] instanceUIDContainer Series instance UID container
     */
    FWDICOMIOEXT_API void pullSeriesUsingMoveRetrieveMethod(InstanceUIDContainer instanceUIDContainer);

    /**
     * @brief Pull series using C-GET requests
     * @param[in] instanceUIDContainer Series instance UID container
     */
    FWDICOMIOEXT_API void pullSeriesUsingGetRetrieveMethod(InstanceUIDContainer instanceUIDContainer);

    /**
     * @brief Pull instance using C-MOVE requests
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] sopInstanceUID SOP Instance UID
     */
    FWDICOMIOEXT_API void pullInstanceUsingMoveRetrieveMethod(const std::string& seriesInstanceUID,
            const std::string& sopInstanceUID);

    /**
     * @brief Pull instance using C-GET requests
     * @param[in] seriesInstanceUID Series instance UID
     * @param[in] sopInstanceUID SOP Instance UID
     */
    FWDICOMIOEXT_API void pullInstanceUsingGetRetrieveMethod(const std::string& seriesInstanceUID,
            const std::string& sopInstanceUID);

protected :

    /**
     * @brief Send Find Request
     * @param[in] dataset Request dataset
     * @return dcmtk response list
     */
    FWDICOMIOEXT_API OFList< QRResponse* > sendFindRequest(DcmDataset dataset);

    /**
     * @brief Send Move Request
     * @param[in] dataset Request dataset
     * @return OFTrue on success
     */
    FWDICOMIOEXT_API OFCondition sendMoveRequest(DcmDataset dataset);

    /**
     * @brief Send Get Request
     * @param[in] dataset Request dataset
     * @return OFTrue on success
     */
    FWDICOMIOEXT_API OFCondition sendGetRequest(DcmDataset dataset);


    /// Handle MOVE Response (Override)
    FWDICOMIOEXT_API virtual OFCondition handleMOVEResponse(
        const T_ASC_PresentationContextID presID, RetrieveResponse *response, OFBool &waitForNextResponse);

    /// Handle STORE Request (Override)
    FWDICOMIOEXT_API virtual OFCondition handleSTORERequest (
            const T_ASC_PresentationContextID presID, DcmDataset *incomingObject,
            OFBool &continueCGETSession, Uint16 &cStoreReturnStatus);

    /**
     * @brief Find uncompressed presentation context
     * @param[in] sopClass SOP class which needs the presentation context
     */
    FWDICOMIOEXT_API Uint8 findUncompressedPC(const OFString& sopClass);

    /// MOVE destination AE Title
    std::string m_moveApplicationTitle;

    /// Move response callback
    ProgressCallbackSlotType::sptr m_progressCallback;

    /// Path where the files must be saved
    ::boost::filesystem::path m_path;

    /// Store request callback slot
    StoreInstanceCallbackSlotType::sptr m_storeInstanceCallback;

};

} //dcmtk
} // namespace fwDicomIOExt

#endif /*__FWDICOMIOEXT_DCMTK_SERIESENQUIRER_HPP__*/
