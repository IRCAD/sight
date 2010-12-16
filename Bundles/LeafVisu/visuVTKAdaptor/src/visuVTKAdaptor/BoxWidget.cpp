
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
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include "visuVTKAdaptor/BoxWidget.hpp"

namespace visuVTKAdaptor
{

#define DELETE_VTKOBJECT( o ) { if( o ) { o->Delete(); o = 0; } }

// MyCallback
//class MyCallback : public ::vtkCommand
//{
//public:
//
//    static MyCallback* New() { return new MyCallback; }
//
//     MyCallback() : mpTransform( ::vtkTransform::New() ) { }
//    ~MyCallback()                                        { DELETE_VTKOBJECT( mpTransform ); }
//
//    virtual void Execute( ::vtkObject* pCaller, unsigned long, void* )
//    {
//        ::vtkBoxWidget* pWidget = reinterpret_cast< ::vtkBoxWidget*>( pCaller );
//        pWidget->GetTransform( mpTransform );
//
//        ::vtkProp3D* pProp3D = pWidget->GetProp3D();
//        if( pProp3D )
//        {
//            ::vtkAssembly* pAssembly = ::vtkAssembly::SafeDownCast( pProp3D );
//            if( pAssembly )
//            {
//                ::vtkProp3DCollection* pParts = pAssembly->GetParts();
//                pParts->InitTraversal();
//
//                while( pProp3D = pParts->GetNextProp3D() )
//                {
//                    pProp3D->SetUserTransform( mpTransform );
//                }
//            }
//        }
//    }
//
//protected:
//
//    ::vtkTransform* mpTransform;
//
//};

// BoxWidget

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, BoxWidget, ::fwData::TransformationMatrix3D );

BoxWidget::BoxWidget() throw()
: ::fwRenderVTK::IVtkAdaptorService(),
  m_vtkBoxWidget( 0 )
{
    addNewHandledEvent( ::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED );
}

BoxWidget::~BoxWidget() throw()
{
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
    boxRep->SetTransform(getTransform());

    //MyCallback* pMyCallback = MyCallback::New();
    //m_vtkBoxWidget->AddObserver( ::vtkCommand::InteractionEvent, pMyCallback );
    //pMyCallback->Delete();
}

void BoxWidget::doStop() throw( ::fwTools::Failed )
{
    //DELETE_VTKOBJECT( m_vtkBoxWidget );
    //if

    unregisterServices();
}

void BoxWidget::doSwap() throw( ::fwTools::Failed )
{
    doUpdate();
}

void BoxWidget::doUpdate() throw( ::fwTools::Failed )
{
    vtkBoxRepresentation *repr = vtkBoxRepresentation::SafeDownCast( m_vtkBoxWidget->GetRepresentation() );
    if( repr )
    {
        repr->SetTransform(getTransform());
    }
}

void BoxWidget::doUpdate( ::fwServices::ObjectMsg::csptr msg ) throw( ::fwTools::Failed )
{
    ::fwComEd::TransformationMatrix3DMsg::csptr transfoMsg = ::fwComEd::TransformationMatrix3DMsg::dynamicConstCast(msg);
    if (transfoMsg && transfoMsg->hasEvent(::fwComEd::TransformationMatrix3DMsg::MATRIX_IS_MODIFIED))
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
