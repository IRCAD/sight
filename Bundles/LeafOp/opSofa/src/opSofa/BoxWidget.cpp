#include <boost/lexical_cast.hpp>

#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkMatrix4x4.h>
#include <vtkTransform.h>
#include <vtkBoxRepresentation.h>
#include <vtkBoxWidget2.h>

#include <fwComEd/TransformationMatrix3DMsg.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwServices/Base.hpp>

//#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/registry/ObjectService.hpp> // new
#include <fwServices/IEditionService.hpp> // new

#include <fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp>

#include "visuVTKAdaptor/Transform.hpp"
#include "opSofa/BoxWidget.hpp"

#include "fwData/Integer.hpp"
#include "fwData/Vector.hpp"
#include "fwData/Float.hpp"
#include "fwData/String.hpp"

namespace opSofa
{

class BoxClallback : public ::vtkCommand
{
public:

    static BoxClallback* New(::opSofa::BoxWidget* adaptor) {
        BoxClallback *cb = new BoxClallback;
        cb->m_adaptor = adaptor;
        return cb;
    }

     BoxClallback() {}
    ~BoxClallback() {}

    virtual void Execute( ::vtkObject* pCaller, unsigned long eventId, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::opSofa::BoxWidget *m_adaptor;
};

// BoxWidget

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, BoxWidget, ::fwData::TransformationMatrix3D );

BoxWidget::BoxWidget() throw()
: ::fwRenderVTK::IVtkAdaptorService(),
  m_vtkBoxWidget( 0 ), m_scaleFactor(1.0), m_enableScaling(true)
{
    m_boxWidgetCommand = BoxClallback::New(this);

    //addNewHandledEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
}

BoxWidget::~BoxWidget() throw()
{
}

void BoxWidget::configuring() throw( ::fwTools::Failed )
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

void BoxWidget::doStart() throw( ::fwTools::Failed )
{
    m_transform = getTransform();
    SLM_ASSERT("BoxWidget need a vtkTransform", m_transform);
    fwVtkBoxRepresentation *boxRep = fwVtkBoxRepresentation::New();
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

void BoxWidget::doStop() throw( ::fwTools::Failed )
{
    unregisterServices();

    m_transform->Delete();
    m_transform = 0;
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    m_vtkBoxWidget->Delete();
    m_vtkBoxWidget = 0;

}

void BoxWidget::doSwap() throw( ::fwTools::Failed )
{
}


void BoxWidget::updateFromVtk()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );

    vtkBoxRepresentation *repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
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
    ::fwData::Vector::NewSptr data;
    ::fwData::String::NewSptr v1(m_idMesh);
    ::fwData::Integer::NewSptr v2(tab[0]);
    ::fwData::Integer::NewSptr v3(tab[1]);
    ::fwData::Integer::NewSptr v4(tab[2]);
    ::fwData::Float::NewSptr v5(tab2[0]);
    ::fwData::Float::NewSptr v6(tab2[1]);
    ::fwData::Float::NewSptr v7(tab2[2]);
    data->getContainer().push_back(v1);
    data->getContainer().push_back(v2);
    data->getContainer().push_back(v3);
    data->getContainer().push_back(v4);
    data->getContainer().push_back(v5);
    data->getContainer().push_back(v6);
    data->getContainer().push_back(v7);

    // Notification
    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    ::fwServices::ObjectMsg::NewSptr msg2;
    msg2->addEvent("MOVE_MESH_SOFA", data);
    ::fwServices::IEditionService::notify(this->getSptr(), trf, msg2);


    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

void BoxWidget::doUpdate() throw( ::fwTools::Failed )
{
   
}

void BoxWidget::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed )
{
  
}

} // namespace visuVTKAdaptor
