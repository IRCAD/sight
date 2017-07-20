/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef ANDROID

#include "visuVTKAdaptor/PlaneSelectionNotifier.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Boolean.hpp>
#include <fwData/Plane.hpp>
#include <fwData/PlaneList.hpp>

#include <fwDataTools/helper/Composite.hpp>

#include <fwServices/macros.hpp>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::PlaneSelectionNotifier,
                         ::fwData::Composite );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_PLANELIST_SLOT = "updatePlaneList";
static const ::fwCom::Slots::SlotKeyType s_SELECT_PLANE_SLOT     = "selectPlane";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SELECTION_SLOT = "updateSelection";
static const ::fwCom::Slots::SlotKeyType s_ADD_PLANE_SLOT        = "addPlane";
static const ::fwCom::Slots::SlotKeyType s_REMOVE_PLANE_SLOT     = "removePlane";
static const ::fwCom::Slots::SlotKeyType s_SHOW_PLANES_SLOT      = "showPlanes";

PlaneSelectionNotifier::PlaneSelectionNotifier() noexcept
{
    m_slotUpdatePlaneList = newSlot(s_UPDATE_PLANELIST_SLOT, &PlaneSelectionNotifier::updatePlaneList, this);
    newSlot(s_SELECT_PLANE_SLOT, &PlaneSelectionNotifier::selectPlane, this);
    newSlot(s_UPDATE_SELECTION_SLOT, &PlaneSelectionNotifier::updateSelection, this);
    newSlot(s_ADD_PLANE_SLOT, &PlaneSelectionNotifier::addPlane, this);
    newSlot(s_REMOVE_PLANE_SLOT, &PlaneSelectionNotifier::removePlane, this);
    newSlot(s_SHOW_PLANES_SLOT, &PlaneSelectionNotifier::showPlanes, this);
}

//------------------------------------------------------------------------------

PlaneSelectionNotifier::~PlaneSelectionNotifier() noexcept
{
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doConfigure()
{
    SLM_TRACE_FUNC();
    assert(m_configuration->getName() == "config");
    assert(m_configuration->hasAttribute("planelist"));
    assert(m_configuration->hasAttribute("planeselection"));

    this->setPlaneListId( m_configuration->getAttributeValue("planelist") );
    this->setPlaneSelectionId( m_configuration->getAttributeValue("planeselection") );
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doStart()
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    SLM_ASSERT("m_currentPlaneList should be expired", m_currentPlaneList.expired());

    if (composite->find(m_planeListId) != composite->end())
    {
        m_currentPlaneList = ::fwData::PlaneList::dynamicCast(composite->getContainer()[m_planeListId]);
    }

    ::fwData::PlaneList::sptr planeList = m_currentPlaneList.lock();
    if(planeList)
    {
        m_plConnection.connect(planeList, ::fwData::PlaneList::s_PLANE_REMOVED_SIG,
                               this->getSptr(), s_REMOVE_PLANE_SLOT);
        m_plConnection.connect(planeList, ::fwData::PlaneList::s_VISIBILITY_MODIFIED_SIG,
                               this->getSptr(), s_SHOW_PLANES_SLOT);

        for( ::fwData::Plane::sptr plane :  planeList->getPlanes() )
        {
            m_planeConnections[plane->getID()] = plane->signal(::fwData::Plane::s_SELECTED_SIG)->connect(
                this->slot(s_UPDATE_SELECTION_SLOT));
        }
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doUpdate()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doSwap()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::doStop()
{
    SLM_TRACE_FUNC();

    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwData::PlaneList::sptr planeList = m_currentPlaneList.lock();
    if(planeList)
    {

        for( ::fwData::Plane::sptr plane :  planeList->getPlanes() )
        {
            m_planeConnections[plane->getID()].disconnect();
        }

        m_plConnection.disconnect();

        m_currentPlaneList.reset();
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::addPlane(::fwData::Plane::sptr plane)
{
    //New plane selected is the last one in planeList
    this->selectPlane(plane);

    m_planeConnections[plane->getID()] = plane->signal(::fwData::Plane::s_SELECTED_SIG)->connect(
        this->slot(s_UPDATE_SELECTION_SLOT));
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::removePlane(::fwData::Plane::sptr plane)
{
    //Remove connections
    m_planeConnections[plane->getID()].disconnect();
    this->deselectPlane();
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::showPlanes(bool visible)
{
    if (visible)
    {
        ::fwData::PlaneList::sptr planeList = m_currentPlaneList.lock();

        //New plane selected is the last one in planeList
        ::fwData::Plane::sptr plane = *(planeList->getRefPlanes().rbegin());
        this->selectPlane(plane);
    }
    else
    {
        this->deselectPlane();
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::selectPlane( ::fwData::Object::sptr plane )
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();

    ::fwDataTools::helper::Composite helper(composite);
    helper.swap(m_planeSelectionId, plane);

    auto sig = composite->signal< ::fwData::Composite::ChangedObjectsSignalType >(
        ::fwData::Composite::s_CHANGED_OBJECTS_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdatePlaneList));
        helper.notify();
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::deselectPlane()
{
    ::fwData::Composite::sptr composite = this->getObject< ::fwData::Composite >();
    if ( composite->find(m_planeSelectionId) != composite->end() )
    {
        ::fwDataTools::helper::Composite helper(composite);
        helper.remove(m_planeSelectionId);
        auto sig = composite->signal< ::fwData::Composite::RemovedObjectsSignalType >(
            ::fwData::Composite::s_REMOVED_OBJECTS_SIG);
        {
            ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdatePlaneList));
            helper.notify();
        }
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::updatePlaneList(::fwData::Composite::ContainerType objects)
{
    if ( objects.find(m_planeListId) != objects.end() )
    {
        this->doStop();
        this->doStart();
        this->deselectPlane();
    }
}

//------------------------------------------------------------------------------

void PlaneSelectionNotifier::updateSelection(bool selected)
{
    if (!selected)
    {
        this->deselectPlane();
    }
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType PlaneSelectionNotifier::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Composite::s_ADDED_OBJECTS_SIG, s_UPDATE_PLANELIST_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_CHANGED_OBJECTS_SIG, s_UPDATE_PLANELIST_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Composite::s_REMOVED_OBJECTS_SIG, s_UPDATE_PLANELIST_SLOT ) );

    return connections;
}
//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor

#endif // ANDROID

