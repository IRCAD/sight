/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "gui/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Slots.hpp>

#include <fwGui/editor/IDialogEditor.hpp>
#include <fwGui/IActionSrv.hpp>

#include <fwJobs/IJob.hpp>

#include <fwServices/IService.hpp>

#include <set>

namespace fwGui
{
namespace dialog
{
class ProgressDialog;
}
}

namespace gui
{
namespace editor
{

/**
 * @brief   Service displaying a progress bar.
 */

class GUI_CLASS_API SJobBar : public ::fwGui::editor::IDialogEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SJobBar)(::fwGui::editor::IDialogEditor) );

    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Type of showJob slot
    typedef ::fwCom::Slot< void ( ::fwJobs::IJob::sptr ) >  ShowJobSlot;

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SJobBar() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SJobBar() noexcept;

protected:

    /**
     * @brief This method gives information about the class. Do nothing.
     */
    GUI_API virtual void info(std::ostream& _sstream ) override;

    /**
     * @brief This method emit a signal.
     */
    GUI_API void updating() override;

    /**
     * @brief This method is used to configure the service.
     */
    GUI_API void configuring() override;

    GUI_API virtual void starting() override;

    GUI_API virtual void stopping() override;

    /**
     * @brief showJob slot's method
     */
    GUI_API virtual void showJob( ::fwJobs::IJob::sptr iJob );

    typedef std::set< SPTR(::fwGui::dialog::ProgressDialog) > ProgressDialogs;
    ProgressDialogs m_progressDialogs;

    typedef ::fwCom::Signal<void () > StartedSignalType;
    typedef ::fwCom::Signal<void () > EndedSignalType;

    SPTR(StartedSignalType) m_sigStarted;
    SPTR(EndedSignalType) m_sigEnded;

};

} // namespace editor
} // namespace gui
