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

#include "modules/data/config.hpp"

#include <services/IController.hpp>
#include <services/IService.hpp>

namespace sight::modules::data
{

/**
 * @brief  This service convert its attached image to a medical image by adding specific fields.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service type="::sight::modules::data::MedicalImageSrv">
           <inout key="image" uid="..."/>
       </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [data::Image]: image to be converted into a medical image.
 */
class MODULE_DATA_CLASS_API MedicalImageSrv : public services::IController
{

public:

    fwCoreServiceMacro(MedicalImageSrv, services::IController)

    MODULE_DATA_API MedicalImageSrv() noexcept;

    MODULE_DATA_API virtual ~MedicalImageSrv() noexcept;

protected:

    /// Implements starting method derived from IService. Convert the image.
    MODULE_DATA_API virtual void starting() override;

    /// Implements stopping method derived from IService. Do nothing.
    MODULE_DATA_API virtual void stopping() override;

    /// Implements swapping method derived from IService. Convert the image.
    MODULE_DATA_API virtual void swapping() override;

    /// Implements configuring method derived from IService. Do nothing.
    MODULE_DATA_API virtual void configuring() override;

    /// Implements reconfiguring method derived from IService. Do nothing.
    MODULE_DATA_API virtual void reconfiguring() override;

    /// Implements updating method derived from IService. Do nothing.
    MODULE_DATA_API virtual void updating() override;

    /// Implements info method derived from IService. Print classname.
    MODULE_DATA_API virtual void info( std::ostream& _sstream ) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Image::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     * Connect Image::s_BUFFER_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    MODULE_DATA_API virtual KeyConnectionsMap getAutoConnections() const override;

private:

    void convertImage();

    std::vector< std::string > m_imageCompositeKeys;
};

} // sight::modules::data
