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

#include "synchronizer.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/image_series.hpp>

namespace sight::module::sync
{

// ----------------------------------------------------------------------------

synchronizer::synchronizer()
{
    new_slot(slots::RESET_TIMELINE_SLOT, &synchronizer::resetTimeline, this);
    new_slot(slots::SYNCHRONIZE_SLOT, &synchronizer::synchronize, this);
    new_slot(slots::SET_FRAME_BINDING_SLOT, &synchronizer::setFrameBinding, this);
    new_slot(slots::SET_MATRIX_BINDING_SLOT, &synchronizer::setMatrixBinding, this);
    new_slot(slots::SET_DELAY_SLOT, &synchronizer::setDelay, this);

    new_signal<signals::timestamp_signal_t>(signals::SYNCHRONIZATION_DONE_SIG);
    new_signal<signals::void_signal_t>(signals::SYNCHRONIZATION_SKIPPED_SIG);
    new_signal<signals::int_signal_t>(signals::FRAME_SYNCHRONIZED_SIG);
    new_signal<signals::int_signal_t>(signals::FRAME_UNSYNCHRONIZED_SIG);
    new_signal<signals::int_signal_t>(signals::MATRIX_SYNCHRONIZED_SIG);
    new_signal<signals::int_signal_t>(signals::MATRIX_UNSYNCHRONIZED_SIG);
}

//-----------------------------------------------------------------------------

service::connections_t synchronizer::auto_connections() const
{
    return {
        {configKey::s_FRAMETL_INPUT, data::timeline::signals::CLEARED, slots::RESET_TIMELINE_SLOT},
        {configKey::s_MATRIXTL_INPUT, data::timeline::signals::CLEARED, slots::RESET_TIMELINE_SLOT},
        {configKey::s_FRAMETL_INPUT, data::timeline::signals::PUSHED, service::slots::UPDATE},
        {configKey::s_MATRIXTL_INPUT, data::timeline::signals::PUSHED, service::slots::UPDATE}
    };
}

// ----------------------------------------------------------------------------

void synchronizer::configuring()
{
    const auto cfg = this->get_config();

    m_legacyAutoSync = cfg.get<bool>(configKey::s_LEGACY_AUTO_SYNCH, m_legacyAutoSync);

    m_tolerance = cfg.get<core::hires_clock::type>(configKey::s_TOLERANCE, m_tolerance);
}

//-----------------------------------------------------------------------------

void synchronizer::starting()
{
    const config_t configuration = this->get_config();

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
        m_worker = sight::core::thread::worker::make();
        m_timer  = m_worker->create_timer();
        const auto duration = std::chrono::milliseconds(m_timeStep);
        m_timer->set_function([this](auto&& ...){synchronize();});
        m_timer->set_duration(duration);
        m_timer->start();
    }
}

// ----------------------------------------------------------------------------

void synchronizer::updating()
{
    m_updateMask |= OBJECT_RECEIVED;

    if(m_legacyAutoSync)
    {
        return;
    }

    if((m_updateMask & SYNC_REQUESTED) != 0)
    {
        this->synchronize();
    }
}

// ----------------------------------------------------------------------------

void synchronizer::stopping()
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

void synchronizer::synchronize()
{
    m_updateMask |= SYNC_REQUESTED;
    if((m_updateMask & OBJECT_RECEIVED) == 0)
    {
        return;
    }

    // do the synchronisation
    // step 1: get the TL implicated in the synchronization
    std::vector<std::size_t> frameTl_populated_index;
    std::vector<core::hires_clock::type> frameTl_populated_timestamp;

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
    std::vector<core::hires_clock::type> matrixTl_populated_timestamp;

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

    core::hires_clock::type maxSynchronizationTimestamp = std::max(
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
        this->signal<signals::void_signal_t>(signals::SYNCHRONIZATION_SKIPPED_SIG)->async_emit();
        return;
    }

    core::hires_clock::type synchronizationTimestamp = maxSynchronizationTimestamp;
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

        this->signal<signals::timestamp_signal_t>(signals::SYNCHRONIZATION_DONE_SIG)->async_emit(
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
        this->signal<signals::void_signal_t>(signals::SYNCHRONIZATION_SKIPPED_SIG)->async_emit();
    }
}

// ----------------------------------------------------------------------------

std::vector<synchronizer::outVarParameter> synchronizer::getFrameTlOutputVarIndex(std::size_t frameTLIndex)
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

