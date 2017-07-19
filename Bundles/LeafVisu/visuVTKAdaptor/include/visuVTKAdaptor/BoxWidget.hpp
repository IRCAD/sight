/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_BOXWIDGET_HPP__
#define __VISUVTKADAPTOR_BOXWIDGET_HPP__

#ifndef ANDROID

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IVtkAdaptorService.hpp>

class vtkTransform;
class vtkBoxWidget2;
class vtkCommand;

namespace visuVTKAdaptor
{

class Transform;

// BoxWidget
class VISUVTKADAPTOR_CLASS_API BoxWidget : public ::fwRenderVTK::IVtkAdaptorService
{
public:

    fwCoreServiceClassDefinitionsMacro( (BoxWidget)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API BoxWidget() noexcept;
    VISUVTKADAPTOR_API virtual ~BoxWidget() noexcept;

    VISUVTKADAPTOR_API void updateFromVtk();

protected:

    VISUVTKADAPTOR_API virtual void doConfigure();
    VISUVTKADAPTOR_API virtual void doStart();
    VISUVTKADAPTOR_API virtual void doStop();
    VISUVTKADAPTOR_API virtual void doSwap();
    VISUVTKADAPTOR_API virtual void doUpdate();

    vtkTransform* m_transform;
    WPTR(::visuVTKAdaptor::Transform) m_transformService;

private:

    vtkBoxWidget2* m_vtkBoxWidget;
    vtkCommand* m_boxWidgetCommand;

    double m_scaleFactor;
    bool m_enableScaling;
};

} // namespace visuVTKAdaptor

#endif // ANDROID

#endif // __VISUVTKADAPTOR_BOXWIDGET_HPP__

