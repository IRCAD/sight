/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_FIXEDINTERACTORSTYLE_HPP__
#define __VISUVTKARADAPTOR_FIXEDINTERACTORSTYLE_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <vtkInteractorStyleUser.h>

/**
 *  @class FixedInteractorStyle
 *  @brief This class disable all user's interactions in a vtk view.
 *  @note This interactor style must be set to the visuVTKAdaptor::InteractorStyle.
 *  @verbatim
    <adaptor id="interactorStyle" class="::visuVTKAdaptor::InteractorStyle" objectId="self">
        <config renderer="default" style="FixedInteractorStyle" />
    </adaptor>
    @endverbatim
 *  @note All method are empty in order to disable interactions.
 */
class VISUVTKARADAPTOR_CLASS_API FixedInteractorStyle : public vtkInteractorStyleUser
{

public:

    ///  New macro
    VISUVTKARADAPTOR_API static FixedInteractorStyle *New();

    /// vtk revision macro
    vtkTypeRevisionMacro(FixedInteractorStyle, vtkInteractorStyle);

    VISUVTKARADAPTOR_API virtual void OnMiddleButtonDown();
    VISUVTKARADAPTOR_API virtual void OnChar();
    VISUVTKARADAPTOR_API virtual void OnLeftButtonDown();
    VISUVTKARADAPTOR_API virtual void OnMouseMove();
    VISUVTKARADAPTOR_API virtual void OnMouseWheelBackward();
    VISUVTKARADAPTOR_API virtual void OnMouseWheelForward();
    VISUVTKARADAPTOR_API virtual void OnRightButtonDown();

protected:

    /// constructor
    VISUVTKARADAPTOR_API FixedInteractorStyle();

    ///  destructor
    VISUVTKARADAPTOR_API ~FixedInteractorStyle();

private:

    FixedInteractorStyle(const FixedInteractorStyle&);  // Not implemented.

    void operator=(const FixedInteractorStyle&);  // Not implemented.

};

#endif // __VISUVTKARADAPTOR_FIXEDINTERACTORSTYLE_HPP__
