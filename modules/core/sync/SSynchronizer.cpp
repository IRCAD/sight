/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "SSynchronizer.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

#include <data/TimeLine.hpp>

namespace sight::module::sync
{

// ----------------------------------------------------------------------------

SSynchronizer::SSynchronizer()
{
    newSlot(slots::s_RESET_TIMELINE_SLOT, &SSynchronizer::resetTimeline, this);
    newSlot(slots::s_SYNCHRONIZE_SLOT, &SSynchronizer::synchronize, this);
    newSlot(slots::s_SET_FRAME_BINDING_SLOT, &SSynchronizer::setFrameBinding, this);
    newSlot(slots::s_SET_MATRIX_BINDING_SLOT, &SSynchronizer::setMatrixBinding, this);
    newSlot(slots::s_SET_DELAY_SLOT, &SSynchronizer::setDelay, this);

    newSignal<signals::timestamp_signal_t>(signals::s_SYNCHRONIZATION_DONE_SIG);
    newSignal<signals::void_signal_t>(signals::s_SYNCHRONIZATION_SKIPPED_SIG);
    newSignal<signals::int_signal_t>(signals::s_FRAME_SYNCHRONIZED_SIG);
    newSignal<signals::int_signal_t>(signals::s_FRAME_UNSYNCHRONIZED_SIG);
    newSignal<signals::int_signal_t>(signals::s_MATRIX_SYNCHRONIZED_SIG);
    newSignal<signals::int_signal_t>(signals::s_MATRIX_UNSYNCHRONIZED_SIG);
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SSynchronizer::getAutoConnections() const
{
    return {
        {configKey::s_FRAMETL_INPUT, data::TimeLine::s_CLEARED_SIG, slots::s_RESET_TIMELINE_SLOT},
        {configKey::s_MATRIXTL_INPUT, data::TimeLine::s_CLEARED_SIG, slots::s_RESET_TIMELINE_SLOT},
        {configKey::s_FRAMETL_INPUT, data::TimeLine::s_OBJECT_PUSHED_SIG, IService::slots::s_UPDATE},
        {configKey::s_MATRIXTL_INPUT, data::TimeLine::s_OBJECT_PUSHED_SIG, IService::slots::s_UPDATE}
    };
}

// ----------------------------------------------------------------------------

void SSynchronizer::configuring()
{
    const auto cfg = this->getConfiguration();

    m_legacyAutoSync = cfg.get<bool>(configKey::s_LEGACY_AUTO_SYNCH, m_legacyAutoSync);

    m_tolerance = cfg.get<core::HiResClock::HiResClockType>(configKey::s_TOLERANCE, m_tolerance);
}

//-----------------------------------------------------------------------------

void SSynchronizer::starting()
{
    const ConfigType configuration = this->getConfiguration();

    const auto inConfig = configuration.equal_range("in");
    for(auto itConfig = inConfig.first ; itConfig != inConfig.second ; ++itConfig)
    {
        const std::string group = itConfig->second.get<std::string>("<xmlattr>.group", "");
        if(group == configKey::s_FRAMETL_INPUT)
        {
            const auto keyConfig = itConfig->second.equal_range(configKey::s_KEY);
            for(auto frameTLConfig = keyConfig.first ;
                frameTLConfig != keyConfig.second ;
                ++frameTLConfig)
            {
                int delay = frameTLConfig->second.get<int>(
                    configKey::s_TL_DELAY,
                    0
                );
                if(delay < 0)
                {
                    SIGHT_WARN("Synchronization delay should be positive");
                    delay = 0;
                }

                m_frameTLDelay.push_back(delay);
            }
        }
        else if(group == configKey::s_MATRIXTL_INPUT)
        {
            const auto keyConfig = itConfig->second.equal_range(configKey::s_KEY);
            for(auto matrixTLConfig = keyConfig.first ;
                matrixTLConfig != keyConfig.second ;
                ++matrixTLConfig)
            {
                int delay = matrixTLConfig->second.get<int>(
                    configKey::s_TL_DELAY,
                    0
                );
                if(delay < 0)
                {
                    SIGHT_WARN("Synchronization delay should be positive");
                    delay = 0;
                }

                m_matrixTLDelay.push_back(delay);
            }
        }
    }

    const auto inoutsConfig = configuration.equal_range("inout");
    for(auto itConfig = inoutsConfig.first ; itConfig != inoutsConfig.second ; ++itConfig)
    {
        const std::string group = itConfig->second.get<std::string>("<xmlattr>.group", "");

        if(group == configKey::s_FRAME_INOUT)
        {
            std::size_t frameIndex = 0;
            const auto keyConfig   = itConfig->second.equal_range(configKey::s_KEY);
            for(auto frameOutVarConfig = keyConfig.first ;
                frameOutVarConfig != keyConfig.second ;
                ++frameOutVarConfig, ++frameIndex)
            {
                const std::size_t tlIndex = frameOutVarConfig->second.get<std::size_t>(
                    configKey::s_OUTVAR_TL_INDEX,
                    0
                );
                const unsigned int elementIndex = frameOutVarConfig->second.get<unsigned int>(
                    configKey::s_OUTVAR_ELEMENT_INDEX,
                    0
                );
                const bool sendStatus = frameOutVarConfig->second.get<bool>(
                    configKey::s_OUTVAR_SEND_STATUS,
                    false
                );

                m_frameOutVarParameters.emplace_back(
                    outVarParameter(
                    {
                        frameIndex,
                        tlIndex,
                        elementIndex,
                        false,
                        sendStatus,
                        0
                    })
                );
            }
        }
        else if(group == configKey::s_MATRIX_INOUT)
        {
            std::size_t matrixIndex = 0;
            const auto keyConfig    = itConfig->second.equal_range(configKey::s_KEY);
            for(auto matrixOutVarConfig = keyConfig.first ;
                matrixOutVarConfig != keyConfig.second ;
                ++matrixOutVarConfig, ++matrixIndex)
            {
                const std::size_t tlIndex = matrixOutVarConfig->second.get<std::size_t>(
                    configKey::s_OUTVAR_TL_INDEX,
                    0
                );
                const unsigned int elementIndex = matrixOutVarConfig->second.get<unsigned int>(
                    configKey::s_OUTVAR_ELEMENT_INDEX,
                    0
                );
                const bool sendStatus = matrixOutVarConfig->second.get<bool>(
                    configKey::s_OUTVAR_SEND_STATUS,
                    false
                );

                m_matrixOutVarParameters.emplace_back(
                    outVarParameter(
                    {
                        matrixIndex,
                        tlIndex,
                        elementIndex,
                        false,
                        sendStatus,
                        0
                    })
                );
            }
        }
    }

    SIGHT_ASSERT("No valid worker for timer.", this->worker());
    if(m_legacyAutoSync)
    {
        m_worker = sight::core::thread::Worker::New();
        m_timer  = m_worker->createTimer();
        const auto duration = std::chrono::milliseconds(m_timeStep);
        m_timer->setFunction([this](auto&& ...){synchronize();});
        m_timer->setDuration(duration);
        m_timer->start();
    }
}

// ----------------------------------------------------------------------------

void SSynchronizer::updating()
{
    m_updateMask |= OBJECT_RECEIVED;

    if((m_updateMask & SYNC_REQUESTED) != 0)
    {
        this->synchronize();
    }
}

// ----------------------------------------------------------------------------

void SSynchronizer::stopping()
{
    if(m_legacyAutoSync)
    {
        m_timer->stop();
        m_timer.reset();
        m_worker->stop();
        m_worker.reset();
    }
}

// ----------------------------------------------------------------------------

void SSynchronizer::synchronize()
{
    m_updateMask |= SYNC_REQUESTED;
    if((m_updateMask & OBJECT_RECEIVED) == 0)
    {
        return;
    }

    // do the synchronisation
    // step 1: get the TL implicated in the synchronization
    std::vector<std::size_t> frameTl_populated_index;
    std::vector<core::HiResClock::HiResClockType> frameTl_populated_timestamp;

    for(std::size_t i = 0 ; i != m_frameTLs.size() ; ++i)
    {
        const auto tl = m_frameTLs[i].lock();
        // if the tl is null, ignore it
        if(tl)
        {
            // get the tl new timestamp
            const auto tl_newest_timestamp = tl->getNewerTimestamp();

            //treat only the tl with some data inside.
            if(tl_newest_timestamp > 0)
            {
                frameTl_populated_timestamp.push_back(tl_newest_timestamp);
                frameTl_populated_index.push_back(i);
            }
        }
    }

    std::vector<std::size_t> matrixTl_populated_index;
    std::vector<core::HiResClock::HiResClockType> matrixTl_populated_timestamp;

    for(std::size_t i = 0 ; i != m_matrixTLs.size() ; ++i)
    {
        const auto tl = m_matrixTLs[i].lock();
        if(tl)
        {
            // get the tl new timestamp
            const auto tl_newest_timestamp = tl->getNewerTimestamp();

            //treat only the tl with some data inside.
            if(tl_newest_timestamp > 0)
            {
                matrixTl_populated_timestamp.push_back(tl_newest_timestamp);
                matrixTl_populated_index.push_back(i);
            }
        }
    }

    // step 2: find the synchronization timestamp

    // Timestamp reference for the synchronization
    const auto frameTl_max_timestamp = frameTl_populated_timestamp.empty() ? 0 : *(std::max_element(
                                                                                       frameTl_populated_timestamp.
                                                                                       begin(),
                                                                                       frameTl_populated_timestamp.
                                                                                       end()
    ));

    const auto matrixTl_max_timestamp = matrixTl_populated_timestamp.empty() ? 0 : *(std::max_element(
                                                                                         matrixTl_populated_timestamp
                                                                                         .begin(),
                                                                                         matrixTl_populated_timestamp
                                                                                         .end()
    ));

    core::HiResClock::HiResClockType maxSynchronizationTimestamp = std::max(
        frameTl_max_timestamp,
        matrixTl_max_timestamp
    );

    // This gives the most recent timestamp provided in a TL.
    // However the max is not enough, as some timelines have there own maxTimestamp < global maxTimestamp.
    // Using the global max, will imply that only the most recent TL can be properly synchronized.
    // To cover this, the algorithm will find every TL which are in the tolerance range from the max, and will take the
    // minimum.
    // This should allow a synchronization around a reference timestamp which is up to date, while being consensual
    // over the populated TL.

    if(maxSynchronizationTimestamp == 0)
    {
        // there is nothing to synchronize
        SIGHT_INFO("skip synch, because there is nothing to synch");
        this->signal<signals::void_signal_t>(signals::s_SYNCHRONIZATION_SKIPPED_SIG)->asyncEmit();
        return;
    }

    core::HiResClock::HiResClockType synchronizationTimestamp = maxSynchronizationTimestamp;
    std::vector<std::size_t> frameTl_to_synch_index;
    for(std::size_t i = 0 ; i < frameTl_populated_timestamp.size() ; i++)
    {
        if(maxSynchronizationTimestamp - frameTl_populated_timestamp[i] < m_tolerance)
        {
            synchronizationTimestamp = std::min(synchronizationTimestamp, frameTl_populated_timestamp[i]);
            frameTl_to_synch_index.push_back(frameTl_populated_index[i]);
        }
    }

    std::vector<std::size_t> matrixTl_to_synch_index;
    for(std::size_t i = 0 ; i < matrixTl_populated_timestamp.size() ; i++)
    {
        if(maxSynchronizationTimestamp - matrixTl_populated_timestamp[i] < m_tolerance)
        {
            synchronizationTimestamp = std::min(synchronizationTimestamp, matrixTl_populated_timestamp[i]);
            matrixTl_to_synch_index.push_back(matrixTl_populated_index[i]);
        }
    }

    //step 3: get the matrix + frame and populate the output

    if(m_lastTimeStamp != synchronizationTimestamp)
    {
        m_lastTimeStamp = synchronizationTimestamp;

        for(const std::size_t tlIndex : frameTl_to_synch_index)
        {
            copyFrameFromTLtoOutput(tlIndex, synchronizationTimestamp);
        }

        for(const std::size_t tlIndex : matrixTl_to_synch_index)
        {
            copyMatrixFromTLtoOutput(tlIndex, synchronizationTimestamp);
        }

        this->signal<signals::timestamp_signal_t>(signals::s_SYNCHRONIZATION_DONE_SIG)->asyncEmit(
            synchronizationTimestamp
        );
        sendFrameVarStatus(frameTl_to_synch_index);
        sendMatrixVarStatus(matrixTl_to_synch_index);

        // synchronisation has been done with success, reset the mask
        m_updateMask = m_legacyAutoSync ? OBJECT_RECEIVED : 0;
    }
    else
    {
        SIGHT_INFO("skip synch, because m_lastTimeStamp == synchronizationTimestamp");
        this->signal<signals::void_signal_t>(signals::s_SYNCHRONIZATION_SKIPPED_SIG)->asyncEmit();
    }
}

// ----------------------------------------------------------------------------

std::vector<SSynchronizer::outVarParameter> SSynchronizer::getFrameTlOutputVarIndex(std::size_t frameTLIndex)
{
    std::vector<outVarParameter> result;
    for(auto outvarParam : m_frameOutVarParameters)
    {
        if(outvarParam.tlIndex == frameTLIndex)
        {
            result.push_back(outvarParam);
        }
    }

    return result;
}

// ----------------------------------------------------------------------------

void SSynchronizer::copyFrameFromTLtoOutput(
    std::size_t frameTLIndex,
    core::HiResClock::HiResClockType synchronizationTimestamp
)
{
    const auto frameTL = m_frameTLs[frameTLIndex].lock();
    CSPTR(data::FrameTL::BufferType) buffer =
        frameTL->getClosestBuffer(synchronizationTimestamp - m_frameTLDelay[frameTLIndex]);
    data::Image::Size frameTLsize                 = {frameTL->getWidth(), frameTL->getHeight(), 0};
    std::size_t frameTlNumComponents              = frameTL->numComponents();
    core::Type frameTlType                        = frameTL->getType();
    data::FrameTL::PixelFormat frameTlPixelFormat = frameTL->getPixelFormat();

    if(buffer)
    {
        for(const outVarParameter outputVarParam : getFrameTlOutputVarIndex(frameTLIndex))
        {
            const std::size_t frameOutIndex        = outputVarParam.outVarIndex;
            const unsigned int frameTlElementIndex = outputVarParam.tlElementIndex;

            const auto frame = m_frames[frameOutIndex].lock();
            SIGHT_ASSERT("Image with index '" << frameOutIndex << "' does not exist", frame);

            // Check if frame dimensions have changed
            if(frameTLsize != frame->getSize() || frameTlNumComponents != frame->numComponents())
            {
                data::Image::PixelFormat format {data::Image::UNDEFINED};
                switch(frameTlPixelFormat)
                {
                    case data::FrameTL::PixelFormat::GRAY_SCALE:
                        format = data::Image::GRAY_SCALE;
                        break;

                    case data::FrameTL::PixelFormat::RGB:
                        format = data::Image::RGB;
                        break;

                    case data::FrameTL::PixelFormat::BGR:
                        format = data::Image::BGR;
                        break;

                    case data::FrameTL::PixelFormat::RGBA:
                        format = data::Image::RGBA;
                        break;

                    case data::FrameTL::PixelFormat::BGRA:
                        format = data::Image::BGRA;
                        break;

                    default:
                        SIGHT_ERROR("FrameTL pixel format undefined");
                        return;
                }

                frame->resize(frameTLsize, frameTlType, format);
                const data::Image::Origin origin = {0., 0., 0.};
                frame->setOrigin(origin);
                const data::Image::Spacing spacing = {1., 1., 1.};
                frame->setSpacing(spacing);
                frame->setWindowWidth({1.0});
                frame->setWindowCenter({0.0});
            }

            const std::uint8_t* frameBuff = &buffer->getElement(frameTlElementIndex);
            auto iter                     = frame->begin<std::uint8_t>();
            std::memcpy(&*iter, frameBuff, buffer->getSize());

            // Notify
            auto sig = frame->signal<data::Image::BufferModifiedSignalType>(data::Image::s_BUFFER_MODIFIED_SIG);
            sig->asyncEmit();
        }
    }
    else
    {
        SIGHT_ERROR(
            "Buffer not found for timestamp " << synchronizationTimestamp << " in timeline 'frame" << frameTLIndex
            << "'."
        );
    }
}

// ----------------------------------------------------------------------------

std::vector<SSynchronizer::outVarParameter> SSynchronizer::getMatrixTlOutputVarIndex(
    std::size_t matrixTLIndex
)
{
    std::vector<outVarParameter> result;
    for(auto outvarParam : m_matrixOutVarParameters)
    {
        if(outvarParam.tlIndex == matrixTLIndex)
        {
            result.push_back(outvarParam);
        }
    }

    return result;
}

//------------------------------------------------------------------------------

void SSynchronizer::copyMatrixFromTLtoOutput(
    std::size_t matrixTLIndex,
    core::HiResClock::HiResClockType synchronizationTimestamp
)
{
    const auto matrixTL = m_matrixTLs[matrixTLIndex].lock();
    CSPTR(data::MatrixTL::BufferType) buffer =
        matrixTL->getClosestBuffer(synchronizationTimestamp - m_matrixTLDelay[matrixTLIndex]);

    if(buffer)
    {
        for(const auto outputVarParam : getMatrixTlOutputVarIndex(matrixTLIndex))
        {
            const std::size_t matrixOutIndex        = outputVarParam.outVarIndex;
            const unsigned int matrixTlElementIndex = outputVarParam.tlElementIndex;

            if(buffer->isPresent(matrixTlElementIndex))
            {
                auto matrix = m_matrix[matrixOutIndex].lock();
                SIGHT_ASSERT("Matrix with indices '" << matrixOutIndex << "' does not exist", matrix);
                const auto& values = buffer->getElement(matrixTlElementIndex);
                for(std::uint8_t i = 0 ; i < 4 ; ++i)
                {
                    for(std::uint8_t j = 0 ; j < 4 ; ++j)
                    {
                        (*matrix)(i, j) = static_cast<double>(values[i * 4 + j]);
                    }
                }

                auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
                sig->asyncEmit();
            }
        }
    }
    else
    {
        SIGHT_ERROR(
            "Buffer not found for timestamp " << synchronizationTimestamp << " in timeline 'matrix" << matrixTLIndex
            << "'."
        );
    }
}

//-----------------------------------------------------------------------------

void SSynchronizer::sendFrameVarStatus(const std::vector<std::size_t>& synchFrameTLIndex)
{
    for(auto& outputVarParam : m_frameOutVarParameters)
    {
        if(outputVarParam.signalSynchronization)
        {
            //a signal should be send when synchronized/un-synchronized
            bool isSynch =
                std::find(
                    synchFrameTLIndex.begin(),
                    synchFrameTLIndex.end(),
                    outputVarParam.tlIndex
                ) != synchFrameTLIndex.end();
            if(outputVarParam.isSynchronized != isSynch)
            {
                outputVarParam.isSynchronized = isSynch;
                const auto signalKey =
                    isSynch ? signals::s_FRAME_SYNCHRONIZED_SIG : signals::s_FRAME_UNSYNCHRONIZED_SIG;
                this->signal<signals::int_signal_t>(signalKey)->asyncEmit(static_cast<int>(outputVarParam.outVarIndex));
            }
        }
    }
}

//-----------------------------------------------------------------------------

void SSynchronizer::sendMatrixVarStatus(const std::vector<std::size_t>& synchMatrixTLIndex)
{
    for(auto& outputVarParam : m_matrixOutVarParameters)
    {
        if(outputVarParam.signalSynchronization)
        {
            //a signal should be send when synchronized/un-synchronized
            bool isSynch =
                std::find(
                    synchMatrixTLIndex.begin(),
                    synchMatrixTLIndex.end(),
                    outputVarParam.tlIndex
                ) != synchMatrixTLIndex.end();
            if(outputVarParam.isSynchronized != isSynch)
            {
                outputVarParam.isSynchronized = isSynch;
                const auto signalKey =
                    isSynch ? signals::s_MATRIX_SYNCHRONIZED_SIG : signals::s_MATRIX_UNSYNCHRONIZED_SIG;
                this->signal<signals::int_signal_t>(signalKey)->asyncEmit(static_cast<int>(outputVarParam.outVarIndex));
            }
        }
    }
}

// ----------------------------------------------------------------------------

void SSynchronizer::resetTimeline()
{
    m_lastTimeStamp = 0.;
}

//-----------------------------------------------------------------------------

void SSynchronizer::setFrameBinding(
    std::size_t _tlIndex,
    unsigned int _elementIndex,
    std::size_t _outputVarIndex
)
{
    for(auto& outputVarParam : m_frameOutVarParameters)
    {
        if(outputVarParam.outVarIndex == _outputVarIndex)
        {
            outputVarParam.tlIndex        = _tlIndex;
            outputVarParam.tlElementIndex = _elementIndex;
            return;
        }
    }

    SIGHT_WARN(
        "The outputVar Index " << _outputVarIndex
        << " provided in the slot setFrameBinding has not been found."
    );
}

//-----------------------------------------------------------------------------

void SSynchronizer::setMatrixBinding(
    std::size_t _tlIndex,
    unsigned int _elementIndex,
    std::size_t _outputVarIndex
)
{
    for(auto& outputVarParam : m_matrixOutVarParameters)
    {
        if(outputVarParam.outVarIndex == _outputVarIndex)
        {
            outputVarParam.tlIndex        = _tlIndex;
            outputVarParam.tlElementIndex = _elementIndex;
            return;
        }
    }

    SIGHT_WARN(
        "The outputVar Index " << _outputVarIndex
        << " provided in the slot setMatrixBinding has not been found."
    );
}

//-----------------------------------------------------------------------------

void SSynchronizer::setDelay(int val, std::string key)
{
    if(val < 0)
    {
        SIGHT_ERROR("The delay set for " << key << " is negative. A positive value is expected");
        return;
    }

    /**
     * if the key received is of format frameDelay_i where i is a number,
     * it means that the value is a delay set for the i th frameTL
     * This works respectively for matrixDelay_i and matrixTL
     */
    if(key.rfind(slots::s_FRAME_DELAY_PREFIX, 0) == 0)
    {
        try
        {
            static constexpr size_t frameDelayKeySize = std::size(slots::s_FRAME_DELAY_PREFIX);
            const size_t frameTLIndex                 = static_cast<size_t>(std::stoul(key.substr(frameDelayKeySize)));
            if(frameTLIndex < m_frameTLDelay.size())
            {
                m_frameTLDelay[frameTLIndex] = val;
            }
            else
            {
                SIGHT_ERROR(
                    "The frameTL index " << frameTLIndex
                    << " provided in the update delay slot is out of bound"
                );
            }
        }
        catch(...)
        {
            SIGHT_ERROR("The frameTL index provided in the update delay slot is not a proper number: " << key);
        }
    }
    else if(key.rfind(slots::s_MATRIX_DELAY_PREFIX, 0) == 0)
    {
        try
        {
            static constexpr size_t matrixDelayKeySize = std::size(slots::s_MATRIX_DELAY_PREFIX);
            const size_t matrixTLIndex                 =
                static_cast<size_t>(std::stoul(key.substr(matrixDelayKeySize)));
            if(matrixTLIndex < m_matrixTLDelay.size())
            {
                m_matrixTLDelay[matrixTLIndex] = val;
            }
            else
            {
                SIGHT_ERROR(
                    "The matrixTL index " << matrixTLIndex << " provided in the update delay slot is out of bound"
                );
            }
        }
        catch(...)
        {
            SIGHT_ERROR("The matrixTL index provided in the update delay slot is not a proper number: " << key);
        }
    }
    else
    {
        SIGHT_WARN("Unknown key");
    }
}

} // namespace sight::module::sync
