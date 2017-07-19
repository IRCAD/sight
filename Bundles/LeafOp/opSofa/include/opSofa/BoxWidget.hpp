/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPSOFA_BOXWIDGET_HPP__
#define __OPSOFA_BOXWIDGET_HPP__

#include <fwRenderVTK/IVtkAdaptorService.hpp>
#include "opSofa/config.hpp"

class vtkTransform;
class vtkBoxWidget2;


namespace opSofa
{

class Transform;

// BoxWidget
class BoxWidget : public ::fwRenderVTK::IVtkAdaptorService
{
public:

    fwCoreServiceClassDefinitionsMacro( (BoxWidget)(::fwRenderVTK::IVtkAdaptorService) );

    BoxWidget() noexcept;
    virtual ~BoxWidget() noexcept;

    void updateFromVtk();

protected:

    virtual void configuring();
    virtual void doStart();
    virtual void doStop();
    virtual void doSwap();
    virtual void doUpdate();
    virtual void doReceive( ::fwServices::ObjectMsg::csptr msg );

    vtkTransform* m_transform;
    WPTR(::visuVTKAdaptor::Transform) m_transformService;

private:

    ::vtkBoxWidget2* m_vtkBoxWidget;
    vtkCommand* m_boxWidgetCommand;

    double m_scaleFactor;
    bool m_enableScaling;
    std::string m_idMesh;
};

} // namespace visuVTKAdaptor

#endif // __OPSOFA_BOXWIDGET_HPP__
