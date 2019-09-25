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

#include <fwTools/Failed.hpp>

namespace fwGui
{
namespace editor
{

/**
 * @brief Defines the service interface managing the editor service for object.
 */

class FWGUI_CLASS_API IEditor : public ::fwGui::IGuiContainerSrv
{

public:

    fwCoreServiceMacro(IEditor, ::fwGui::IGuiContainerSrv);

protected:

    /**
     * @brief   Constructor. Do nothing (Just initialize parameters).
     */
    IEditor() noexcept
    {
    }

    /// Destructor. Do nothing.
    virtual ~IEditor() noexcept
    {
    }

};

} // namespace editor
} // namespace fwGui
