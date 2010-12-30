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

    virtual void Execute( ::vtkObject* pCaller, unsigned long, void* )
    {
        m_adaptor->updateFromVtk();
    }

    ::visuVTKAdaptor::BoxWidget *m_adaptor;
};

// BoxWidget

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, BoxWidget, ::fwData::TransformationMatrix3D );

BoxWidget::BoxWidget() throw()
: ::fwRenderVTK::IVtkAdaptorService(),
  m_vtkBoxWidget( 0 ), m_scale(1.0)
{
    //m_transform = vtkTransform::New();
    m_boxWidgetCommand = BoxClallback::New(this);

    addNewHandledEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
}

BoxWidget::~BoxWidget() throw()
{
    //m_transform->Delete();
    //m_transform = 0;
}

void BoxWidget::configuring() throw( ::fwTools::Failed )
{
    setRenderId( m_configuration->getAttributeValue( "renderer" ) );
    this->setTransformId( m_configuration->getAttributeValue("transform") );

    if (m_configuration->hasAttribute("scale"))
    {
        m_scale = ::boost::lexical_cast<double>(m_configuration->getAttributeValue("scale"));
    }
}

void BoxWidget::doStart() throw( ::fwTools::Failed )
{
    m_transform = getTransform();
    SLM_ASSERT("BoxWidget need a vtkTransform", m_transform);
    vtkBoxRepresentation *boxRep = vtkBoxRepresentation::New();
    boxRep->SetPlaceFactor(m_scale);

    double bounds[] = {-1,1,-1,1,-1,1};
    boxRep->PlaceWidget(bounds);

    m_vtkBoxWidget = ::vtkBoxWidget2::New();
    m_vtkBoxWidget->SetRepresentation(boxRep);
    m_vtkBoxWidget->SetInteractor( this->getInteractor() );
    m_vtkBoxWidget->On();

    //m_transformService = ::visuVTKAdaptor::Transform::dynamicCast(
        //::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
                //this->getObject(),
                //"::visuVTKAdaptor::Transform" ));
    //assert(m_transformService.lock());
    //::visuVTKAdaptor::Transform::sptr transformService = m_transformService.lock();


    //transformService->setRenderService ( this->getRenderService()  );
    //transformService->setRenderId      ( this->getRenderId()       );

    //transformService->setTransform(m_transform);

    boxRep->SetTransform(m_transform);

    //this->registerService(transformService);
    //transformService->start();


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
    doUpdate();
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

void BoxWidget::doUpdate() throw( ::fwTools::Failed )
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    vtkBoxRepresentation *repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        repr->SetTransform(m_transform);
    }
    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
}

void BoxWidget::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed )
{
    ::fwComEd::TransformationMatrix3DMsg::csptr transfoMsg = ::fwComEd::TransformationMatrix3DMsg::dynamicConstCast(msg);
    if (transfoMsg &&transfoMsg->hasEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED) &&  m_vtkBoxWidget->HasObserver(::vtkCommand::InteractionEvent, m_boxWidgetCommand))
    {
        doUpdate();
    }
}

} // namespace visuVTKAdaptor
