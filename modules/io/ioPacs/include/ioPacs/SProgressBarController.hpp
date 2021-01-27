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

#include "ioPacs/config.hpp"

#include <core/com/Slot.hpp>
#include <core/mt/types.hpp>

#include <gui/dialog/ProgressDialog.hpp>

#include <services/IController.hpp>

#include <filesystem>

namespace ioPacs
{

/**
 * @brief This editor service is used to display a progress bar.
 *
 * @section Slots Slots
 * - \b startProgress(std::string): starts the progress bar (bar id).
 * - \b updateProgress(std::string, float, string): updates the progress bar (bar id, percentage, message).
 * - \b stopProgress(std::string): stops the progress bar (bar id).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="progressBarController" type="::ioPacs::SProgressBarController" />
   @endcode
 */
class IOPACS_CLASS_API SProgressBarController : public services::IController
{

public:

    fwCoreServiceMacro(SProgressBarController,  services::IController)

    /// Initializes slots.
    IOPACS_API SProgressBarController() noexcept;

    /// Destroys the service.
    IOPACS_API virtual ~SProgressBarController() noexcept;

protected:

    /// Does nothing.
    IOPACS_API virtual void configuring() override;

    /// Does nothing.
    IOPACS_API virtual void starting() override;

    /// Does nothing.
    IOPACS_API virtual void stopping() override;

    /// Does nothing.
    IOPACS_API void updating() override;

private:

    /// Starts a progress bar with the given id.
    void startProgress(std::string _id);

    /// Starts a progress bar with the given id with a new message.
    void updateProgress(std::string _id, float _percentage, std::string _message);

    /// Stops a progress bar with the given id.
    void stopProgress(std::string _id);

    /// Stores progress bars.
    std::map< std::string, gui::dialog::ProgressDialog::sptr > m_progressDialogs;

    /// Synchronizes progress bars access.
    core::mt::Mutex m_mutex;

};

} // namespace ioPacs.
