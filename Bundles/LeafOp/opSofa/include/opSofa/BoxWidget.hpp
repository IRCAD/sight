/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_BOXWIDGETdd_HPP_
#define _VISUVTKADAPTOR_BOXWIDGETdd_HPP_

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

                  BoxWidget() throw();
     virtual     ~BoxWidget() throw();

     void updateFromVtk();

protected:

     virtual void configuring() throw( ::fwTools::Failed );
     virtual void doStart()     throw( ::fwTools::Failed );
     virtual void doStop()      throw( ::fwTools::Failed );
     virtual void doSwap()      throw( ::fwTools::Failed );
     virtual void doUpdate()    throw( ::fwTools::Failed );
     virtual void doReceive( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed );

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

#endif // _VISUVTKADAPTOR_BOXWIDGET_HPP_
