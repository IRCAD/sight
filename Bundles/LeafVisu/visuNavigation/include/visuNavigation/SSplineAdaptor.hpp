/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUNAVIGATION_SSPLINEADAPTOR_HPP__
#define __VISUNAVIGATION_SSPLINEADAPTOR_HPP__

#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkPoints.h>
#include <vtkSmartPointer.h>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include "visuNavigation/config.hpp"

namespace visuNavigation
{

/**
 * @brief Adaptor representing a spline from a point list.
 * @class SSplineAdaptor
 */
class VISUNAVIGATION_CLASS_API SSplineAdaptor: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro((SSplineAdaptor)(::fwRenderVTK::IVtkAdaptorService));

    /**
     * @name Constructor/Destructor
     * @{ */
    VISUNAVIGATION_API SSplineAdaptor() throw();
    VISUNAVIGATION_API virtual ~SSplineAdaptor() throw();
    /**  @} */

protected:

    /**
     * @name Overrides.
     * @{ */
    VISUNAVIGATION_API void doStart() throw(fwTools::Failed);
    VISUNAVIGATION_API void doStop() throw(fwTools::Failed);
    VISUNAVIGATION_API void doSwap() throw(fwTools::Failed) {};
    VISUNAVIGATION_API void doUpdate() throw(fwTools::Failed);
    VISUNAVIGATION_API void doReceive(CSPTR(::fwServices::ObjectMsg)) throw(fwTools::Failed);
    VISUNAVIGATION_API void configuring() throw(fwTools::Failed);
    /**  @} */

private:

    /// Spline as a VTK object.
    vtkSmartPointer<vtkParametricSpline> m_parametricSpline;

    /// Spline points.
    vtkSmartPointer<vtkPoints> m_vtkpoints;

    /// Number of points.
    int m_numberOfPoints;

    /// Spline length.
    double m_splineLength;
};

} //namespace visuNavigation

#endif /* __VISUNAVIGATION_SSPLINEADAPTOR_HPP__ */

