/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "opSofa/BoxWidget.hpp"

#include "fwData/Float.hpp"
#include "fwData/Integer.hpp"
#include "fwData/String.hpp"
#include "fwData/Vector.hpp"
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp>

#include <fwServices/IEditionService.hpp>
#include <fwServices/macros.hpp>

#include "visuVTKAdaptor/Transform.hpp"

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

namespace opSofa
{

class BoxClallback : public ::vtkCommand
{
public:

    static BoxClallback* New(::opSofa::BoxWidget* adaptor)
    {
        BoxClallback* cb = new BoxClallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

    BoxClallback() : m_adaptor(NULL)
    {
    }
    ~BoxClallback()
    {
    }

    virtual void Execute( ::vtkObject* pCaller, unsigned long eventId, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::opSofa::BoxWidget* m_adaptor;
};

// BoxWidget

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::opSofa::BoxWidget, ::fwData::TransformationMatrix3D );

BoxWidget::BoxWidget() noexcept
    : ::fwRenderVTK::IVtkAdaptorService(),
      m_vtkBoxWidget( 0 ), m_scaleFactor(1.0), m_enableScaling(true)
{
    m_boxWidgetCommand = BoxClallback::New(this);
}

BoxWidget::~BoxWidget() noexcept
{
}

void BoxWidget::configuring()
{
    setRenderId( m_configuration->getAttributeValue( "renderer" ) );
    this->setTransformId( m_configuration->getAttributeValue("transform") );

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

    m_idMesh = m_configuration->getAttributeValue("idMesh");
}

void BoxWidget::doStart()
{
    m_transform = getTransform();
    SLM_ASSERT("BoxWidget need a vtkTransform", m_transform);
    fwVtkBoxRepresentation* boxRep = fwVtkBoxRepresentation::New();
    boxRep->SetPlaceFactor(m_scaleFactor);

    double bounds[] = {-1,1,-1,1,-1,1};
    boxRep->PlaceWidget(bounds);

    m_vtkBoxWidget = ::vtkBoxWidget2::New();
    m_vtkBoxWidget->SetRepresentation(boxRep);
    m_vtkBoxWidget->SetInteractor( this->getInteractor() );
    if (!m_enableScaling)
    {
        boxRep->ScalingEnabledOff();
//        boxRep->HandlesOff();
    }
    m_vtkBoxWidget->On();

    boxRep->SetTransform(m_transform);

    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

void BoxWidget::doStop()
{
    unregisterServices();

    m_transform->Delete();
    m_transform = 0;
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    m_vtkBoxWidget->Delete();
    m_vtkBoxWidget = 0;

}

void BoxWidget::doSwap()
{
}


void BoxWidget::updateFromVtk()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );

    vtkBoxRepresentation* repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        repr->GetTransform(m_transform);
        m_transform->Modified();
    }

    // Position
    double tab[3];
    m_transform->GetPosition(tab);
    //OSLM_ERROR("position = " << tab[0] << " " << tab[1] << " " << tab[2]);

    // Orientation
    float tab2[3];
    m_transform->GetOrientation(tab2);
    OSLM_ERROR("orientation = " << tab2[0] << " " << tab2[1] << " " << tab2[2]);

    //float tab3[4];
    //m_transform->GetOrientationWXYZ(tab3);
    //OSLM_ERROR("orientationWXYZ = " << tab3[0] << " " << tab3[1] << " " << tab3[2] << " " << tab3[3]);

    // data
    ::fwData::Vector::sptr data = ::fwData::Vector::New();
    ::fwData::String::sptr v1   = ::fwData::String::New(m_idMesh);
    ::fwData::Integer::sptr v2  = ::fwData::Integer::New(tab[0]);
    ::fwData::Integer::sptr v3  = ::fwData::Integer::New(tab[1]);
    ::fwData::Integer::sptr v4  = ::fwData::Integer::New(tab[2]);
    ::fwData::Float::sptr v5    = ::fwData::Float::New(tab2[0]);
    ::fwData::Float::sptr v6    = ::fwData::Float::New(tab2[1]);
    ::fwData::Float::sptr v7    = ::fwData::Float::New(tab2[2]);
    data->getContainer().push_back(v1);
    data->getContainer().push_back(v2);
    data->getContainer().push_back(v3);
    data->getContainer().push_back(v4);
    data->getContainer().push_back(v5);
    data->getContainer().push_back(v6);
    data->getContainer().push_back(v7);

    // Notification
    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    ::fwServices::ObjectMsg::sptr msg2         = ::fwServices::ObjectMsg::New();
    msg2->addEvent("MOVE_MESH_SOFA", data);
    msg2->setSource(this->getSptr());
    msg2->setSubject( trf);
    ::fwData::Object::ObjectModifiedSignalType::sptr sig;
    sig = trf->signal< ::fwData::Object::ObjectModifiedSignalType >(::fwData::Object::s_OBJECT_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotReceive));
        sig->asyncEmit( msg2);
    }


    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

void BoxWidget::doUpdate()
{

}

void BoxWidget::doReceive( ::fwServices::ObjectMsg::csptr msg )
{

}

} // namespace visuVTKAdaptor
