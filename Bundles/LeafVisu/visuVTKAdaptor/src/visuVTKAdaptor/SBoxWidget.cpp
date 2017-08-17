/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/SBoxWidget.hpp"

#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp>

#include <fwServices/macros.hpp>

#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

namespace visuVTKAdaptor
{

class BoxClallback : public ::vtkCommand
{
public:

    //------------------------------------------------------------------------------

    static BoxClallback* New(::visuVTKAdaptor::SBoxWidget* adaptor)
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

    virtual void Execute( ::vtkObject*, unsigned long, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::SBoxWidget* m_adaptor;
};

// SBoxWidget

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SBoxWidget);

static const ::fwServices::IService::KeyType s_TRANSFORM_INOUT = "transform";

//------------------------------------------------------------------------------

SBoxWidget::SBoxWidget() noexcept :
    ::fwRenderVTK::IAdaptor(),
    m_transform(nullptr),
    m_vtkBoxWidget(nullptr),
    m_boxWidgetCommand(nullptr),
    m_scaleFactor(1.0),
    m_enableScaling(true)
{
    m_boxWidgetCommand = BoxClallback::New(this);
}

//------------------------------------------------------------------------------

SBoxWidget::~SBoxWidget() noexcept
{
}

//------------------------------------------------------------------------------

void SBoxWidget::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    m_scaleFactor = config.get("scaleFactor", 1.);

    const std::string enableScaling = config.get("enableScaling", "no");

    SLM_ASSERT("Wrong value for 'enableScaling', must be 'yes' or 'no'",
               enableScaling == "yes" || enableScaling == "no");

    m_enableScaling = (enableScaling == "yes");
}

//------------------------------------------------------------------------------

void SBoxWidget::starting()
{
    this->initialize();

    SLM_ASSERT("vtk transform must be defined.", !this->getTransformId().empty());
    m_transform = this->getRenderService()->getOrAddVtkTransform(this->getTransformId());

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

    this->updating();
}

//------------------------------------------------------------------------------

void SBoxWidget::stopping()
{
    m_transform->Delete();
    m_transform = 0;
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    m_vtkBoxWidget->Delete();
    m_vtkBoxWidget = 0;
    this->requestRender();
}

//------------------------------------------------------------------------------

void SBoxWidget::updateFromVtk()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );

    vtkBoxRepresentation* repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        repr->GetTransform(m_transform);
        m_transform->Modified();
    }

    ::fwData::TransformationMatrix3D::sptr trf = this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);
    vtkMatrix4x4* mat = m_transform->GetMatrix();

    for(int lt = 0; lt < 4; lt++)
    {
        for(int ct = 0; ct < 4; ct++)
        {
            trf->setCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct), mat->GetElement(lt, ct));
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

void SBoxWidget::updating()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    vtkBoxRepresentation* repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        ::fwData::TransformationMatrix3D::sptr transMat =
            this->getInOut< ::fwData::TransformationMatrix3D >(s_TRANSFORM_INOUT);
        vtkMatrix4x4* mat = m_transform->GetMatrix();
        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                mat->SetElement(lt, ct, transMat->getCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct)));
            }
        }

        repr->SetTransform(m_transform);
        this->setVtkPipelineModified();
    }
    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SBoxWidget::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_TRANSFORM_INOUT, ::fwData::TransformationMatrix3D::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor

#endif
