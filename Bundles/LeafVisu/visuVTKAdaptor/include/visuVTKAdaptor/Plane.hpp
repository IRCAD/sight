/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_PLANE_HPP_
#define _VISUVTKADAPTOR_PLANE_HPP_

#include <fwServices/ObjectMsg.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkPlaneSource;
class vtkPlaneCollection;
class vtkPlane;
class vtkObject;

namespace fwData
{
    class Plane;
}

namespace visuVTKAdaptor
{



class VISUVTKADAPTOR_CLASS_API Plane: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Plane)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Plane() throw();

    VISUVTKADAPTOR_API virtual ~Plane() throw();

    VISUVTKADAPTOR_API void setVtkPlaneCollection( vtkObject * col );

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API virtual void doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    void selectPlane(bool select);

private:

    vtkPlaneSource* m_vtkPlane;
    vtkActor *m_actorPlan;
    fwData::Plane::wptr m_pPlane;

    vtkPlane* m_vtkImplicitPlane;
    vtkPlaneCollection* m_vtkPlaneCollection;
};


} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_PLANE_HPP_
