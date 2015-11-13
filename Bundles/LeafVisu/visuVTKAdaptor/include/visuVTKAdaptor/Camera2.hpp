/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_CAMERA2_HPP__
#define __VISUVTKADAPTOR_CAMERA2_HPP__

#include <vector>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"

class vtkCommand;
class vtkPerspectiveTransform;

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API Camera2 : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Camera2)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Camera2() throw();

    VISUVTKADAPTOR_API virtual ~Camera2() throw();

    VISUVTKADAPTOR_API void updateFromVtk();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

    VISUVTKADAPTOR_API void doReceive(::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed);

private:
    vtkCommand* m_cameraCommand;
    vtkPerspectiveTransform* m_transOrig;

};




} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_CAMERA2_HPP__
