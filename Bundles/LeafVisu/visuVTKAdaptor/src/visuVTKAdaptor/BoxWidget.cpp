
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

#define DELETE_VTKOBJECT( o ) { if( o ) { o->Delete(); o = 0; } }

    
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
  m_vtkBoxWidget( 0 )
{
    m_transform = vtkTransform::New();
    m_boxWidgetCommand = BoxClallback::New(this);

    addNewHandledEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
}

BoxWidget::~BoxWidget() throw()
{
    m_transform->Delete();
    m_transform = 0;

}

void BoxWidget::configuring() throw( ::fwTools::Failed )
{
    setRenderId( m_configuration->getAttributeValue( "renderer" ) );
    this->setTransformId( m_configuration->getAttributeValue("transform") );
}

void BoxWidget::doStart() throw( ::fwTools::Failed )
{
    vtkBoxRepresentation *boxRep = vtkBoxRepresentation::New();
    m_vtkBoxWidget = ::vtkBoxWidget2::New();
    m_vtkBoxWidget->SetRepresentation(boxRep);
    m_vtkBoxWidget->SetInteractor( this->getInteractor() );
    m_vtkBoxWidget->On();

    m_transformService = ::visuVTKAdaptor::Transform::dynamicCast(
        ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService > (
                this->getObject(),
                "::visuVTKAdaptor::Transform" ));
    assert(m_transformService.lock());
    ::visuVTKAdaptor::Transform::sptr transformService = m_transformService.lock();


    transformService->setRenderService ( this->getRenderService()  );
    transformService->setRenderId      ( this->getRenderId()       );


    transformService->setTransform(m_transform);

    boxRep->SetTransform(m_transform);

    this->registerService(transformService);
    transformService->start();


    m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, m_boxWidgetCommand );
    //MyCallback* pMyCallback = MyCallback::New();
    //m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, pMyCallback );
    //pMyCallback->Delete();
}

void BoxWidget::doStop() throw( ::fwTools::Failed )
{
    unregisterServices();
}

void BoxWidget::doSwap() throw( ::fwTools::Failed )
{
    doUpdate();
}


void BoxWidget::updateFromVtk() throw( ::fwTools::Failed )
{
    m_vtkBoxWidget->RemoveObserver( m_boxWidgetCommand );
    vtkBoxRepresentation *repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        repr->GetTransform(m_transform);
        m_transform->Modified();
    }
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
        OSLM_ERROR("GOT A MESSAGE " << getTransformId() );
        doUpdate();
    }
    //::fwComEd::AcquisitionMsg::csptr pMessage = ::fwComEd::AcquisitionMsg::dynamicConstCast( msg );

    //if( pMessage )
    /*{
        if( pMessage->hasEvent( "Widget" ) )
        {
            doUpdate();
        }*/
    //}
}

} // namespace visuVTKAdaptor
