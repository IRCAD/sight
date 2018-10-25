/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SPointList.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Material.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <boost/function.hpp>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>

#include <algorithm>
#include <functional>
#include <iterator>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SPointList);

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_ADD_POINT_SLOT     = "addPoint";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SPLINE_SLOT = "updateSpline";
const ::fwCom::Slots::SlotKeyType s_UPDATE_VISIBILITY_SLOT    = "updateVisibility";

const ::fwServices::IService::KeyType SPointList::s_POINTLIST_INPUT = "pointList";

//------------------------------------------------------------------------------

SPointList::SPointList() noexcept :
    m_radius(7.0),
    m_interaction(true)
{
    newSlot(s_ADD_POINT_SLOT, &SPointList::addPoint, this);
    newSlot(s_UPDATE_SPLINE_SLOT, &SPointList::updateSpline, this);
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SPointList::updateVisibility, this);

    m_ptColor = ::fwData::Color::New();
}

//------------------------------------------------------------------------------

SPointList::~SPointList() noexcept
{
}
//------------------------------------------------------------------------------

void SPointList::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string hexaColor = config.get<std::string>("color", "");
    m_ptColor = ::fwData::Color::New();
    if (!hexaColor.empty())
    {
        m_ptColor->setRGBA(hexaColor);
    }

    const std::string radius = config.get<std::string>("radius", "");
    if(!radius.empty())
    {
        m_radius = std::stod(radius);
    }

    const std::string interaction = config.get<std::string>("interaction", "");

    if (!interaction.empty())
    {
        SLM_FATAL_IF("value for 'interaction' must be 'on' or 'off', actual: " + interaction,
                     interaction != "on" && interaction != "off");

        m_interaction = (interaction == "on");
    }
}

//------------------------------------------------------------------------------

void SPointList::starting()
{
    this->initialize();

    m_oldWeakPointList.clear();

    m_weakPointList = this->getWeakPointList();

    this->updating();
}

//------------------------------------------------------------------------------

void SPointList::updating()
{
    WeakPointListType points = this->getNewPoints();
    this->createServices( points );
    this->requestRender();
}

//----------------------------------------------------------------------------------------------------------------

void SPointList::addPoint(::fwData::Point::sptr /*point*/)
{
    m_oldWeakPointList = m_weakPointList;
    m_weakPointList    = this->getWeakPointList();
    this->setVtkPipelineModified();
    this->updating();
}

//----------------------------------------------------------------------------------------------------------------

void SPointList::updateSpline()
{
    this->stopping();
    this->updating();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SPointList::stopping()
{
    m_oldWeakPointList.clear();
    m_weakPointList.clear();
    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SPointList::createServices(WeakPointListType& wPtList)
{
    for( ::fwData::Point::wptr wpt :  wPtList )
    {
        SLM_ASSERT("Point Expired", !wpt.expired());

        ::fwData::Point::sptr pt = wpt.lock();

        // create the srv configuration for objects auto-connection
        auto pointAdaptor = this->registerService< ::visuVTKAdaptor::SPoint >("::visuVTKAdaptor::SPoint");
        pointAdaptor->registerInOut(pt, SPoint::s_POINT_INOUT, true);

        SLM_ASSERT("Bad cast of IAdaptor to Point", pointAdaptor);

        pointAdaptor->setRenderService(this->getRenderService());
        pointAdaptor->setRendererId( this->getRendererId() );
        pointAdaptor->setPickerId( this->getPickerId() );
        pointAdaptor->start();

        pointAdaptor->setColor(m_ptColor->red(), m_ptColor->green(), m_ptColor->blue(), m_ptColor->alpha());
        pointAdaptor->setRadius(m_radius);
        pointAdaptor->setInteraction(m_interaction);
    }
}

//------------------------------------------------------------------------------

SPointList::WeakPointListType SPointList::getWeakPointList()
{
    ::fwData::PointList::csptr ptList = this->getInput< ::fwData::PointList >(s_POINTLIST_INPUT);
    SLM_ASSERT("'pointList' is not defined.", ptList);

    WeakPointListType weakList;

    std::copy(ptList->getPoints().begin(), ptList->getPoints().end(), std::back_inserter(weakList));

    return weakList;
}

//------------------------------------------------------------------------------

SPointList::WeakPointListType SPointList::getNewPoints()
{
    WeakPointListType newPoints;

    bool isFound;
    for(::fwData::Point::wptr point :  m_weakPointList)
    {
        isFound = false;
        for(::fwData::Point::wptr oldPoint :  m_oldWeakPointList)
        {
            isFound = (point.lock() == oldPoint.lock());
            if(isFound)
            {
                break;
            }
        }
        if(!isFound)
        {
            newPoints.push_back(point);
        }
    }
    return newPoints;
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SPointList::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_POINTLIST_INPUT, ::fwData::PointList::s_MODIFIED_SIG, s_UPDATE_SPLINE_SLOT);
    connections.push(s_POINTLIST_INPUT, ::fwData::PointList::s_POINT_ADDED_SIG, s_ADD_POINT_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

void SPointList::setRadius(const double radius)
{
    m_radius = radius;
}

//------------------------------------------------------------------------------

void SPointList::setColor(const fwData::Color::sptr ptColor)
{
    m_ptColor = ptColor;
}

//------------------------------------------------------------------------------

void SPointList::setInteraction(const bool interaction)
{
    m_interaction = interaction;
}

//------------------------------------------------------------------------------

void SPointList::updateVisibility( bool isVisible)
{
    const auto& services = this->getRegisteredServices();
    for(const auto& service : services)
    {
        auto srv = ::visuVTKAdaptor::SPoint::dynamicCast(service.lock());
        if(srv)
        {
            srv->updateVisibility(isVisible);
        }
        else
        {
            SLM_WARN("::visuVTKAdaptor::SPoint cast failed");
        }
    }
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
