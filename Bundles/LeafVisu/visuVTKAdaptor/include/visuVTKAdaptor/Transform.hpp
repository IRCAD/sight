/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_TRANSFORM_HPP_
#define _VISUVTKADAPTOR_TRANSFORM_HPP_

#include <vector>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"

class vtkTransform;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API Transform: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Transform)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Transform() throw();

    VISUVTKADAPTOR_API virtual ~Transform() throw();
    VISUVTKADAPTOR_API void setTransform(vtkTransform *t);
    VISUVTKADAPTOR_API vtkTransform *getTransform();

    VISUVTKADAPTOR_API void updateFromVtk();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    // manage only "ShowReconstructions" Field
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

    vtkTransform* m_transform;
    vtkCommand* m_transformCommand;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_TRANSFORM_HPP_
