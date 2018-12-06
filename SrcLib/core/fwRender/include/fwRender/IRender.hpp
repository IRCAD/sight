/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWRENDER_IRENDER_HPP__
#define __FWRENDER_IRENDER_HPP__

#include <fwTools/Failed.hpp>
#include <fwServices/IService.hpp>

#include <fwGui/IGuiContainerSrv.hpp>

#include "fwRender/config.hpp"


namespace fwRender
{

/**
 * @brief   Defines the service interface managing the rendering service for object.
 * @class   IRender
 *
 * @date    2007-2009.
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

    fwCoreServiceClassDefinitionsMacro ( (IRender)(::fwGui::IGuiContainerSrv) );

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

#endif /*__FWRENDER_IRENDER_HPP__*/


