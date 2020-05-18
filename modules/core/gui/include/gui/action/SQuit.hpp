/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "gui/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <fwServices/IService.hpp>

namespace gui
{
namespace action
{

/**
 * @brief   This action tries to close the window and reset root object.
 */
class GUI_CLASS_API SQuit : public ::fwGui::IActionSrv
{

public:

    fwCoreServiceMacro(SQuit, ::fwGui::IActionSrv);

    /**
     * @brief Constructor. Do nothing.
     */
    GUI_API SQuit() noexcept;

    /**
     * @brief Destructor. Do nothing.
     */
    GUI_API virtual ~SQuit() noexcept;

protected:

    /**
     * @brief This method is used to configure the service parameters: specifies which services must be started or
     * stopped
     */
    GUI_API void configuring() override;

    GUI_API virtual void starting() override;

    GUI_API virtual void stopping() override;

    /**
     * @brief This method tries to close the window and reset root object.
     */
    GUI_API virtual void updating() override;

    /*
     * @brief This method gives information about the class.
     */
    GUI_API virtual void info(std::ostream& _sstream ) override;

};

} // namespace action
} // namespace gui
