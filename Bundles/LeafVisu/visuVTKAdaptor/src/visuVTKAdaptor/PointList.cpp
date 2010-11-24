/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iterator>
#include <algorithm>
#include <functional>

#include <boost/foreach.hpp>
#include <boost/function.hpp>

#include <fwData/PointList.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwData/Material.hpp>

#include <fwComEd/PointListMsg.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/Factory.hpp>

#include <fwServices/ObjectServiceRegistry.hpp>

#include <vtkCubeSource.h>
#include <vtkActor.h>
#include <vtkPolyDataMapper.h>

#include "visuVTKAdaptor/PointList.hpp"

REGISTER_SERVICE( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::PointList, ::fwData::PointList ) ;

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

PointList::PointList() throw()
{
    addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_ADDED );
    addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_MODIFIED );
    addNewHandledEvent( ::fwComEd::PointListMsg::ELEMENT_REMOVED );
}

//------------------------------------------------------------------------------

PointList::~PointList() throw()
{}

//------------------------------------------------------------------------------

void PointList::configuring() throw(fwTools::Failed)
{

    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    this->setPickerId( m_configuration->getAttributeValue("picker") );
    this->setRenderId( m_configuration->getAttributeValue("renderer") );
}

//------------------------------------------------------------------------------

void PointList::doStart() throw(fwTools::Failed)
{
    m_oldWeakPointList.clear();

    m_weakPointList = this->getWeakPointList();

    this->doUpdate();
}

//------------------------------------------------------------------------------

void PointList::doUpdate() throw(fwTools::Failed)
{
    WeakPointListType points = this->getNewPoints();
    this->createServices( points );
}

//------------------------------------------------------------------------------

void PointList::doUpdate( ::fwServices::ObjectMsg::csptr msg) throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();

    if ( msg->hasEvent( ::fwComEd::PointListMsg::ELEMENT_REMOVED )
         || ( msg->hasEvent( ::fwComEd::PointListMsg::ELEMENT_MODIFIED )) )
    {
        this->doStop();
        this->doUpdate();
        setVtkPipelineModified();
    }
    else if ( msg->hasEvent( ::fwComEd::PointListMsg::ELEMENT_ADDED ))
    {
        m_oldWeakPointList = m_weakPointList;
        m_weakPointList    = this->getWeakPointList();
        this->doUpdate();
        setVtkPipelineModified();
    }
}

//------------------------------------------------------------------------------

void PointList::doSwap() throw(fwTools::Failed)
{
    this->doStop();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void PointList::doStop() throw(fwTools::Failed)
{
    m_oldWeakPointList.clear();
    m_weakPointList.clear();
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void PointList::createServices(WeakPointListType &wPtList)
{

    BOOST_FOREACH( ::fwData::Point::wptr wpt, wPtList )
    {
        SLM_ASSERT("Point Expired", !wpt.expired());

        ::fwData::Point::sptr pt = wpt.lock();
        ::fwRenderVTK::IVtkAdaptorService::sptr service =
            ::fwServices::add< ::fwRenderVTK::IVtkAdaptorService >
                ( pt, "::visuVTKAdaptor::Point" );
        assert(service);

        service->setRenderService(this->getRenderService());
        service->setRenderId( this->getRenderId() );
        service->setPickerId( this->getPickerId() );
        service->start();

        this->registerService(service);
    }
}

//------------------------------------------------------------------------------

PointList::WeakPointListType PointList::getWeakPointList()
{
    ::fwData::PointList::sptr ptList = this->getObject< ::fwData::PointList >();
    WeakPointListType weakList;

    std::copy(ptList->getRefPoints().begin(), ptList->getRefPoints().end(), std::back_inserter(weakList));

    return weakList;
}

//------------------------------------------------------------------------------

PointList::WeakPointListType PointList::getNewPoints()
{
    WeakPointListType newPoints;

//    std::set_difference (
//            m_weakPointList.begin(), m_weakPointList.end(),
//            m_oldWeakPointList.begin(), m_oldWeakPointList.end(),
//            std::back_inserter(newPoints)
//            );
    bool isFound;
    BOOST_FOREACH(::fwData::Point::wptr point, m_weakPointList)
    {
        isFound = false;
        BOOST_FOREACH(::fwData::Point::wptr oldPoint, m_oldWeakPointList)
        {
            isFound = (point.lock() == oldPoint.lock());
            if(isFound)
                break;
        }
        if(!isFound)
        {
            newPoints.push_back(point);
        }
    }
    return newPoints;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
