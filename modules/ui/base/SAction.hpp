/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <ui/base/IAction.hpp>

namespace sight::module::ui::base
{

/**
 * @brief  Default action for a simple toolbar button or menu item.*
 * Common signals and slots options can be found in the interface @see IAction.
 *
 * @section Signals Signals
 * - \b clicked(): Emitted when the action is clicked.
 *
 * Example of configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::base::SAction" >
        <state checked="false" enabled="false" inverse="true" visible="true" />
        <sync>true</sync>
        <confirmation message="..." />
    </service>
   @endcode
 *
 * All configurations options are optional. Common action options can be found in @see IAction.
 * - \b sync: set to true to emit the 'clicked' signals synchronously instead of the default, asynchronously.
 */
class MODULE_UI_BASE_CLASS_API SAction : public sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SAction, sight::ui::base::IAction);

    MODULE_UI_BASE_API SAction() noexcept;
    MODULE_UI_BASE_API ~SAction() noexcept override = default;

private:

    MODULE_UI_BASE_API void configuring() override;

    MODULE_UI_BASE_API void starting() override;

    MODULE_UI_BASE_API void stopping() override;

    /// Emits the clicked signal
    MODULE_UI_BASE_API void updating() override;

    /// Configuration option to emit the clicked signal synchronously or asynchronously
    bool m_sync {false};
};

} // namespace sight::module::ui::base
