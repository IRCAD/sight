/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwRenderVTK/IInteractorStyle.hpp"

#include "visuVTKARAdaptor/config.hpp"

#include <vtkInteractorStyleUser.h>

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
 *  @note All overriden methods do absolutely nothing.
 */
class VISUVTKARADAPTOR_CLASS_API FixedInteractorStyle : public vtkInteractorStyleUser,
                                                        public ::fwRenderVTK::IInteractorStyle
{

public:

    /// New macro.
    VISUVTKARADAPTOR_API static FixedInteractorStyle* New();

    /// VTK revision macro.
    vtkTypeMacro(FixedInteractorStyle, vtkInteractorStyle);

    VISUVTKARADAPTOR_API virtual void OnMiddleButtonDown() override;
    VISUVTKARADAPTOR_API virtual void OnChar() override;
    VISUVTKARADAPTOR_API virtual void OnLeftButtonDown() override;
    VISUVTKARADAPTOR_API virtual void OnMouseMove() override;
    VISUVTKARADAPTOR_API virtual void OnMouseWheelBackward() override;
    VISUVTKARADAPTOR_API virtual void OnMouseWheelForward() override;
    VISUVTKARADAPTOR_API virtual void OnRightButtonDown() override;

protected:

    /// Constructor.
    VISUVTKARADAPTOR_API FixedInteractorStyle();

    /// Destructor.
    VISUVTKARADAPTOR_API ~FixedInteractorStyle();

private:

    /// Not implemented.
    FixedInteractorStyle(const FixedInteractorStyle&);

    /// Not implemented.
    void operator=(const FixedInteractorStyle&);

};
