/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwMath/IntrasecTypes.hpp>
#include <fwMath/PlaneFunctions.hpp>

#include <fwData/Plane.hpp>
#include <fwData/Color.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwComEd/PointMsg.hpp>
#include <fwComEd/PlaneMsg.hpp>

#include <vtkActor.h>
#include <vtkPlaneCollection.h>
#include <vtkPolyDataMapper.h>
#include <vtkPropCollection.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>


#include "fwRenderVTK/vtk/Helpers.hpp"
#include "fwRenderVTK/vtk/MarkedSphereHandleRepresentation.hpp"
#include "visuVTKAdaptor/Plane.hpp"
#include "visuVTKAdaptor/Point.hpp"


REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Plane, ::fwData::Plane ) ;

namespace visuVTKAdaptor
{



Plane::Plane() throw()
{
    SLM_TRACE_FUNC();

    m_vtkPlaneCollection = 0;
    m_vtkImplicitPlane = 0;

    addNewHandledEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED );
    addNewHandledEvent( ::fwComEd::PointMsg::START_POINT_INTERACTION );
    addNewHandledEvent( ::fwComEd::PlaneMsg::PLANE_MODIFIED );
    addNewHandledEvent( ::fwComEd::PlaneMsg::WAS_SELECTED );
    addNewHandledEvent( ::fwComEd::PlaneMsg::WAS_DESELECTED );

}

//------------------------------------------------------------------------------

Plane::~Plane() throw()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void Plane::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void Plane::doStart() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

