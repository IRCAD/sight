/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "ctrlSelection/wrapper/SImageSignalForwarder.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <regex>

namespace ctrlSelection
{
namespace wrapper
{

static const ::fwCom::Slots::SlotKeyType s_FORWARD_MODIFIED_SLOT              = "forwardModified";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_BUFFER_MODIFIED_SLOT       = "forwardBufferModified";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_LANDMARK_ADDED_SLOT        = "forwardLandmarkAdded";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_LANDMARK_REMOVED_SLOT      = "forwardLandmarkRemoved";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_LANDMARK_DISPLAYED_SLOT    = "forwardLandmarkDisplayed";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_DISTANCE_ADDED_SLOT        = "forwardDistanceAdded";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_DISTANCE_REMOVED_SLOT      = "forwardDistanceRemoved";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_DISTANCE_DISPLAYED_SLOT    = "forwardDistanceDisplayed";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_SLICE_INDEX_MODIFIED_SLOT  = "forwardSliceIndexModified";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_SLICE_TYPE_MODIFIED_SLOT   = "forwardSliceTypeModified";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_VISIBILITY_MODIFIED_SLOT   = "forwardVisibilityModified";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_TRANSPARENCY_MODIFIED_SLOT = "forwardTransparencyModified";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_ADDED_FIELDS_SLOT          = "forwardAddedFields";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_CHANGED_FIELDS_SLOT        = "forwardChangedFields";
static const ::fwCom::Slots::SlotKeyType s_FORWARD_REMOVED_FIELDS_SLOT        = "forwardRemovedFields";

static const ::fwServices::IService::KeyType s_SOURCE_INPUT = "source";
static const ::fwServices::IService::KeyType s_TARGET_INPUT = "target";

SImageSignalForwarder::AvailableConnectionMapType SImageSignalForwarder::m_availableConnection;

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IWrapperSrv, ::ctrlSelection::wrapper::SImageSignalForwarder,
                         ::fwData::Image );

//-----------------------------------------------------------------------------

SImageSignalForwarder::SImageSignalForwarder() noexcept
{
    m_availableConnection[::fwData::Image::s_MODIFIED_SIG]              = s_FORWARD_MODIFIED_SLOT;
    m_availableConnection[::fwData::Image::s_BUFFER_MODIFIED_SIG]       = s_FORWARD_BUFFER_MODIFIED_SLOT;
    m_availableConnection[::fwData::Image::s_LANDMARK_ADDED_SIG]        = s_FORWARD_LANDMARK_ADDED_SLOT;
    m_availableConnection[::fwData::Image::s_LANDMARK_REMOVED_SIG]      = s_FORWARD_LANDMARK_REMOVED_SLOT;
    m_availableConnection[::fwData::Image::s_LANDMARK_DISPLAYED_SIG]    = s_FORWARD_LANDMARK_DISPLAYED_SLOT;
    m_availableConnection[::fwData::Image::s_DISTANCE_ADDED_SIG]        = s_FORWARD_DISTANCE_ADDED_SLOT;
    m_availableConnection[::fwData::Image::s_DISTANCE_REMOVED_SIG]      = s_FORWARD_DISTANCE_REMOVED_SLOT;
    m_availableConnection[::fwData::Image::s_DISTANCE_DISPLAYED_SIG]    = s_FORWARD_DISTANCE_DISPLAYED_SLOT;
    m_availableConnection[::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG]  = s_FORWARD_SLICE_INDEX_MODIFIED_SLOT;
    m_availableConnection[::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG]   = s_FORWARD_SLICE_TYPE_MODIFIED_SLOT;
    m_availableConnection[::fwData::Image::s_VISIBILITY_MODIFIED_SIG]   = s_FORWARD_VISIBILITY_MODIFIED_SLOT;
    m_availableConnection[::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG] = s_FORWARD_TRANSPARENCY_MODIFIED_SLOT;
    m_availableConnection[::fwData::Object::s_ADDED_FIELDS_SIG]         = s_FORWARD_ADDED_FIELDS_SLOT;
    m_availableConnection[::fwData::Object::s_CHANGED_FIELDS_SIG]       = s_FORWARD_CHANGED_FIELDS_SLOT;
    m_availableConnection[::fwData::Object::s_REMOVED_FIELDS_SIG]       = s_FORWARD_REMOVED_FIELDS_SLOT;

    newSlot(s_FORWARD_MODIFIED_SLOT, &SImageSignalForwarder::forwardModified, this);
    newSlot(s_FORWARD_BUFFER_MODIFIED_SLOT, &SImageSignalForwarder::forwardBufferModified, this);
    newSlot(s_FORWARD_LANDMARK_ADDED_SLOT, &SImageSignalForwarder::forwardLandmarkAdded, this);
    newSlot(s_FORWARD_LANDMARK_REMOVED_SLOT, &SImageSignalForwarder::forwardLandmarkRemoved, this);
    newSlot(s_FORWARD_LANDMARK_DISPLAYED_SLOT, &SImageSignalForwarder::forwardLandmarkDisplayed, this);
    newSlot(s_FORWARD_DISTANCE_ADDED_SLOT, &SImageSignalForwarder::forwardDistanceAdded, this);
    newSlot(s_FORWARD_DISTANCE_REMOVED_SLOT, &SImageSignalForwarder::forwardDistanceRemoved, this);
    newSlot(s_FORWARD_DISTANCE_DISPLAYED_SLOT, &SImageSignalForwarder::forwardDistanceDisplayed, this);
    newSlot(s_FORWARD_SLICE_INDEX_MODIFIED_SLOT, &SImageSignalForwarder::forwardSliceIndexModified, this);
    newSlot(s_FORWARD_SLICE_TYPE_MODIFIED_SLOT, &SImageSignalForwarder::forwardSliceTypeModified, this);
    newSlot(s_FORWARD_VISIBILITY_MODIFIED_SLOT, &SImageSignalForwarder::forwardVisibilityModified, this);
    newSlot(s_FORWARD_TRANSPARENCY_MODIFIED_SLOT, &SImageSignalForwarder::forwardTransparencyModified, this);
    newSlot(s_FORWARD_ADDED_FIELDS_SLOT, &SImageSignalForwarder::forwardAddedFields, this);
    newSlot(s_FORWARD_CHANGED_FIELDS_SLOT, &SImageSignalForwarder::forwardChangedFields, this);
    newSlot(s_FORWARD_REMOVED_FIELDS_SLOT, &SImageSignalForwarder::forwardRemovedFields, this);
}

//-----------------------------------------------------------------------------

SImageSignalForwarder::~SImageSignalForwarder() noexcept
{
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::configuring()
{
    std::vector< ::fwRuntime::ConfigurationElement::sptr > configs = m_configuration->find("forward");

    for (auto cfg : configs)
    {
        std::string signal = cfg->getValue();
        SLM_ASSERT("Signal '" + signal + "' is unknown.",
                   m_availableConnection.find(signal) != m_availableConnection.end());
        m_managedSignals.push_back(signal);
    }
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::starting()
{
    ::fwData::Image::csptr src = this->getInput< ::fwData::Image >(s_SOURCE_INPUT);

    for (auto signalKey: m_managedSignals)
    {
        m_connections.connect(src, signalKey, this->getSptr(), m_availableConnection[signalKey]);
    }
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::stopping()
{
    m_connections.disconnect();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::swapping()
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::updating()
{
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::info( std::ostream& _sstream )
{
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardModified()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardBufferModified()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::BufferModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkAdded(SPTR(::fwData::Point)point)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::LandmarkAddedSignalType >(::fwData::Image::s_LANDMARK_ADDED_SIG);
    sig->asyncEmit(point);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkRemoved(SPTR(::fwData::Point)point)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::LandmarkRemovedSignalType >(::fwData::Image::s_LANDMARK_REMOVED_SIG);
    sig->asyncEmit(point);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkDisplayed(bool display)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::LandmarkDisplayedSignalType >(::fwData::Image::s_LANDMARK_DISPLAYED_SIG);
    sig->asyncEmit(display);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceAdded(SPTR(::fwData::PointList)pointList)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::DistanceAddedSignalType >(::fwData::Image::s_DISTANCE_ADDED_SIG);
    sig->asyncEmit(pointList);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceRemoved(CSPTR(::fwData::PointList)pointList)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::DistanceRemovedSignalType >(::fwData::Image::s_DISTANCE_REMOVED_SIG);
    sig->asyncEmit(pointList);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceDisplayed(bool display)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::DistanceDisplayedSignalType >(::fwData::Image::s_DISTANCE_DISPLAYED_SIG);
    sig->asyncEmit(display);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardSliceIndexModified(int axial, int frontal, int sagittal)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::SliceIndexModifiedSignalType >(
        ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG);
    sig->asyncEmit(axial, frontal, sagittal);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardSliceTypeModified(int from, int to)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig =
        image->signal< ::fwData::Image::SliceTypeModifiedSignalType >(::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG);
    sig->asyncEmit(from, to);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardVisibilityModified(bool visibility)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig =
        image->signal< ::fwData::Image::VisibilityModifiedSignalType >(::fwData::Image::s_VISIBILITY_MODIFIED_SIG);
    sig->asyncEmit(visibility);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardTransparencyModified()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Image::TransparencyModifiedSignalType >(
        ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardAddedFields(::fwData::Object::FieldsContainerType objects)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Object::AddedFieldsSignalType >(::fwData::Object::s_ADDED_FIELDS_SIG);
    sig->asyncEmit(objects);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardChangedFields(::fwData::Object::FieldsContainerType newObjects,
                                                 ::fwData::Object::FieldsContainerType oldObjects)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Object::ChangedFieldsSignalType >(::fwData::Object::s_CHANGED_FIELDS_SIG);
    sig->asyncEmit(newObjects, oldObjects);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardRemovedFields(::fwData::Object::FieldsContainerType objects)
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    auto sig = image->signal< ::fwData::Object::RemovedFieldsSignalType >(::fwData::Object::s_REMOVED_FIELDS_SIG);
    sig->asyncEmit(objects);
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
