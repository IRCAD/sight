/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/IInteractorStyle.hpp"

#include <vtkInteractorStyleUser.h>

namespace fwRenderVTK
{

namespace vtk
{

/**
 *  @brief Interactor class disabling all user interactions in a VTK scene.
 *
 *  @note Interactor styles can be set in ::visuVTKAdaptor::SInteractorStyle.
 *
 *  @see ::visuVTKAdaptor::SInteractorStyle.
 *
 *  @code{.xml}
    <service type="::visuVTKAdaptor::InteractorStyle" >
        <config renderer="default" style="FixedInteractorStyle" />
    </service>
    @endcode
 *
 *  @note All overriden methods does absolutely nothing.
 */
class FWRENDERVTK_CLASS_API FixedInteractorStyle : public vtkInteractorStyleUser,
                                                   public ::fwRenderVTK::IInteractorStyle
{

public:

    /// New macro.
    FWRENDERVTK_API static FixedInteractorStyle* New();

    /// VTK revision macro.
    vtkTypeMacro(FixedInteractorStyle, vtkInteractorStyle)

    FWRENDERVTK_API virtual void OnMiddleButtonDown() override;
    FWRENDERVTK_API virtual void OnChar() override;
    FWRENDERVTK_API virtual void OnLeftButtonDown() override;
    FWRENDERVTK_API virtual void OnMouseMove() override;
    FWRENDERVTK_API virtual void OnMouseWheelBackward() override;
    FWRENDERVTK_API virtual void OnMouseWheelForward() override;
    FWRENDERVTK_API virtual void OnRightButtonDown() override;

protected:

    /// Constructor.
    FWRENDERVTK_API FixedInteractorStyle();

    /// Destructor.
    FWRENDERVTK_API ~FixedInteractorStyle() override;

private:

    /// Not implemented.
    FixedInteractorStyle(const FixedInteractorStyle&);

    /// Not implemented.
    void operator=(const FixedInteractorStyle&);

};

} // namespace vtk
} // namespace fwRenderVTK
