/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_CAMERA_HPP__
#define __VISUVTKADAPTOR_CAMERA_HPP__

#include <vector>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{

/**
 * @brief Render Camera (video) in the generic scene.
 */
class VISUVTKADAPTOR_CLASS_API Camera: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Camera)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API Camera() throw();

    VISUVTKADAPTOR_API virtual ~Camera() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

private:

    bool initCameraParameters();

    bool bCam_init;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_CAMERA_HPP__

