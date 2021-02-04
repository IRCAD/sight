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

#include "SImageSignalForwarder.hpp"

#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>

#include <data/Composite.hpp>
#include <data/Image.hpp>

#include <services/macros.hpp>

#include <regex>

namespace sight::modules::data
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

static const services::IService::KeyType s_SOURCE_INPUT = "source";
static const services::IService::KeyType s_TARGET_INPUT = "target";

SImageSignalForwarder::AvailableConnectionMapType SImageSignalForwarder::m_availableConnection;

//-----------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::modules::data::IWrapperSrv, ::sight::modules::data::wrapper::SImageSignalForwarder,
                         ::sight::data::Image )

//-----------------------------------------------------------------------------

SImageSignalForwarder::SImageSignalForwarder() noexcept
{
    m_availableConnection[sight::data::Image::s_MODIFIED_SIG]              = s_FORWARD_MODIFIED_SLOT;
    m_availableConnection[sight::data::Image::s_BUFFER_MODIFIED_SIG]       = s_FORWARD_BUFFER_MODIFIED_SLOT;
    m_availableConnection[sight::data::Image::s_LANDMARK_ADDED_SIG]        = s_FORWARD_LANDMARK_ADDED_SLOT;
    m_availableConnection[sight::data::Image::s_LANDMARK_REMOVED_SIG]      = s_FORWARD_LANDMARK_REMOVED_SLOT;
    m_availableConnection[sight::data::Image::s_LANDMARK_DISPLAYED_SIG]    = s_FORWARD_LANDMARK_DISPLAYED_SLOT;
    m_availableConnection[sight::data::Image::s_DISTANCE_ADDED_SIG]        = s_FORWARD_DISTANCE_ADDED_SLOT;
    m_availableConnection[sight::data::Image::s_DISTANCE_REMOVED_SIG]      = s_FORWARD_DISTANCE_REMOVED_SLOT;
    m_availableConnection[sight::data::Image::s_DISTANCE_DISPLAYED_SIG]    = s_FORWARD_DISTANCE_DISPLAYED_SLOT;
    m_availableConnection[sight::data::Image::s_SLICE_INDEX_MODIFIED_SIG]  = s_FORWARD_SLICE_INDEX_MODIFIED_SLOT;
    m_availableConnection[sight::data::Image::s_SLICE_TYPE_MODIFIED_SIG]   = s_FORWARD_SLICE_TYPE_MODIFIED_SLOT;
    m_availableConnection[sight::data::Image::s_VISIBILITY_MODIFIED_SIG]   = s_FORWARD_VISIBILITY_MODIFIED_SLOT;
    m_availableConnection[sight::data::Image::s_TRANSPARENCY_MODIFIED_SIG] = s_FORWARD_TRANSPARENCY_MODIFIED_SLOT;
    m_availableConnection[sight::data::Object::s_ADDED_FIELDS_SIG]         = s_FORWARD_ADDED_FIELDS_SLOT;
    m_availableConnection[sight::data::Object::s_CHANGED_FIELDS_SIG]       = s_FORWARD_CHANGED_FIELDS_SLOT;
    m_availableConnection[sight::data::Object::s_REMOVED_FIELDS_SIG]       = s_FORWARD_REMOVED_FIELDS_SLOT;

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
    sight::data::Image::csptr src = this->getInput< sight::data::Image >(s_SOURCE_INPUT);

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
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Object::ModifiedSignalType >(
        sight::data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardBufferModified()
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        =
        image->signal< sight::data::Image::BufferModifiedSignalType >(sight::data::Image::s_BUFFER_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkAdded(SPTR(sight::data::Point)point)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Image::LandmarkAddedSignalType >(
        sight::data::Image::s_LANDMARK_ADDED_SIG);
    sig->asyncEmit(point);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkRemoved(SPTR(sight::data::Point)point)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Image::LandmarkRemovedSignalType >(
        sight::data::Image::s_LANDMARK_REMOVED_SIG);
    sig->asyncEmit(point);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkDisplayed(bool display)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Image::LandmarkDisplayedSignalType >(
        sight::data::Image::s_LANDMARK_DISPLAYED_SIG);
    sig->asyncEmit(display);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceAdded(SPTR(sight::data::PointList)pointList)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Image::DistanceAddedSignalType >(
        sight::data::Image::s_DISTANCE_ADDED_SIG);
    sig->asyncEmit(pointList);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceRemoved(CSPTR(sight::data::PointList)pointList)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Image::DistanceRemovedSignalType >(
        sight::data::Image::s_DISTANCE_REMOVED_SIG);
    sig->asyncEmit(pointList);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceDisplayed(bool display)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Image::DistanceDisplayedSignalType >(
        sight::data::Image::s_DISTANCE_DISPLAYED_SIG);
    sig->asyncEmit(display);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardSliceIndexModified(int axial, int frontal, int sagittal)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Image::SliceIndexModifiedSignalType >(
        sight::data::Image::s_SLICE_INDEX_MODIFIED_SIG);
    sig->asyncEmit(axial, frontal, sagittal);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardSliceTypeModified(int from, int to)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        =
        image->signal< sight::data::Image::SliceTypeModifiedSignalType >(sight::data::Image::s_SLICE_TYPE_MODIFIED_SIG);
    sig->asyncEmit(from, to);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardVisibilityModified(bool visibility)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        =
        image->signal< sight::data::Image::VisibilityModifiedSignalType >(sight::data::Image::s_VISIBILITY_MODIFIED_SIG);
    sig->asyncEmit(visibility);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardTransparencyModified()
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Image::TransparencyModifiedSignalType >(
        sight::data::Image::s_TRANSPARENCY_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardAddedFields(sight::data::Object::FieldsContainerType objects)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        = image->signal< sight::data::Object::AddedFieldsSignalType >(
        sight::data::Object::s_ADDED_FIELDS_SIG);
    sig->asyncEmit(objects);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardChangedFields(sight::data::Object::FieldsContainerType newObjects,
                                                 sight::data::Object::FieldsContainerType oldObjects)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        =
        image->signal< sight::data::Object::ChangedFieldsSignalType >(sight::data::Object::s_CHANGED_FIELDS_SIG);
    sig->asyncEmit(newObjects, oldObjects);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardRemovedFields(sight::data::Object::FieldsContainerType objects)
{
    sight::data::Image::csptr image = this->getInput< sight::data::Image >(s_TARGET_INPUT);
    auto sig                        =
        image->signal< sight::data::Object::RemovedFieldsSignalType >(sight::data::Object::s_REMOVED_FIELDS_SIG);
    sig->asyncEmit(objects);
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
