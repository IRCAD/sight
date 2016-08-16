/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPACSIO_SERIESRETRIEVER_HPP__
#define __FWPACSIO_SERIESRETRIEVER_HPP__

#include <dcmtk/config/osconfig.h>

#include "fwPacsIO/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCore/macros.hpp>
#include <fwTools/ProgressAdviser.hpp>
#include <fwTools/macros.hpp>

#include <boost/filesystem/path.hpp>

#include <dcmtk/dcmnet/scp.h>

namespace fwPacsIO
{

/**
 * @brief Reads DICOM series from pacs.
 */
class FWPACSIO_CLASS_API SeriesRetriever : public ::fwCore::BaseObject,
                                           public DcmSCP
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (SeriesRetriever)(::fwPacsIO::SeriesRetriever),
                                            (()), new SeriesRetriever);
    fwCoreAllowSharedFromThis();

    FWPACSIO_API static const ::fwCom::Slots::SlotKeyType s_PROGRESS_CALLBACK_SLOT;
    typedef ::fwCom::Slot<void (const std::string&, unsigned int, const std::string&)> ProgressCallbackSlotType;

    /// Constructor
    FWPACSIO_API SeriesRetriever();

    /// Destructor
    FWPACSIO_API ~SeriesRetriever();

    /**
     * @brief Initialize the connection
     * @param[in] applicationTitle Move application title
     * @param[in] applicationport Move application port
     * @param[in] path Local dicom folder path
     * @param[in] timeout Connection timeout
     * @param[in] progressCallback Progress callback
     */
    FWPACSIO_API void initialize(const std::string& applicationTitle, unsigned short applicationport, int timeout = 3,
                                 ProgressCallbackSlotType::sptr progressCallback = ProgressCallbackSlotType::sptr());

    /// Start the server
    FWPACSIO_API bool start();

protected:
    /// Handle Incoming Command (Override)
    virtual OFCondition handleIncomingCommand(T_DIMSE_Message* incomingMsg,
                                              const DcmPresentationContextInfo& presContextInfo);

    /**
     * @brief Handle C-STORE Request
     * @param[in] incomingMsg Incoming DIMSE message
     * @param[in] presID Presentation context ID
     * @return OFTrue on success
     */
    virtual OFCondition handleSTORERequest(T_DIMSE_Message* incomingMsg, T_ASC_PresentationContextID presID);

    /// Path where the files must be saved
    ::boost::filesystem::path m_path;

    /// Progress callback slot
    ProgressCallbackSlotType::sptr m_progressCallback;

    /// Dowloaded instance index
    unsigned int m_instanceIndex;

};

} // namespace fwPacsIO

#endif /*__FWPACSIO_SERIESRETRIEVER_HPP__*/
