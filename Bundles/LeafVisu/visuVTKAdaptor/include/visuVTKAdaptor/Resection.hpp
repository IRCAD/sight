/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_RESECTION_HPP_
#define _VISUVTKADAPTOR_RESECTION_HPP_

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"


namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API Resection: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Resection)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Resection() throw();

    VISUVTKADAPTOR_API virtual ~Resection() throw();

    VISUVTKADAPTOR_API void setClippingPlanes(::fwRenderVTK::VtkRenderService::VtkObjectIdType id){ m_clippingPlanes = id ; }

    VISUVTKADAPTOR_API void setSharpEdgeAngle(double angle){ m_sharpEdgeAngle = angle; }

protected:
    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    // manage only "ShowReconstructions" Field
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

private:
    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanes;
    double m_sharpEdgeAngle;
    bool   m_autoResetCamera;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_RESECTION_HPP_
