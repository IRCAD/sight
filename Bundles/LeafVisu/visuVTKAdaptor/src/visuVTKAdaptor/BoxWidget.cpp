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

#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwRenderVTK/vtk/fwVtkBoxRepresentation.hpp>

#include "visuVTKAdaptor/Transform.hpp"
#include "visuVTKAdaptor/BoxWidget.hpp"

namespace visuVTKAdaptor
{

class BoxClallback : public ::vtkCommand
{
public:

    static BoxClallback* New(::visuVTKAdaptor::BoxWidget* adaptor) {
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

    ::visuVTKAdaptor::BoxWidget *m_adaptor;
};

// BoxWidget

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, BoxWidget, ::fwData::TransformationMatrix3D );

//------------------------------------------------------------------------------

BoxWidget::BoxWidget() throw()
: ::fwRenderVTK::IVtkAdaptorService(),
  m_vtkBoxWidget( 0 ), m_scaleFactor(1.0), m_enableScaling(true)
{
    m_boxWidgetCommand = BoxClallback::New(this);

    addNewHandledEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
}

//------------------------------------------------------------------------------

BoxWidget::~BoxWidget() throw()
{
}

//------------------------------------------------------------------------------

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
}

//------------------------------------------------------------------------------

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
    }
    m_vtkBoxWidget->On();

    boxRep->SetTransform(m_transform);

    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );

    this->doUpdate();
}

//------------------------------------------------------------------------------

void BoxWidget::doStop() throw( ::fwTools::Failed )
{
    unregisterServices();

    m_transform->Delete();
    m_transform = 0;
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    m_vtkBoxWidget->Delete();
    m_vtkBoxWidget = 0;

}

//------------------------------------------------------------------------------

void BoxWidget::doSwap() throw( ::fwTools::Failed )
{
    doUpdate();
}

//------------------------------------------------------------------------------

void BoxWidget::updateFromVtk()
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );

    vtkBoxRepresentation *repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        repr->GetTransform(m_transform);
        m_transform->Modified();
    }

    ::fwData::TransformationMatrix3D::sptr trf = this->getObject< ::fwData::TransformationMatrix3D >();
    vtkMatrix4x4* mat = m_transform->GetMatrix();

    for(int lt=0; lt<4; lt++)
    {
        for(int ct=0; ct<4; ct++)
        {
            trf->setCoefficient(lt,ct, mat->GetElement(lt,ct));
        }
    }

    ::fwComEd::TransformationMatrix3DMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), trf, msg);


    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

//------------------------------------------------------------------------------

void BoxWidget::doUpdate() throw( ::fwTools::Failed )
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    vtkBoxRepresentation *repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        vtkMatrix4x4* mat = m_transform->GetMatrix();
        ::fwData::TransformationMatrix3D::sptr transMat =
                    this->getObject< ::fwData::TransformationMatrix3D >();
        for(int lt=0; lt<4; lt++)
        {
            for(int ct=0; ct<4; ct++)
            {
                mat->SetElement(lt, ct, transMat->getCoefficient(lt,ct));
            }
        }

        repr->SetTransform(m_transform);
        this->setVtkPipelineModified();
    }
    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

//------------------------------------------------------------------------------

void BoxWidget::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed )
{
    ::fwComEd::TransformationMatrix3DMsg::csptr transfoMsg = ::fwComEd::TransformationMatrix3DMsg::dynamicConstCast(msg);
    if (transfoMsg &&transfoMsg->hasEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED) &&  m_vtkBoxWidget->HasObserver(::vtkCommand::InteractionEvent, m_boxWidgetCommand))
    {
        doUpdate();
    }
}

//------------------------------------------------------------------------------

} // namespace visuVTKAdaptor
