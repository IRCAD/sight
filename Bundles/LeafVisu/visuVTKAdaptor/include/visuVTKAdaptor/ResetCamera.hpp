/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_RESETCAMERA_HPP_
#define _VISUVTKADAPTOR_RESETCAMERA_HPP_

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

namespace visuVTKAdaptor
{

/**
 * @brief   Do a reset camera.
 * @class   ResetCamera
 *    The priority  of the comChannel of this Adaptor is very low (0.1). The update of the GenericScene must be done before this reset.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class VISUVTKADAPTOR_CLASS_API ResetCamera: public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (ResetCamera)(::fwRenderVTK::IVtkAdaptorService) ) ;

    VISUVTKADAPTOR_API ResetCamera() throw();

    VISUVTKADAPTOR_API virtual ~ResetCamera() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doUpdate(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

private:

    bool bCameraReseted;

};
} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_RESETCAMERA_HPP_
