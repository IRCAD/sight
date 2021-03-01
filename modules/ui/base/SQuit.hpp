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
 * @brief   This action tries to close the window and reset root object.
 */
class MODULE_UI_BASE_CLASS_API SQuit : public ::sight::ui::base::IAction
{

public:

    SIGHT_DECLARE_SERVICE(SQuit, ::sight::ui::base::IAction)

    /**
     * @brief Constructor. Do nothing.
     */
    MODULE_UI_BASE_API SQuit() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    MODULE_UI_BASE_API virtual ~SQuit() noexcept;

protected:

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or
     * stopped
     */
    MODULE_UI_BASE_API void configuring() override;

    MODULE_UI_BASE_API virtual void starting() override;

    MODULE_UI_BASE_API virtual void stopping() override;

    /**
     * @brief This method tries to close the window and reset root object.
     */
    MODULE_UI_BASE_API virtual void updating() override;

    /*
     * @brief This method gives information about the class.
     */
    MODULE_UI_BASE_API virtual void info(std::ostream& _sstream ) override;

};

} // namespace sight::module::ui::base
