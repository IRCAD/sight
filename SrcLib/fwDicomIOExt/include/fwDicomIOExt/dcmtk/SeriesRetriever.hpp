/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DCMTK_SERIESRETRIEVER_HPP__
#define __FWDICOMIOEXT_DCMTK_SERIESRETRIEVER_HPP__

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scp.h>
#include <boost/filesystem/path.hpp>

#include <fwCore/macros.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwTools/macros.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace dcmtk
{

/**
 * @class SeriesRetriever
 * @brief Reads DICOM series from pacs.
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API SeriesRetriever : public ::fwCore::BaseObject,
                                               public DcmSCP
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (SeriesRetriever)(::fwDicomIOExt::dcmtk::SeriesRetriever),
                                            (()), new SeriesRetriever);
    fwCoreAllowSharedFromThis();

    FWDICOMIOEXT_API static const ::fwCom::Slots::SlotKeyType s_STORE_INSTANCE_CALLBACK_SLOT;
    typedef ::fwCom::Slot<void (const std::string&, unsigned int, const std::string&)> StoreInstanceCallbackSlotType;

    /// Constructor
    FWDICOMIOEXT_API SeriesRetriever();

    /// Destructor
    FWDICOMIOEXT_API ~SeriesRetriever();

    /**
     * @brief Initialize the connection
     * @param[in] applicationTitle Move application title
     * @param[in] applicationport Move application port
     * @param[in] path Local dicom folder path
     * @param[in] timeout Connection timeout
     * @param[in] storeCallback Store callback
     */
    FWDICOMIOEXT_API void initialize(const std::string& applicationTitle, unsigned int applicationport, int timeout = 3,
                                     StoreInstanceCallbackSlotType::sptr storeCallback =
                                         StoreInstanceCallbackSlotType::sptr());

    /// Start the server
    FWDICOMIOEXT_API bool start();

protected:
    /// Handle Incoming Command (Override)
    virtual OFCondition handleIncomingCommand(T_DIMSE_Message *incomingMsg,
                                              const DcmPresentationContextInfo &presContextInfo);

    /**
     * @brief Handle C-STORE Request
     * @param[in] incomingMsg Incoming DIMSE message
     * @param[in] presID Presentation context ID
     * @return OFTrue on success
     */
    virtual OFCondition handleSTORERequest(T_DIMSE_Message *incomingMsg,
                                           T_ASC_PresentationContextID presID);

    /// Path where the files must be saved
    ::boost::filesystem::path m_path;

    /// Store request callback slot
    StoreInstanceCallbackSlotType::sptr m_storeInstanceCallback;

};

} //dcmtk
} // namespace fwDicomIOExt

#endif /*__FWDICOMIOEXT_DCMTK_SERIESRETRIEVER_HPP__*/
