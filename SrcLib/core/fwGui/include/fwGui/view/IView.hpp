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

#include "fwGui/config.hpp"
#include "fwGui/IGuiContainerSrv.hpp"

#include <fwServices/IService.hpp>

namespace fwGui
{
namespace view
{

/**
 * @brief   Defines the service interface managing a view placed in main frame.
 *
 */
class FWGUI_CLASS_API IView : public ::fwGui::IGuiContainerSrv
{

public:

    fwCoreClassMacro(IView, ::fwGui::IGuiContainerSrv);

protected:

    /**
     * @brief Constructor. Do nothing (Just initialize parameters).
     */
    IView() noexcept
    {
    }

    /**
     * @brief Destructor. Do nothing.
     */
    virtual ~IView() noexcept
    {
    }

};

} // namespace view
} // namespace fwGui
