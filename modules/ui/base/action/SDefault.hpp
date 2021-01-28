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

#include <services/IService.hpp>

#include <ui/base/IActionSrv.hpp>

namespace sight::modules::ui::base
{
namespace action
{

/**
 * @brief  Defines the default action to have a simple action button.
 */
class MODULE_UI_BASE_CLASS_API SDefault : public ::sight::ui::base::IActionSrv
{

public:

    fwCoreServiceMacro(SDefault, ::sight::ui::base::IActionSrv)

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_BASE_API SDefault() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_BASE_API virtual ~SDefault() noexcept;

protected:

    /** @name Service methods ( override from services::IService )
     * @{
     */

    MODULE_UI_BASE_API virtual void configuring() override;

    MODULE_UI_BASE_API virtual void starting() override;

    MODULE_UI_BASE_API virtual void stopping() override;

    MODULE_UI_BASE_API virtual void updating() override;

    MODULE_UI_BASE_API virtual void info(std::ostream& _sstream ) override;

    ///@}

};

} // namespace action
} // namespace sight::modules::ui::base
