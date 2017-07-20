/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_POINTLISTINTERACTOR_HPP__
#define __VISUVTKADAPTOR_POINTLISTINTERACTOR_HPP__

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

#include <vector>

class vtkCommand;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PointListInteractor : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (PointListInteractor)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API PointListInteractor() noexcept;

    VISUVTKADAPTOR_API virtual ~PointListInteractor() noexcept;

    VISUVTKADAPTOR_API void resetPointList();
    VISUVTKADAPTOR_API void addPoint(const double& x, const double& y, const double& z);

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();

    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();

    vtkCommand* m_interactionCommand;
    float m_priority;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_POINTLISTINTERACTOR_HPP__
