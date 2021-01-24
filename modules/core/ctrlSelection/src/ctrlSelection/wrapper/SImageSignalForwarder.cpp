/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/Composite.hpp>
#include <data/Image.hpp>

#include <fwServices/macros.hpp>

#include <regex>

namespace ctrlSelection
{
namespace wrapper
{

static const core::com::Slots::SlotKeyType s_FORWARD_MODIFIED_SLOT              = "forwardModified";
static const core::com::Slots::SlotKeyType s_FORWARD_BUFFER_MODIFIED_SLOT       = "forwardBufferModified";
static const core::com::Slots::SlotKeyType s_FORWARD_LANDMARK_ADDED_SLOT        = "forwardLandmarkAdded";
static const core::com::Slots::SlotKeyType s_FORWARD_LANDMARK_REMOVED_SLOT      = "forwardLandmarkRemoved";
static const core::com::Slots::SlotKeyType s_FORWARD_LANDMARK_DISPLAYED_SLOT    = "forwardLandmarkDisplayed";
static const core::com::Slots::SlotKeyType s_FORWARD_DISTANCE_ADDED_SLOT        = "forwardDistanceAdded";
static const core::com::Slots::SlotKeyType s_FORWARD_DISTANCE_REMOVED_SLOT      = "forwardDistanceRemoved";
static const core::com::Slots::SlotKeyType s_FORWARD_DISTANCE_DISPLAYED_SLOT    = "forwardDistanceDisplayed";
static const core::com::Slots::SlotKeyType s_FORWARD_SLICE_INDEX_MODIFIED_SLOT  = "forwardSliceIndexModified";
static const core::com::Slots::SlotKeyType s_FORWARD_SLICE_TYPE_MODIFIED_SLOT   = "forwardSliceTypeModified";
static const core::com::Slots::SlotKeyType s_FORWARD_VISIBILITY_MODIFIED_SLOT   = "forwardVisibilityModified";
static const core::com::Slots::SlotKeyType s_FORWARD_TRANSPARENCY_MODIFIED_SLOT = "forwardTransparencyModified";
static const core::com::Slots::SlotKeyType s_FORWARD_ADDED_FIELDS_SLOT          = "forwardAddedFields";
static const core::com::Slots::SlotKeyType s_FORWARD_CHANGED_FIELDS_SLOT        = "forwardChangedFields";
static const core::com::Slots::SlotKeyType s_FORWARD_REMOVED_FIELDS_SLOT        = "forwardRemovedFields";

static const ::fwServices::IService::KeyType s_SOURCE_INPUT = "source";
static const ::fwServices::IService::KeyType s_TARGET_INPUT = "target";

SImageSignalForwarder::AvailableConnectionMapType SImageSignalForwarder::m_availableConnection;

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::ctrlSelection::IWrapperSrv, ::ctrlSelection::wrapper::SImageSignalForwarder,
                         data::Image )

//-----------------------------------------------------------------------------

SImageSignalForwarder::SImageSignalForwarder() noexcept
{
    m_availableConnection[data::Image::s_MODIFIED_SIG]              = s_FORWARD_MODIFIED_SLOT;
    m_availableConnection[data::Image::s_BUFFER_MODIFIED_SIG]       = s_FORWARD_BUFFER_MODIFIED_SLOT;
    m_availableConnection[data::Image::s_LANDMARK_ADDED_SIG]        = s_FORWARD_LANDMARK_ADDED_SLOT;
    m_availableConnection[data::Image::s_LANDMARK_REMOVED_SIG]      = s_FORWARD_LANDMARK_REMOVED_SLOT;
    m_availableConnection[data::Image::s_LANDMARK_DISPLAYED_SIG]    = s_FORWARD_LANDMARK_DISPLAYED_SLOT;
    m_availableConnection[data::Image::s_DISTANCE_ADDED_SIG]        = s_FORWARD_DISTANCE_ADDED_SLOT;
    m_availableConnection[data::Image::s_DISTANCE_REMOVED_SIG]      = s_FORWARD_DISTANCE_REMOVED_SLOT;
    m_availableConnection[data::Image::s_DISTANCE_DISPLAYED_SIG]    = s_FORWARD_DISTANCE_DISPLAYED_SLOT;
    m_availableConnection[data::Image::s_SLICE_INDEX_MODIFIED_SIG]  = s_FORWARD_SLICE_INDEX_MODIFIED_SLOT;
    m_availableConnection[data::Image::s_SLICE_TYPE_MODIFIED_SIG]   = s_FORWARD_SLICE_TYPE_MODIFIED_SLOT;
    m_availableConnection[data::Image::s_VISIBILITY_MODIFIED_SIG]   = s_FORWARD_VISIBILITY_MODIFIED_SLOT;
    m_availableConnection[data::Image::s_TRANSPARENCY_MODIFIED_SIG] = s_FORWARD_TRANSPARENCY_MODIFIED_SLOT;
    m_availableConnection[data::Object::s_ADDED_FIELDS_SIG]         = s_FORWARD_ADDED_FIELDS_SLOT;
    m_availableConnection[data::Object::s_CHANGED_FIELDS_SIG]       = s_FORWARD_CHANGED_FIELDS_SLOT;
    m_availableConnection[data::Object::s_REMOVED_FIELDS_SIG]       = s_FORWARD_REMOVED_FIELDS_SLOT;

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
    std::vector< core::runtime::ConfigurationElement::sptr > configs = m_configuration->find("forward");

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
    data::Image::csptr src = this->getInput< data::Image >(s_SOURCE_INPUT);

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
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardBufferModified()
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 =
        image->signal< data::Image::BufferModifiedSignalType >(data::Image::s_BUFFER_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkAdded(SPTR(data::Point)point)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Image::LandmarkAddedSignalType >(data::Image::s_LANDMARK_ADDED_SIG);
    sig->asyncEmit(point);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkRemoved(SPTR(data::Point)point)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Image::LandmarkRemovedSignalType >(
        data::Image::s_LANDMARK_REMOVED_SIG);
    sig->asyncEmit(point);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkDisplayed(bool display)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Image::LandmarkDisplayedSignalType >(
        data::Image::s_LANDMARK_DISPLAYED_SIG);
    sig->asyncEmit(display);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceAdded(SPTR(data::PointList)pointList)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Image::DistanceAddedSignalType >(data::Image::s_DISTANCE_ADDED_SIG);
    sig->asyncEmit(pointList);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceRemoved(CSPTR(data::PointList)pointList)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Image::DistanceRemovedSignalType >(
        data::Image::s_DISTANCE_REMOVED_SIG);
    sig->asyncEmit(pointList);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceDisplayed(bool display)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Image::DistanceDisplayedSignalType >(
        data::Image::s_DISTANCE_DISPLAYED_SIG);
    sig->asyncEmit(display);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardSliceIndexModified(int axial, int frontal, int sagittal)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Image::SliceIndexModifiedSignalType >(
        data::Image::s_SLICE_INDEX_MODIFIED_SIG);
    sig->asyncEmit(axial, frontal, sagittal);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardSliceTypeModified(int from, int to)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 =
        image->signal< data::Image::SliceTypeModifiedSignalType >(data::Image::s_SLICE_TYPE_MODIFIED_SIG);
    sig->asyncEmit(from, to);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardVisibilityModified(bool visibility)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 =
        image->signal< data::Image::VisibilityModifiedSignalType >(data::Image::s_VISIBILITY_MODIFIED_SIG);
    sig->asyncEmit(visibility);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardTransparencyModified()
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Image::TransparencyModifiedSignalType >(
        data::Image::s_TRANSPARENCY_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardAddedFields(data::Object::FieldsContainerType objects)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 = image->signal< data::Object::AddedFieldsSignalType >(data::Object::s_ADDED_FIELDS_SIG);
    sig->asyncEmit(objects);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardChangedFields(data::Object::FieldsContainerType newObjects,
                                                 data::Object::FieldsContainerType oldObjects)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 =
        image->signal< data::Object::ChangedFieldsSignalType >(data::Object::s_CHANGED_FIELDS_SIG);
    sig->asyncEmit(newObjects, oldObjects);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardRemovedFields(data::Object::FieldsContainerType objects)
{
    data::Image::csptr image = this->getInput< data::Image >(s_TARGET_INPUT);
    auto sig                 =
        image->signal< data::Object::RemovedFieldsSignalType >(data::Object::s_REMOVED_FIELDS_SIG);
    sig->asyncEmit(objects);
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
