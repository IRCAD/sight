/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "modules/io/dimse/config.hpp"

#include <core/com/slot.hpp>
#include <core/mt/types.hpp>

#include <service/controller.hpp>

#include <ui/__/dialog/progress.hpp>

#include <filesystem>

namespace sight::module::io::dimse
{

/**
 * @brief This editor service is used to display a progress bar.
 *
 * @section Slots Slots
 * - \b start_progress(std::string): starts the progress bar (bar id).
 * - \b update_progress(std::string, float, string): updates the progress bar (bar id, percentage, message).
 * - \b stop_progress(std::string): stops the progress bar (bar id).
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="progressBarController" type="sight::module::io::dimse::progress_bar_controller" />
   @endcode
 */
class MODULE_IO_DIMSE_CLASS_API progress_bar_controller : public service::controller
{
public:

    SIGHT_DECLARE_SERVICE(progress_bar_controller, service::controller);

    /// Initializes slots.
    MODULE_IO_DIMSE_API progress_bar_controller() noexcept;

    /// Destroys the service.
    MODULE_IO_DIMSE_API ~progress_bar_controller() noexcept override;

protected:

    /// Does nothing.
    MODULE_IO_DIMSE_API void configuring() override;

    /// Does nothing.
    MODULE_IO_DIMSE_API void starting() override;

    /// Does nothing.
    MODULE_IO_DIMSE_API void stopping() override;

    /// Does nothing.
    MODULE_IO_DIMSE_API void updating() override;

private:

    /// Starts a progress bar with the given id.
    void start_progress(std::string _id);

    /// Starts a progress bar with the given id with a new message.
    void update_progress(std::string _id, float _percentage, std::string _message);

    /// Stops a progress bar with the given id.
    void stop_progress(std::string _id);

    /// Stores progress bars.
    std::map<std::string, sight::ui::dialog::progress::sptr> m_progress_dialogs;

    /// Synchronizes progress bars access.
    core::mt::mutex m_mutex;
};

} // namespace sight::module::io::dimse.
