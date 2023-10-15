/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include <service/base.hpp>

#include <ui/__/action.hpp>

namespace sight::module::ui
{

/**
 * @brief  Defines the default action to have a simple action button.
 * @deprecated Replaced by sight::module::ui::action since Sight 22.0, will be removed in Sight 23.0
 */
class MODULE_UI_CLASS_API default_action : public sight::ui::action
{
public:

    SIGHT_DECLARE_SERVICE(default_action, sight::ui::action);

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_API default_action() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_API ~default_action() noexcept override;

protected:

    /** @name Service methods ( override from service::base )
     * @{
     */

    MODULE_UI_API void configuring() override;

    MODULE_UI_API void starting() override;

    MODULE_UI_API void stopping() override;

    MODULE_UI_API void updating() override;

    MODULE_UI_API void info(std::ostream& _sstream) override;

    ///@}
};

} // namespace sight::module::ui
