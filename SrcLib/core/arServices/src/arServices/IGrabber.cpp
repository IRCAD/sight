/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "arServices/IGrabber.hpp"

#include <arData/FrameTL.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

namespace arServices
{

const ::fwCom::Signals::SignalKeyType IGrabber::s_POSITION_MODIFIED_SIG = "positionModified";
const ::fwCom::Signals::SignalKeyType IGrabber::s_DURATION_MODIFIED_SIG = "durationModified";

const ::fwCom::Signals::SignalKeyType IGrabber::s_CAMERA_STARTED_SIG = "cameraStarted";
const ::fwCom::Signals::SignalKeyType IGrabber::s_CAMERA_STOPPED_SIG = "cameraStopped";

const ::fwCom::Signals::SignalKeyType IGrabber::s_FRAME_PRESENTED_SIG = "framePresented";

const ::fwCom::Slots::SlotKeyType IGrabber::s_START_CAMERA_SLOT       = "startCamera";
const ::fwCom::Slots::SlotKeyType IGrabber::s_STOP_CAMERA_SLOT        = "stopCamera";
const ::fwCom::Slots::SlotKeyType IGrabber::s_PAUSE_CAMERA_SLOT       = "pauseCamera";
const ::fwCom::Slots::SlotKeyType IGrabber::s_PLAY_PAUSE_CAMERA_SLOT  = "playPauseCamera";
const ::fwCom::Slots::SlotKeyType IGrabber::s_LOOP_VIDEO_SLOT         = "loopVideo";
const ::fwCom::Slots::SlotKeyType IGrabber::s_SET_POSITION_VIDEO_SLOT = "setPositionVideo";
const ::fwCom::Slots::SlotKeyType IGrabber::s_PREVIOUS_IMAGE_SLOT     = "previousImage";
const ::fwCom::Slots::SlotKeyType IGrabber::s_NEXT_IMAGE_SLOT         = "nextImage";
const ::fwCom::Slots::SlotKeyType IGrabber::s_SET_STEP_SLOT           = "setStep";

const ::fwServices::IService::KeyType IGrabber::s_CAMERA_INPUT  = "camera";
const ::fwServices::IService::KeyType IGrabber::s_FRAMETL_INOUT = "frameTL";

// ----------------------------------------------------------------------------

IGrabber::IGrabber() noexcept
{
    //Declare all signals
    newSignal< PositionModifiedSignalType >( s_POSITION_MODIFIED_SIG );
    newSignal< DurationModifiedSignalType >( s_DURATION_MODIFIED_SIG );
    newSignal< CameraStartedSignalType >   ( s_CAMERA_STARTED_SIG    );
    newSignal< CameraStoppedSignalType >   ( s_CAMERA_STOPPED_SIG    );
    newSignal< FramePresentedSignalType >  ( s_FRAME_PRESENTED_SIG   );

    newSlot( s_START_CAMERA_SLOT, &IGrabber::startCamera, this );
    newSlot( s_STOP_CAMERA_SLOT, &IGrabber::stopCamera, this );
    newSlot( s_PAUSE_CAMERA_SLOT, &IGrabber::pauseCamera, this );
    newSlot( s_PLAY_PAUSE_CAMERA_SLOT, &IGrabber::playPauseCamera, this );
    newSlot( s_LOOP_VIDEO_SLOT, &IGrabber::toggleLoopMode, this );
    newSlot( s_SET_POSITION_VIDEO_SLOT, &IGrabber::setPosition, this );
    newSlot( s_PREVIOUS_IMAGE_SLOT, &IGrabber::previousImage, this);
    newSlot(s_NEXT_IMAGE_SLOT, &IGrabber::nextImage, this);
    newSlot(s_SET_STEP_SLOT, &IGrabber::setStep, this);
}

// ----------------------------------------------------------------------------

IGrabber::~IGrabber() noexcept
{

}

// ----------------------------------------------------------------------------

void IGrabber::playPauseCamera()
{
    if(m_isStarted)
    {
        this->pauseCamera();
    }
    else
    {
        this->startCamera();
    }
}

// ----------------------------------------------------------------------------
void IGrabber::previousImage()
{
    SLM_WARN("Frame by frame mode not implemented for this grabber type.");
}

// ----------------------------------------------------------------------------

void IGrabber::nextImage()
{
    SLM_WARN("Frame by frame mode not implemented for this grabber type.");
}

// ----------------------------------------------------------------------------

void IGrabber::setStep(int /*step*/, std::string /*key*/)
{
    SLM_WARN("Frame by frame mode not implemented for this grabber type.");
}

// ----------------------------------------------------------------------------

void IGrabber::clearTimeline(::arData::FrameTL::sptr const& _tl)
{
    if(_tl->isAllocated())
    {
        // Clear the timeline: send a black frame
        const ::fwCore::HiResClock::HiResClockType timestamp = _tl->getNewerTimestamp() + 1;

        SPTR(::arData::FrameTL::BufferType) buffer = _tl->createBuffer(timestamp);
        auto destBuffer = reinterpret_cast< std::uint8_t* >( buffer->addElement(0) );

        std::fill(destBuffer, destBuffer + _tl->getWidth() * _tl->getHeight() * _tl->getNumberOfComponents(), 0);

        // push buffer and notify
        _tl->clearTimeline();
        _tl->pushObject(buffer);

        auto sigTL = _tl->signal< ::arData::TimeLine::ObjectPushedSignalType >(
            ::arData::TimeLine::s_OBJECT_PUSHED_SIG );
        sigTL->asyncEmit(timestamp);
    }
}

// ----------------------------------------------------------------------------

void IGrabber::setStartState(bool state)
{
    m_isStarted = state;
}

// ----------------------------------------------------------------------------

}  // namespace arServices
