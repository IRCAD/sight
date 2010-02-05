/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_VIDEO_HPP_
#define _VISUVTKADAPTOR_VIDEO_HPP_

#include <vector>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"


class VISUVTKADAPTOR_CLASS_API vtkImageData;
class VISUVTKADAPTOR_CLASS_API vtkUnsignedCharArray;
class VISUVTKADAPTOR_CLASS_API vtkTexture;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API Video: public ::fwRenderVTK::IVtkAdaptorService, protected MeshFactory
{

public:
    fwCoreServiceClassDefinitionsMacro ( (Video)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    Video() throw();
    virtual ~Video() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

private:

    vtkImageData* m_imageData;
    vtkUnsignedCharArray* m_array;
    vtkTexture* m_texture ;

    bool bText_init;
    bool bForceRender;
//  enum ARL_VIDEO_FLIP { VIDEO_FLIP_VERTICAL, VIDEO_FLIP_HORIZONTAL, VIDEO_FLIP_RADIAL, VIDEO_NBFLIP };

};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_VIDEO_HPP_
