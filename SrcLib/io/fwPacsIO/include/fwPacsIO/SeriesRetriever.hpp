/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWPACSIO_SERIESRETRIEVER_HPP__
#define __FWPACSIO_SERIESRETRIEVER_HPP__

#include "fwPacsIO/config.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwTools/ProgressAdviser.hpp>

#include <boost/filesystem/path.hpp>

#include <dcmtk/config/osconfig.h>
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
                                              const DcmPresentationContextInfo& presContextInfo) override;

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
