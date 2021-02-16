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

namespace sight::modules::ui::base
{
namespace action
{

/**
 * @brief   This action does nothing.
 * @deprecated use SDefault instead
 */
class MODULE_UI_BASE_CLASS_API SDoNothing : public ::sight::ui::base::IAction
{

public:

    fwCoreServiceMacro(SDoNothing, ::sight::ui::base::IAction)

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_BASE_API SDoNothing() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_BASE_API virtual ~SDoNothing() noexcept;

protected:

    /** @name Service methods ( override from service::IService )
     * @{
     */

    /**
     * @brief This method is used to configure the action.
     */
    MODULE_UI_BASE_API virtual void configuring() override;

    MODULE_UI_BASE_API virtual void starting() override;

    MODULE_UI_BASE_API virtual void stopping() override;

    /**
     * @brief Process the action. Do nothing.
     */
    MODULE_UI_BASE_API virtual void updating() override;

    /**
     * @brief This method gives information about the class.
     */
    MODULE_UI_BASE_API virtual void info(std::ostream& _sstream ) override;

    ///@}

};

} // namespace action
} // namespace sight::modules::ui::base
