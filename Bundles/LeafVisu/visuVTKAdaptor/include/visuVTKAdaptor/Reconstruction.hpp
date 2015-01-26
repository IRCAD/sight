/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
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

    void setClippingPlanes(::fwRenderVTK::VtkRenderService::VtkObjectIdType id){ m_clippingPlanesId = id ; }

    void setSharpEdgeAngle(double angle){ m_sharpEdgeAngle = angle; }

    VISUVTKADAPTOR_API void setForceHide(bool hide);

    /// Set underlying actor visible or not
    VISUVTKADAPTOR_API void setVisibility(bool visible);

    /// Active/Inactive automatic reset on camera for triangular mesh adaptor. By default =true.
    VISUVTKADAPTOR_API void setAutoResetCamera(bool autoResetCamera);

    /// Retrieve a weak_ptr on underlying mesh adaptor service
    ::fwRenderVTK::IVtkAdaptorService::wptr getMeshService() const { return m_meshService; }

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKADAPTOR_API virtual void doReceive( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);


    void createMeshService();

    ::fwRenderVTK::VtkRenderService::VtkObjectIdType m_clippingPlanesId;

    ::fwRenderVTK::IVtkAdaptorService::wptr m_meshService;

    double      m_sharpEdgeAngle;

private:
    bool   m_autoResetCamera;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_RECONSTRUCTION_HPP_
