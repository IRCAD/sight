/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_SLICEFOLLOWERCAMERA_HPP_
#define _VISUVTKADAPTOR_SLICEFOLLOWERCAMERA_HPP_

#include <vector>
#include <fwComEd/helper/MedicalImageAdaptor.hpp>
#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API SliceFollowerCamera: public ::fwComEd::helper::MedicalImageAdaptor,public ::fwRenderVTK::IVtkAdaptorService 
{
public:
    fwCoreServiceClassDefinitionsMacro ( (SliceFollowerCamera)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    VISUVTKADAPTOR_API SliceFollowerCamera() throw();

    VISUVTKADAPTOR_API virtual ~SliceFollowerCamera() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);


    void initializeCamera();
    void updateCamera(double distance = -1., double size=-1);

    vtkCamera* m_camera;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_SLICEFOLLOWERCAMERA_HPP_
