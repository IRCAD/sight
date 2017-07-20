/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_PLANEINTERACTOR_HPP__
#define __VISUVTKADAPTOR_PLANEINTERACTOR_HPP__

#ifndef ANDROID

#include "visuVTKAdaptor/config.hpp"

#include <fwData/TransferFunction.hpp>

#include <fwDataTools/helper/MedicalImageAdaptor.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkCommand;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PlaneInteractor : public ::fwDataTools::helper::MedicalImageAdaptor,
                                                 public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (PlaneInteractor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API PlaneInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~PlaneInteractor() noexcept;

    VISUVTKADAPTOR_API void switchPlaneNormal();

    VISUVTKADAPTOR_API void pushPlane(double factor);

    VISUVTKADAPTOR_API void deselectPlane();

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();

    vtkCommand* m_vtkObserver;
    double m_priority;
};

} //namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_PLANEINTERACTOR_HPP__
