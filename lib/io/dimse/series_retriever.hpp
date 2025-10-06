/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include <sight/io/dimse/config.hpp>

#include <core/com/slot.hpp>
#include <core/com/slots.hpp>
#include <core/progress/observer.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/scp.h>

#include <filesystem>

namespace sight::io::dimse
{

/**
 * @brief Reads DICOM series from pacs.
 */
class SIGHT_IO_DIMSE_CLASS_API series_retriever : public core::base_object,
                                                  public DcmSCP
{
public:

    SIGHT_DECLARE_CLASS(series_retriever, io::dimse::series_retriever);
    SIGHT_ALLOW_SHARED_FROM_THIS();

    SIGHT_IO_DIMSE_API static const core::com::slots::key_t PROGRESS_CALLBACK_SLOT;
    using progress_callback_slot_t = core::com::slot<void (const std::string&, unsigned int, const std::string&)>;

    /// Constructor
    SIGHT_IO_DIMSE_API series_retriever();

    /// Destructor
    SIGHT_IO_DIMSE_API ~series_retriever() override;

    /**
     * @brief Initialize the connection
     * @param[in] _application_title Move application title
     * @param[in] _applicationport Move application port
     * @param[in] _timeout Connection timeout
     * @param[in] _progress Progress monitor
     */
    SIGHT_IO_DIMSE_API void initialize(
        const std::string& _application_title,
        std::uint16_t _applicationport,
        int _timeout                             = 3,
        core::progress::observer::sptr _progress = core::progress::observer::sptr()
    );

    /// Start the server
    SIGHT_IO_DIMSE_API bool start();

protected:

    // workaround warning 'sight::io::dimse::SeriesRetriever::handleSTORERequest' hides overloaded virtual function
    using DcmSCP::handleSTORERequest;

    /// Handle Incoming Command (Override)
    OFCondition handleIncomingCommand(
        T_DIMSE_Message* _incoming_msg,
        const DcmPresentationContextInfo& _pres_context_info
    ) override;

    /**
     * @brief Handle C-STORE Request
     * @param[in] _incoming_msg Incoming DIMSE message
     * @param[in] _pres_id Presentation context ID
     * @return OFTrue on success
     */
    virtual OFCondition handleSTORERequest(T_DIMSE_Message* _incoming_msg, T_ASC_PresentationContextID _pres_id);

    /// Path where the files must be saved
    std::filesystem::path m_path;

    /// Progress callback slot
    core::progress::observer::sptr m_progress;

    /// Downloaded instance index
    unsigned int m_instance_index {};
};

} // namespace sight::io::dimse
