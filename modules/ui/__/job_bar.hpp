/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "modules/ui/__/config.hpp"

#include <core/com/signal.hpp>
#include <core/com/slots.hpp>
#include <core/jobs/base.hpp>

#include <service/base.hpp>

#include <ui/__/action.hpp>
#include <ui/__/dialog_editor.hpp>

#include <set>

namespace sight::ui::dialog
{

class progress;

} // namespace sight::ui::dialog

namespace sight::module::ui
{

/**
 * @brief   Service displaying a progress bar.
 */

class MODULE_UI_CLASS_API job_bar : public sight::ui::dialog_editor
{
public:

    SIGHT_DECLARE_SERVICE(job_bar, sight::ui::dialog_editor);

    /// Type of showJob slot
    using show_job_slot = core::com::slot<void (core::jobs::base::sptr)>;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_API job_bar() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_API ~job_bar() noexcept override = default;

protected:

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    MODULE_UI_API void info(std::ostream& _sstream) override;

    /**
     * @brief This method emit a signal.
     */
    MODULE_UI_API void updating() override;

    /**
     * @brief This method is used to configure the service.
     */
    MODULE_UI_API void configuring() override;

    MODULE_UI_API void starting() override;

    MODULE_UI_API void stopping() override;

    /**
     * @brief showJob slot's method
     */
    MODULE_UI_API virtual void show_job(core::jobs::base::sptr _job);

    using progress_dialogs = std::set<std::shared_ptr<sight::ui::dialog::progress> >;
    progress_dialogs m_progress_dialogs;

    using started_signal_t = core::com::signal<void ()>;
    using ended_signal_t   = core::com::signal<void ()>;

    SPTR(started_signal_t) m_sig_started;
    SPTR(ended_signal_t) m_sig_ended;
};

} // namespace sight::module::ui