//  vtkPolyDataMapper *mapper = vtkPolyDataMapper::New();
//  mapper->SetInput( m_vtkPlane->GetOutput() );
//  m_actorPlan->SetMapper(mapper);
//  m_actorPlan->GetProperty()->SetOpacity( 0.5 );
//  m_actorPlan->GetProperty()->SetColor( 1., 1., 1. );
//  this->addToRenderer(m_actorPlan);
//
//  mapper->Delete();
//  mapper = 0;
//
//  vtkMaskPoints* ptMask = vtkMaskPoints::New();
//  ptMask->SetInputConnection(m_vtkPlane->GetOutputPort());
//  ptMask->SetOnRatio(1);
//  //ptMask->RandomModeOn();
//
//  vtkArrowSource* arrow = vtkArrowSource::New();
//
//  vtkGlyph3D* glyph = vtkGlyph3D::New();
//  glyph->SetInputConnection(ptMask->GetOutputPort());
//  glyph->SetSource(arrow->GetOutput());
//  glyph->SetVectorModeToUseNormal();
//  glyph->SetScaleModeToScaleByVector();
//  glyph->SetScaleFactor(50.0);
//
//  vtkPolyDataMapper* glyphMapper = vtkPolyDataMapper::New();
//  glyphMapper->SetInputConnection(glyph->GetOutputPort());
//
//  vtkActor *arrowsActor = vtkActor::New();
//    arrowsActor->SetMapper(glyphMapper);
//  arrowsActor->GetProperty()->SetOpacity( 0.5 );
//  this->addToRenderer(arrowsActor);
//
//  arrowsActor->Delete();
//  ptMask->Delete();
//  glyph->Delete();
//  glyphMapper->Delete();
//  arrow->Delete();

    m_pPlane = this->getObject< ::fwData::Plane >();

    BOOST_FOREACH( ::fwData::Point::sptr point, m_pPlane.lock()->getPoints() )
    {
        ::fwRenderVTK::IVtkAdaptorService::sptr servicePoint =
                ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
        ( point, "::visuVTKAdaptor::Point" );
        SLM_ASSERT("servicePoint not instanced", servicePoint);

        servicePoint->setRenderService(this->getRenderService());
        servicePoint->setRenderId( this->getRenderId() );
        servicePoint->setPickerId( this->getPickerId() );
        servicePoint->start();

        this->registerService(servicePoint);

        ::fwServices::registerCommunicationChannel(point, this->getSptr() )->start();
    }

    if (m_vtkPlaneCollection)
    {
        m_vtkImplicitPlane = vtkPlane::New();
        m_vtkPlaneCollection->AddItem(m_vtkImplicitPlane);
    }

    setVtkPipelineModified();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Plane::doSwap() throw(fwTools::Failed)
{
    SLM_TRACE("SWAPPING Plane");
    m_pPlane = this->getObject< ::fwData::Plane >();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void Plane::doUpdate() throw(fwTools::Failed)
{
    assert(!m_pPlane.expired());
    ::fwData::Plane::sptr plane = m_pPlane.lock();

    ::fwData::Point::sptr pt0 = plane->getPoints()[0];
    ::fwData::Point::sptr pt1 = plane->getPoints()[1];
    ::fwData::Point::sptr pt2 = plane->getPoints()[2];
    plane->computePlaneFromPoints();

    if(m_vtkImplicitPlane)
    {
        fwVec3d normal = ::fwMath::getNormal(plane->getPlane());

        m_vtkImplicitPlane->SetOrigin(pt0->getCoord()[0], pt0->getCoord()[1], pt0->getCoord()[2]);
        m_vtkImplicitPlane->SetNormal(normal[0], normal[1], normal[2]);
        m_vtkImplicitPlane->Modified();
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Plane::doUpdate( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwComEd::PointMsg::csptr pointMsg = ::fwComEd::PointMsg::dynamicConstCast( _msg );
    ::fwComEd::PlaneMsg::csptr planeMsg = ::fwComEd::PlaneMsg::dynamicConstCast( _msg );
    if ( pointMsg)
    {
        if (pointMsg->hasEvent( ::fwComEd::PointMsg::POINT_IS_MODIFIED ) )
        {
            this->doUpdate();
            ::fwComEd::PlaneMsg::NewSptr msg;
            msg->addEvent( ::fwComEd::PlaneMsg::PLANE_MODIFIED );
            ::fwServices::IEditionService::notify( this->getSptr(), m_pPlane.lock(), msg );
        }

        if ( pointMsg->hasEvent( ::fwComEd::PointMsg::START_POINT_INTERACTION ))
        {
            ::fwComEd::PlaneMsg::NewSptr msg;
            msg->addEvent( ::fwComEd::PlaneMsg::START_PLANE_INTERACTION );
            ::fwServices::IEditionService::notify( this->getSptr(), m_pPlane.lock(), msg );
        }
    }
    else if (planeMsg)
    {
        if (planeMsg->hasEvent( ::fwComEd::PlaneMsg::PLANE_MODIFIED ) )
        {
            this->doUpdate();
        }
        else  if (planeMsg->hasEvent( ::fwComEd::PlaneMsg::WAS_SELECTED ) )
        {
            // highlight selected plan
            this->selectPlane(true);
            setVtkPipelineModified();
        }
        else  if (planeMsg->hasEvent( ::fwComEd::PlaneMsg::WAS_DESELECTED ) )
        {
            this->selectPlane(false);
            setVtkPipelineModified();
        }
    }
}

//------------------------------------------------------------------------------

void Plane::doStop() throw(fwTools::Failed)
{
    if (m_vtkPlaneCollection && m_vtkImplicitPlane)
    {
        m_vtkPlaneCollection->RemoveItem(m_vtkImplicitPlane);
        m_vtkImplicitPlane->Delete();
        m_vtkImplicitPlane = 0;
    }

    if (!m_pPlane.expired())
    {
        BOOST_FOREACH( ::fwData::Point::sptr point, m_pPlane.lock()->getPoints() )
        {
            ::fwServices::unregisterCommunicationChannel(point, this->getSptr() );
        }
    }

    this->unregisterServices();
    this->removeAllPropFromRenderer();
}

//------------------------------------------------------------------------------

void Plane::setVtkPlaneCollection( vtkObject * col )
{
    if (m_vtkPlaneCollection != col)
    {
        if (m_vtkPlaneCollection)
        {
            if (m_vtkImplicitPlane)
            {
                m_vtkPlaneCollection->RemoveItem(m_vtkImplicitPlane);
            }
            m_vtkPlaneCollection=0;
        }

        if ( col )
        {
            m_vtkPlaneCollection = vtkPlaneCollection::SafeDownCast(col);
            if (m_vtkImplicitPlane)
            {
                m_vtkPlaneCollection->AddItem(m_vtkImplicitPlane);
            }
        }
    }
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Plane::selectPlane(bool select)
{
    BOOST_FOREACH( ServiceVector::value_type service, m_subServices )
    {
        if(!service.expired())
        {
            ::visuVTKAdaptor::Point::sptr servicePoint = ::visuVTKAdaptor::Point::dynamicCast(service.lock());
            if(select)
            {
                servicePoint->setColor(1.,0.,0.);
            }
            else
            {
                servicePoint->setColor(1.,1.,1.);
            }
        }
    }



}


} //namespace visuVTKAdaptor
