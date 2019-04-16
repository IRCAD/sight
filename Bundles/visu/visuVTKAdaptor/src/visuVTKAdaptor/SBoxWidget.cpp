/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

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

    ::fwRenderVTK::vtk::fwVtkBoxRepresentation* boxRep = ::fwRenderVTK::vtk::fwVtkBoxRepresentation::New();
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
    m_transform = nullptr;
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    m_vtkBoxWidget->Delete();
    m_vtkBoxWidget = nullptr;
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

        vtkMatrix4x4* mat = vtkMatrix4x4::New();

        for(int lt = 0; lt < 4; lt++)
        {
            for(int ct = 0; ct < 4; ct++)
            {
                mat->SetElement(lt, ct, transMat->getCoefficient(static_cast<size_t>(lt), static_cast<size_t>(ct)));
            }
        }
        m_transform->SetMatrix(mat);
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
