/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "opPicking/config.hpp"

#include <fwCom/Signal.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwServices/IService.hpp>

namespace opPicking
{

/**
 * @brief Apply a transformation matrix on a picked point.
 *
 * * @section Signals Signals
 * - \b pickedPointTransformed(::fwDataTools::PickingInfo): Emitted when a picked point is transformed.
 *
 * @section Slots Slots
 * - \b pickedPoint(::fwDataTools::PickingInfo): Transform picked point.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::opPicking::STransformPickedPoint">
            <in key="matrix" uid="..." />
       </service>
   @endcode
 *
 * @subsection Input Input:
 * - \b matrix [::fwData::TransformationMatrix3D]: Input matrix.
 */
class OPPICKING_CLASS_API STransformPickedPoint : public ::fwServices::IService
{

public:

    fwCoreServiceMacro(STransformPickedPoint, ::fwServices::IService)

    typedef ::fwCom::Signal< void (::fwDataTools::PickingInfo) > PickedPointTransformedSignalType;

    /// Creates the slot and the signal.
    OPPICKING_API STransformPickedPoint();

    /// Destroys the services.
    OPPICKING_API virtual ~STransformPickedPoint() final;

private:

    /// Does nothing.
    virtual void starting() final;

    /// Does nothing.
    virtual void stopping() final;

    /// Does nothing.
    virtual void updating() final;

    /// Configures the service.
    virtual void configuring() final;

    /**
     * @brief SLOT: This method transforms picking coordinates.
     * @param _info picking information.
     */
    void pickedPoint(::fwDataTools::PickingInfo _info);

};
} // opPicking
