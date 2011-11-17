/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_MESHNORMALS_HPP_
#define _VISUVTKADAPTOR_MESHNORMALS_HPP_

#include <vtkSmartPointer.h>

#include <fwServices/ObjectMsg.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkPolyData;
class vtkActor;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API MeshNormals: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (MeshNormals)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API MeshNormals() throw();

    VISUVTKADAPTOR_API virtual ~MeshNormals() throw();

    VISUVTKADAPTOR_API void setPolyData(vtkSmartPointer< vtkPolyData > polydata);

protected:

   VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
   VISUVTKADAPTOR_API virtual void doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
   VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    vtkActor* getActor();
    void updateMeshNormals();

private:

    vtkSmartPointer< vtkPolyData > m_polyData;
    vtkSmartPointer< vtkActor >    m_actor;
};


} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_MESHNORMALS_HPP_
