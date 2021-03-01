/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/tools/ProgressAdviser.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scp.h>

#include <filesystem>

namespace sight::io::dimse
{

/**
 * @brief Reads DICOM series from pacs.
 */
class IO_DIMSE_CLASS_API SeriesRetriever : public core::BaseObject,
                                           public DcmSCP
{

public:
    SIGHT_DECLARE_CLASS(SeriesRetriever, io::dimse::SeriesRetriever, new SeriesRetriever)
    SIGHT_ALLOW_SHARED_FROM_THIS();

    IO_DIMSE_API static const core::com::Slots::SlotKeyType s_PROGRESS_CALLBACK_SLOT;
    typedef core::com::Slot<void (const std::string&, unsigned int, const std::string&)> ProgressCallbackSlotType;

    /// Constructor
    IO_DIMSE_API SeriesRetriever();

    /// Destructor
    IO_DIMSE_API ~SeriesRetriever();

    /**
     * @brief Initialize the connection
     * @param[in] applicationTitle Move application title
     * @param[in] applicationport Move application port
     * @param[in] path Local dicom folder path
     * @param[in] timeout Connection timeout
     * @param[in] progressCallback Progress callback
     */
    IO_DIMSE_API void initialize(const std::string& applicationTitle, unsigned short applicationport, int timeout = 3,
                                 ProgressCallbackSlotType::sptr progressCallback = ProgressCallbackSlotType::sptr());

    /// Start the server
    IO_DIMSE_API bool start();

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
    std::filesystem::path m_path;

    /// Progress callback slot
    ProgressCallbackSlotType::sptr m_progressCallback;

    /// Dowloaded instance index
    unsigned int m_instanceIndex;

};

} // namespace sight::io::dimse
