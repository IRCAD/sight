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

#include "fwRender/config.hpp"

#include <fwGui/IGuiContainerSrv.hpp>

#include <fwServices/IService.hpp>

#include <fwTools/Failed.hpp>

namespace fwRender
{

/**
 * @brief   Defines the service interface managing the rendering service for object.
 *
 * Use the base service methods :
 * @li The service is configured with methods setConfiguration(cfg) and configure()
 * @li The method start() initialize the service
 * @li Use update() method to update data
 * @li To react on modifications use update(msg)
 * @li Finally we must call stop() before deleting the service
 *
 */

class FWRENDER_CLASS_API IRender : public ::fwGui::IGuiContainerSrv
{

public:

    fwCoreServiceMacro(IRender, ::fwGui::IGuiContainerSrv);

protected:

    /**
     * @brief   Constructor. Initialize default values.
     */
    FWRENDER_API IRender() noexcept;

    /**
     * @brief   Destructor. Do nothing.
     */
    FWRENDER_API virtual ~IRender() noexcept;

};

} // namespace fwRenderVTK
