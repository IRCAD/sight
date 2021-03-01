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

#include "modules/ui/base/config.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Slots.hpp>
#include <core/jobs/IJob.hpp>

#include <service/IService.hpp>

#include <ui/base/IAction.hpp>
#include <ui/base/IDialogEditor.hpp>

#include <set>

namespace sight::ui::base
{
namespace dialog
{
class ProgressDialog;
}
}

namespace sight::module::ui::base
{

/**
 * @brief   Service displaying a progress bar.
 */

class MODULE_UI_BASE_CLASS_API SJobBar : public ::sight::ui::base::IDialogEditor
{

public:

    SIGHT_DECLARE_SERVICE(SJobBar, ::sight::ui::base::IDialogEditor)

    typedef core::runtime::ConfigurationElement::sptr ConfigurationType;

    /// Type of showJob slot
    typedef core::com::Slot< void ( core::jobs::IJob::sptr ) >  ShowJobSlot;

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_BASE_API SJobBar() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_BASE_API virtual ~SJobBar() noexcept;

protected:

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    MODULE_UI_BASE_API virtual void info(std::ostream& _sstream ) override;

    /**
     * @brief This method emit a signal.
     */
    MODULE_UI_BASE_API void updating() override;

    /**
     * @brief This method is used to configure the service.
     */
    MODULE_UI_BASE_API void configuring() override;

    MODULE_UI_BASE_API virtual void starting() override;

    MODULE_UI_BASE_API virtual void stopping() override;

    /**
     * @brief showJob slot's method
     */
    MODULE_UI_BASE_API virtual void showJob( core::jobs::IJob::sptr iJob );

    typedef std::set< SPTR(::sight::ui::base::dialog::ProgressDialog) > ProgressDialogs;
    ProgressDialogs m_progressDialogs;

    typedef core::com::Signal<void () > StartedSignalType;
    typedef core::com::Signal<void () > EndedSignalType;

    SPTR(StartedSignalType) m_sigStarted;
    SPTR(EndedSignalType) m_sigEnded;

};

} // namespace sight::module::ui::base