void synchronizer::copyFrameFromTLtoOutput(
    std::size_t frameTLIndex,
    core::hires_clock::type synchronizationTimestamp
)
{
    const auto frameTL = m_frameTLs[frameTLIndex].lock();
    CSPTR(data::frame_tl::BufferType) buffer =
        frameTL->getClosestBuffer(synchronizationTimestamp - m_frameTLDelay[frameTLIndex]);

    data::image::Size frameTLsize = {frameTL->getWidth(), frameTL->getHeight(), 0};

    std::size_t frameTlNumComponents               = frameTL->numComponents();
    core::type frameTlType                         = frameTL->getType();
    data::frame_tl::PixelFormat frameTlPixelFormat = frameTL->getPixelFormat();

    if(buffer)
    {
        for(const outVarParameter outputVarParam : getFrameTlOutputVarIndex(frameTLIndex))
        {
            const std::size_t frameOutIndex        = outputVarParam.outVarIndex;
            const unsigned int frameTlElementIndex = outputVarParam.tlElementIndex;

            const auto frame = m_frames[frameOutIndex].lock();
            SIGHT_ASSERT("image with index '" << frameOutIndex << "' does not exist", frame);

            // Check if frame dimensions have changed
            if(frameTLsize != frame->size() || frameTlNumComponents != frame->numComponents())
            {
                data::image::PixelFormat format {data::image::UNDEFINED};
                switch(frameTlPixelFormat)
                {
                    case data::frame_tl::PixelFormat::GRAY_SCALE:
                        format = data::image::GRAY_SCALE;
                        break;

                    case data::frame_tl::PixelFormat::RGB:
                        format = data::image::RGB;
                        break;

                    case data::frame_tl::PixelFormat::BGR:
                        format = data::image::BGR;
                        break;

                    case data::frame_tl::PixelFormat::RGBA:
                        format = data::image::RGBA;
                        break;

                    case data::frame_tl::PixelFormat::BGRA:
                        format = data::image::BGRA;
                        break;

                    default:
                        SIGHT_ERROR("FrameTL pixel format undefined");
                        return;
                }

                frame->resize(frameTLsize, frameTlType, format);
                const data::image::Origin origin = {0., 0., 0.};
                frame->setOrigin(origin);
                const data::image::Spacing spacing = {1., 1., 1.};
                frame->setSpacing(spacing);
                frame->setWindowWidth({1.0});
                frame->setWindowCenter({0.0});
            }

            // Set the time stamp on the image, if we set dicom image as output.
            // The value must be set after the previous `if`, in order to prevent the data
            // from being erased in case of resize (frameTLSize has a 0 value as 3 dimension)
            // and thus prevent the timestamp to be lost
            if(auto imageSeries = std::dynamic_pointer_cast<sight::data::image_series>(frame.get_shared()); imageSeries)
            {
                imageSeries->setFrameAcquisitionTimePoint(synchronizationTimestamp);
            }

            const std::uint8_t* frameBuff = &buffer->getElement(frameTlElementIndex);
            auto iter                     = frame->begin<std::uint8_t>();
            std::memcpy(&*iter, frameBuff, buffer->size());

            // Notify
            auto sig = frame->signal<data::image::BufferModifiedSignalType>(data::image::BUFFER_MODIFIED_SIG);
            sig->async_emit();
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

std::vector<synchronizer::outVarParameter> synchronizer::getMatrixTlOutputVarIndex(
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

void synchronizer::copyMatrixFromTLtoOutput(
    std::size_t matrixTLIndex,
    core::hires_clock::type synchronizationTimestamp
)
{
    const auto matrixTL = m_matrixTLs[matrixTLIndex].lock();
    CSPTR(data::matrix_tl::BufferType) buffer =
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

                auto sig = matrix->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
                sig->async_emit();
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

void synchronizer::sendFrameVarStatus(const std::vector<std::size_t>& synchFrameTLIndex)
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
                    isSynch ? signals::FRAME_SYNCHRONIZED_SIG : signals::FRAME_UNSYNCHRONIZED_SIG;
                this->signal<signals::int_signal_t>(signalKey)->async_emit(static_cast<int>(outputVarParam.outVarIndex));
            }
        }
    }
}

//-----------------------------------------------------------------------------

void synchronizer::sendMatrixVarStatus(const std::vector<std::size_t>& synchMatrixTLIndex)
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
                    isSynch ? signals::MATRIX_SYNCHRONIZED_SIG : signals::MATRIX_UNSYNCHRONIZED_SIG;
                this->signal<signals::int_signal_t>(signalKey)->async_emit(static_cast<int>(outputVarParam.outVarIndex));
            }
        }
    }
}

// ----------------------------------------------------------------------------

void synchronizer::resetTimeline()
{
    m_lastTimeStamp = 0.;
}

//-----------------------------------------------------------------------------

void synchronizer::setFrameBinding(
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

void synchronizer::setMatrixBinding(
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

void synchronizer::setDelay(int val, std::string key)
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
    if(key.rfind(slots::FRAME_DELAY_PREFIX, 0) == 0)
    {
        try
        {
            static constexpr size_t frameDelayKeySize = std::size(slots::FRAME_DELAY_PREFIX);
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
    else if(key.rfind(slots::MATRIX_DELAY_PREFIX, 0) == 0)
    {
        try
        {
            static constexpr size_t matrixDelayKeySize = std::size(slots::MATRIX_DELAY_PREFIX);
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
