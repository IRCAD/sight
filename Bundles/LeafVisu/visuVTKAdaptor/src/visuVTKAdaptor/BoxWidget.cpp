/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/BoxWidget.hpp"

#include "visuVTKAdaptor/Transform.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp>

#include <fwServices/macros.hpp>

#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

#include <boost/lexical_cast.hpp>

namespace visuVTKAdaptor
{

class BoxClallback : public ::vtkCommand
{
public:

    //------------------------------------------------------------------------------

    static BoxClallback* New(::visuVTKAdaptor::BoxWidget* adaptor)
    {
        BoxClallback* cb = new BoxClallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

    BoxClallback() :
        m_adaptor(nullptr)
    {
    }
    ~BoxClallback()
    {
    }

    //------------------------------------------------------------------------------

    virtual void Execute( ::vtkObject* pCaller, unsigned long eventId, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::BoxWidget* m_adaptor;
};

// BoxWidget

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::BoxWidget,
                         ::fwData::TransformationMatrix3D );

//------------------------------------------------------------------------------

BoxWidget::BoxWidget() noexcept :
    ::fwRenderVTK::IVtkAdaptorService(),
    m_transform(nullptr),
    m_vtkBoxWidget(nullptr),
    m_boxWidgetCommand(nullptr),
    m_scaleFactor(1.0),
    m_enableScaling(true)
{
    m_boxWidgetCommand = BoxClallback::New(this);
}

//------------------------------------------------------------------------------

BoxWidget::~BoxWidget() noexcept
{
}

//------------------------------------------------------------------------------

void BoxWidget::doConfigure()
{
    if (m_configuration->hasAttribute("scaleFactor"))
    {
        m_scaleFactor = ::boost::lexical_cast<double>(m_configuration->getAttributeValue("scaleFactor"));
    }

    if (m_configuration->hasAttribute("enableScaling"))
    {
        SLM_ASSERT("Wrong value for 'enableScaling', must be 'true' or 'false'",
                   m_configuration->getAttributeValue("enableScaling") == "yes" ||
                   m_configuration->getAttributeValue("enableScaling") == "no");
        m_enableScaling = (m_configuration->getAttributeValue("enableScaling") == "yes");
    }
}

//------------------------------------------------------------------------------

void BoxWidget::doStart()
{
    m_transform = getTransform();
    SLM_ASSERT("BoxWidget need a vtkTransform", m_transform);
    fwVtkBoxRepresentation* boxRep = fwVtkBoxRepresentation::New();
    boxRep->SetPlaceFactor(m_scaleFactor);

    double bounds[] = {-1, 1, -1, 1, -1, 1};
    boxRep->PlaceWidget(bounds);

    m_vtkBoxWidget = ::vtkBoxWidget2::New();
    m_vtkBoxWidget->SetRepresentation(boxRep);
    m_vtkBoxWidget->SetInteractor( this->getInteractor() );
    if (!m_enableScaling)
    {
        boxRep->ScalingEnabledOff();
    }
    m_vtkBoxWidget->On();

    boxRep->SetTransform(m_transform);

    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );

    this->doUpdate();
}

//------------------------------------------------------------------------------

void BoxWidget::doStop()
{
    unregisterServices();

    m_transform->Delete();
    m_transform = 0;
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    m_vtkBoxWidget->Delete();
    m_vtkBoxWidget = 0;

}

//------------------------------------------------------------------------------

void BoxWidget::doSwap()
{
    doUpdate();
}

//------------------------------------------------------------------------------

void BoxWidget::updateFromVtk()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );

    vtkBoxRepresentation* repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        repr->GetTransform(m_transform);
        m_transform->Modified();
    }

    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    vtkMatrix4x4* mat = m_transform->GetMatrix();

    for(int lt = 0; lt < 4; lt++)
    {
        for(int ct = 0; ct < 4; ct++)
        {
            trf->setCoefficient(lt, ct, mat->GetElement(lt, ct));
        }
    }

    auto sig = trf->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }

    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

//------------------------------------------------------------------------------

void BoxWidget::doUpdate()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    vtkBoxRepresentation* repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        vtkMatrix4x4* mat = m_transform->GetMatrix();
        ::fwData::TransformationMatrix3D::sptr transMat = this->getObject< ::fwData::TransformationMatrix3D >();
        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                mat->SetElement(lt, ct, transMat->getCoefficient(lt, ct));
            }
        }

        repr->SetTransform(m_transform);
        this->setVtkPipelineModified();
    }
    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor

#endif
