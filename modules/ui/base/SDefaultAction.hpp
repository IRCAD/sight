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

#include <service/IService.hpp>

#include <ui/base/IAction.hpp>

namespace sight::module::ui::base
{

/**
 * @brief  Defines the default action to have a simple action button.
 */
class MODULE_UI_BASE_CLASS_API SDefaultAction : public ::sight::ui::base::IAction
{
public:

    SIGHT_DECLARE_SERVICE(SDefaultAction, ::sight::ui::base::IAction);

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_BASE_API SDefaultAction() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_BASE_API virtual ~SDefaultAction() noexcept;

protected:

    /** @name Service methods ( override from service::IService )
     * @{
     */

    MODULE_UI_BASE_API void configuring() override;

    MODULE_UI_BASE_API void starting() override;

    MODULE_UI_BASE_API void stopping() override;

    MODULE_UI_BASE_API void updating() override;

    MODULE_UI_BASE_API void info(std::ostream& _sstream) override;

    ///@}
};

} // namespace sight::module::ui::base
