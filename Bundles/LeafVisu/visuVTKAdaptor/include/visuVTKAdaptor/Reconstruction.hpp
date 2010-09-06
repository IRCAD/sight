/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_RECONSTRUCTION_HPP_
#define _VISUVTKADAPTOR_RECONSTRUCTION_HPP_

#include <fwServices/ObjectMsg.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"

namespace visuVTKAdaptor
{

class VISUVTKADAPTOR_CLASS_API Reconstruction: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Reconstruction)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Reconstruction() throw();

    VISUVTKADAPTOR_API virtual ~Reconstruction() throw();

    VISUVTKADAPTOR_API void setClippingPlanes(::fwRenderVTK::VtkRenderService::VtkObjectIdType id){ m_clippingPlanesId = id ; }

    VISUVTKADAPTOR_API void setSharpEdgeAngle(double angle){ m_sharpEdgeAngle = angle; }

    VISUVTKADAPTOR_API void setForceHide(bool hide);

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKADAPTOR_API virtual void doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);


    void createMeshService();

    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanesId;

    ::fwRenderVTK::IVtkAdaptorService::wptr m_meshService;

    double      m_sharpEdgeAngle;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_RECONSTRUCTION_HPP_
