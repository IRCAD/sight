/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ctrlSelection/wrapper/SImageSignalForwarder.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <boost/regex.hpp>
#include <fwData/Composite.hpp>

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
    if (!this->isVersion2())
    {
        ::fwRuntime::ConfigurationElement::sptr fromUidCfg = m_configuration->findConfigurationElement("fromUid");
        if (fromUidCfg)
        {
            m_sourceImageUid = fromUidCfg->getValue();
        }
        ::fwRuntime::ConfigurationElement::sptr fromKeyCfg = m_configuration->findConfigurationElement("fromKey");
        if (fromKeyCfg)
        {
            std::string src = fromKeyCfg->getValue();
            ::boost::regex re("(.*)\\[(.*)\\]");
            ::boost::smatch match;
            std::string uid, key;

            if( ::boost::regex_match(src, match, re) )
            {
                OSLM_ASSERT("Wrong value for attribute src: "<<src, match.size() >= 3);
                uid.assign(match[1].first, match[1].second);
                key.assign(match[2].first, match[2].second);
            }
            m_sourceCompoUid = uid;
            m_sourceImageKey = key;
        }

        SLM_ASSERT("only one tag 'fromUid' or 'fromKey' must be defined.",
                   (!m_sourceImageUid.empty() && m_sourceCompoUid.empty() && m_sourceImageKey.empty()) ||
                   (m_sourceImageUid.empty() && !m_sourceCompoUid.empty() && !m_sourceImageKey.empty()));
    }

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
    ::fwData::Image::csptr src;

    if (!this->isVersion2())
    {
        if (!m_sourceImageUid.empty())
        {
            ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_sourceImageUid);
            src                         = ::fwData::Image::dynamicCast(obj);
            SLM_ASSERT("Source image '" + m_sourceImageUid + "' is not found", src);
        }
        else
        {
            ::fwTools::Object::sptr obj     = ::fwTools::fwID::getObject(m_sourceCompoUid);
            ::fwData::Composite::sptr compo = ::fwData::Composite::dynamicCast(obj);
            src                             = compo->at< ::fwData::Image >(m_sourceImageKey);
        }
    }
    else
    {
        src = this->getInput< ::fwData::Image >(s_SOURCE_INPUT);
    }

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
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }
    auto sig = image->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardBufferModified()
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }
    auto sig = image->signal< ::fwData::Image::BufferModifiedSignalType >(::fwData::Image::s_BUFFER_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkAdded(SPTR(::fwData::Point)point)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }
    auto sig = image->signal< ::fwData::Image::LandmarkAddedSignalType >(::fwData::Image::s_LANDMARK_ADDED_SIG);
    sig->asyncEmit(point);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkRemoved(SPTR(::fwData::Point)point)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }
    auto sig = image->signal< ::fwData::Image::LandmarkRemovedSignalType >(::fwData::Image::s_LANDMARK_REMOVED_SIG);
    sig->asyncEmit(point);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardLandmarkDisplayed(bool display)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig = image->signal< ::fwData::Image::LandmarkDisplayedSignalType >(
        ::fwData::Image::s_LANDMARK_DISPLAYED_SIG);
    sig->asyncEmit(display);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceAdded(SPTR(::fwData::PointList)pointList)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig = image->signal< ::fwData::Image::DistanceAddedSignalType >(::fwData::Image::s_DISTANCE_ADDED_SIG);
    sig->asyncEmit(pointList);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceRemoved(SPTR(::fwData::PointList)pointList)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig =
        image->signal< ::fwData::Image::DistanceRemovedSignalType >(::fwData::Image::s_DISTANCE_REMOVED_SIG);
    sig->asyncEmit(pointList);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardDistanceDisplayed(bool display)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig = image->signal< ::fwData::Image::DistanceDisplayedSignalType >(
        ::fwData::Image::s_DISTANCE_DISPLAYED_SIG);
    sig->asyncEmit(display);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardSliceIndexModified(int axial, int frontal, int sagittal)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig = image->signal< ::fwData::Image::SliceIndexModifiedSignalType >(
        ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG);
    sig->asyncEmit(axial, frontal, sagittal);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardSliceTypeModified(int from, int to)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig =
        image->signal< ::fwData::Image::SliceTypeModifiedSignalType >(::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG);
    sig->asyncEmit(from, to);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardVisibilityModified(bool visibility)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig =
        image->signal< ::fwData::Image::VisibilityModifiedSignalType >(::fwData::Image::s_VISIBILITY_MODIFIED_SIG);
    sig->asyncEmit(visibility);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardTransparencyModified()
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig = image->signal< ::fwData::Image::TransparencyModifiedSignalType >(
        ::fwData::Image::s_TRANSPARENCY_MODIFIED_SIG);
    sig->asyncEmit();
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardAddedFields(::fwData::Object::FieldsContainerType objects)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig = image->signal< ::fwData::Object::AddedFieldsSignalType >(::fwData::Object::s_ADDED_FIELDS_SIG);
    sig->asyncEmit(objects);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardChangedFields(::fwData::Object::FieldsContainerType newObjects,
                                                 ::fwData::Object::FieldsContainerType oldObjects)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig = image->signal< ::fwData::Object::ChangedFieldsSignalType >(::fwData::Object::s_CHANGED_FIELDS_SIG);
    sig->asyncEmit(newObjects, oldObjects);
}

//-----------------------------------------------------------------------------

void SImageSignalForwarder::forwardRemovedFields(::fwData::Object::FieldsContainerType objects)
{
    ::fwData::Image::csptr image;
    if (this->isVersion2())
    {
        image = this->getInput< ::fwData::Image >(s_TARGET_INPUT);
    }
    else
    {
        image = this->getObject< ::fwData::Image >();
    }    auto sig = image->signal< ::fwData::Object::RemovedFieldsSignalType >(::fwData::Object::s_REMOVED_FIELDS_SIG);
    sig->asyncEmit(objects);
}

//-----------------------------------------------------------------------------

} // wrapper
} // ctrlSelection
