/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWRENDERVTK_VTK_INTERACTORSTYLE3DFORNEGATO_HPP__
#define __FWRENDERVTK_VTK_INTERACTORSTYLE3DFORNEGATO_HPP__

#include "fwRenderVTK/config.hpp"
#include "fwRenderVTK/IInteractorStyle.hpp"

#include <vtkInteractorStyleTrackballCamera.h>

class Renderer;

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

#endif // __FWRENDERVTK_VTK_INTERACTORSTYLE3DFORNEGATO_HPP__
