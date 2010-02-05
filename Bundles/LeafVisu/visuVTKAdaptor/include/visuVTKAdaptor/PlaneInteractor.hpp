/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_PLANEINTERACTOR_HPP_
#define _VISUVTKADAPTOR_PLANEINTERACTOR_HPP_

#include <fwData/TransfertFunction.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/IImagesAdaptor.hpp"


class vtkCommand;

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API PlaneInteractor: public ::visuVTKAdaptor::IImagesAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (PlaneInteractor)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    VISUVTKADAPTOR_API PlaneInteractor() throw();

    VISUVTKADAPTOR_API virtual ~PlaneInteractor() throw();

    VISUVTKADAPTOR_API void switchPlaneNormal();

    VISUVTKADAPTOR_API void pushPlane(double factor);

    VISUVTKADAPTOR_API void deselectPlane();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    vtkCommand *m_vtkObserver;
    double m_priority;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_PLANEINTERACTOR_HPP_
