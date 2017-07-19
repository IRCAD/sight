/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_CAMERA2_HPP__
#define __VISUVTKADAPTOR_CAMERA2_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include <vector>

class vtkCommand;
class vtkPerspectiveTransform;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API Camera2 : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Camera2)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Camera2() noexcept;

    VISUVTKADAPTOR_API virtual ~Camera2() noexcept;

    VISUVTKADAPTOR_API void updateFromVtk();

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doStop();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate();

private:
    vtkCommand* m_cameraCommand;
    vtkPerspectiveTransform* m_transOrig;

};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_CAMERA2_HPP__
