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

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/IInteractorStyle.hpp"

#include <vtkInteractorStyleTrackballCamera.h>

class Renderer;

namespace fwRenderVTK
{

namespace vtk
{

class FWRENDERVTK_CLASS_API InteractorStyle3DForNegato : public vtkInteractorStyleTrackballCamera,
                                                         public ::fwRenderVTK::IInteractorStyle
{

public:

    FWRENDERVTK_API static InteractorStyle3DForNegato* New();

    vtkTypeMacro(InteractorStyle3DForNegato, vtkInteractorStyle);

    FWRENDERVTK_API virtual void OnMiddleButtonDown() override;

    FWRENDERVTK_API virtual void OnChar() override;
    FWRENDERVTK_API virtual void OnKeyUp() override;
    FWRENDERVTK_API virtual void OnKeyDown() override;

    FWRENDERVTK_API virtual void OnLeftButtonDown() override;
    FWRENDERVTK_API virtual void OnMouseMove() override;
    FWRENDERVTK_API virtual void OnRightButtonDown() override;
    FWRENDERVTK_API virtual void OnMouseWheelBackward() override;
    FWRENDERVTK_API virtual void OnMouseWheelForward() override;

    FWRENDERVTK_API virtual void Rotate() override;
    FWRENDERVTK_API virtual void Spin() override;
    FWRENDERVTK_API virtual void Pan() override;
    FWRENDERVTK_API virtual void Dolly() override;

protected:

    FWRENDERVTK_API InteractorStyle3DForNegato();
    FWRENDERVTK_API ~InteractorStyle3DForNegato();

    FWRENDERVTK_API virtual void Dolly(double factor) override;
private:

    InteractorStyle3DForNegato(const InteractorStyle3DForNegato&);  // Not implemented.

    void operator=(const InteractorStyle3DForNegato&);  // Not implemented.

    double m_newPickPoint[4];
    double m_oldPickPoint[4];
};

} // namespace vtk

} // namespace fwRenderVTK
