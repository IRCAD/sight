/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_NORMALS_HPP_
#define _VISUVTKADAPTOR_NORMALS_HPP_

#include <fwServices/ObjectMsg.hpp>

#include <vtkCommand.h>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkActor;
class vtkActor2D;
class vtkAlgorithmOutput;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API Normals: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Normals)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    VISUVTKADAPTOR_API Normals() throw();

    VISUVTKADAPTOR_API virtual ~Normals() throw();

    VISUVTKADAPTOR_API void setMapperInput(vtkAlgorithmOutput *input);

protected:

   VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
   VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
   VISUVTKADAPTOR_API virtual void doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
   VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);


    vtkActor* getActor();
    void updateNormals();

private:

    vtkAlgorithmOutput *m_mapperInput;

    vtkActor   * m_actor ;
    vtkActor2D * m_ptsIdActor2D ;
    vtkActor2D * m_cellsIdActor2D ;
};



} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_NORMALS_HPP_
