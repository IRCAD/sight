/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "SFrameMatrixSynchronizer.hpp"

#include <core/com/Signal.hxx>
#include <core/com/Slots.hxx>

namespace sight::module::sync
{

const core::com::Signals::SignalKeyType SFrameMatrixSynchronizer::s_SYNCHRONIZATION_DONE_SIG    = "synchronizationDone";
const core::com::Signals::SignalKeyType SFrameMatrixSynchronizer::s_SYNCHRONIZATION_SKIPPED_SIG =
    "synchronizationSkipped";
const core::com::Signals::SignalKeyType SFrameMatrixSynchronizer::s_ALL_MATRICES_FOUND_SIG    = "allMatricesFound";
const core::com::Signals::SignalKeyType SFrameMatrixSynchronizer::s_MATRIX_SYNCHRONIZED_SIG   = "matrixSynchronized";
const core::com::Signals::SignalKeyType SFrameMatrixSynchronizer::s_MATRIX_UNSYNCHRONIZED_SIG = "matrixUnsynchronized";

// Private slot
const core::com::Slots::SlotKeyType s_RESET_TIMELINE_SLOT = "reset";

// Public slots
const core::com::Slots::SlotKeyType SFrameMatrixSynchronizer::s_SYNCHRONIZE_SLOT     = "synchronize";
const core::com::Slots::SlotKeyType SFrameMatrixSynchronizer::s_SET_FRAME_DELAY_SLOT = "setFrameDelay";

// ----------------------------------------------------------------------------

SFrameMatrixSynchronizer::SFrameMatrixSynchronizer() noexcept :
    m_tolerance(500.),
    m_imagesInitialized(false),
    m_timeStep(33),
    m_lastTimestamp(std::numeric_limits<double>::lowest())
{
    m_sigSynchronizationDone    = newSignal<SynchronizationDoneSignalType>(s_SYNCHRONIZATION_DONE_SIG);
    m_sigSynchronizationSkipped = newSignal<synchronizationSkippedSignalType>(s_SYNCHRONIZATION_SKIPPED_SIG);
    m_sigAllMatricesFound       = newSignal<AllMatricesFoundSignalType>(s_ALL_MATRICES_FOUND_SIG);
    m_sigMatrixSynchronized     = newSignal<MatrixSynchronizedSignalType>(s_MATRIX_SYNCHRONIZED_SIG);
    m_sigMatrixUnsynchronized   = newSignal<MatrixUnsynchronizedSignalType>(s_MATRIX_UNSYNCHRONIZED_SIG);

    newSlot(s_RESET_TIMELINE_SLOT, &SFrameMatrixSynchronizer::resetTimeline, this);
    newSlot(s_SYNCHRONIZE_SLOT, &SFrameMatrixSynchronizer::synchronize, this);
    newSlot(s_SET_FRAME_DELAY_SLOT, &SFrameMatrixSynchronizer::setFrameDelay, this);
}

//-----------------------------------------------------------------------------

SFrameMatrixSynchronizer::~SFrameMatrixSynchronizer() noexcept
{
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SFrameMatrixSynchronizer::getAutoConnections() const
{
    return {
        {s_FRAMETL_INPUT, data::TimeLine::s_CLEARED_SIG, s_RESET_TIMELINE_SLOT},
        {s_FRAMETL_INPUT, data::TimeLine::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT},
        {s_MATRIXTL_INPUT, data::TimeLine::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT}
    };
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::configuring()
{
    const auto cfg = this->getConfigTree();

    const auto framerate = cfg.get<unsigned int>("framerate", 30);
    m_timeStep  = framerate != 0 ? 1000 / cfg.get<unsigned int>("framerate", 30) : 0;
    m_tolerance = cfg.get<unsigned int>("tolerance", 500);

    if(framerate != 0)
    {
        m_updateMask = OBJECT_RECEIVED;
    }
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::starting()
{
    SIGHT_ASSERT("You should have the same number of 'frameTL' and 'image' keys", m_frameTLs.size() == m_images.size());

    const std::size_t numMatrixTLs = m_matrixTLs.size();
    SIGHT_ASSERT(
        "Maximum number of matrix timelines is exceeded: " << numMatrixTLs << ">= " << s_MAX_MATRICES_TL,
        numMatrixTLs < s_MAX_MATRICES_TL
    );

    m_totalOutputMatrices = 0;
    for(std::size_t i = 0 ; i < numMatrixTLs ; ++i)
    {
        const std::size_t numMatrices = m_matrices[i].size();
        m_totalOutputMatrices += numMatrices;
    }

    // We need to browser the XML tree to check if a matrix (inside a matrixTL) has or not a `sendStatus` configuration
    const ConfigType configuration = this->getConfigTree();
    const auto inoutsConfig        = configuration.equal_range("inout");
    int matrixIndex                = 0;
    for(auto itConfig = inoutsConfig.first ; itConfig != inoutsConfig.second ; ++itConfig)
    {
        const std::string group = itConfig->second.get<std::string>("<xmlattr>.group", "");
        if(group.find("matrices") != std::string::npos)
        {
            std::vector<int> sendStatus;

            const auto keyConfig = itConfig->second.equal_range("key");
            for(auto itKeyConfig = keyConfig.first ; itKeyConfig != keyConfig.second ; ++itKeyConfig)
            {
                const bool needToSendStatus = itKeyConfig->second.get<bool>("<xmlattr>.sendStatus", false);
                if(needToSendStatus)
                {
                    // Push the index that will be send through the `matrixSynchronized` signal
                    sendStatus.push_back(matrixIndex);
                    matrixIndex++;
                }
                else
                {
                    // Push back -1 means that we don't want to send its signal later
                    sendStatus.push_back(-1);
                }
            }

            m_sendMatricesStatus.push_back(sendStatus);
        }
    }

    SIGHT_ASSERT("No valid worker for timer.", m_associatedWorker);
    if(m_timeStep)
    {
        m_timer = m_associatedWorker->createTimer();
        const auto duration = std::chrono::milliseconds(m_timeStep);
        m_timer->setFunction(std::bind(&SFrameMatrixSynchronizer::synchronize, this));
        m_timer->setDuration(duration);
        m_timer->start();
    }
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::stopping()
{
    if(m_timeStep)
    {
        m_timer->stop();
    }
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::synchronize()
{
    m_updateMask |= SYNC_REQUESTED;

    if(!(m_updateMask & OBJECT_RECEIVED))
    {
        return;
    }

    // Timestamp reference for the synchronization
    core::HiResClock::HiResClockType frameTimestamp = 0;

    // Get timestamp for synchronization
    std::vector<std::size_t> availableFramesTL;

    // If multiple TLs are set, we want to synchronize their frames together.
    // If TLs are updated, we get the one with the oldest timestamp to synchronize them.
    // In particular case, we could have only one TL updated, we still need to get frames from it.
    // Then we get the one with the newest timestamp and the other ones are not considered.
    for(std::size_t i = 0 ; i != m_frameTLs.size() ; ++i)
    {
        core::HiResClock::HiResClockType tlTimestamp;
        {
            const auto tl = m_frameTLs[i].lock();
            SIGHT_ASSERT("Frame TL does not exist", tl);
            tlTimestamp = tl->getNewerTimestamp();
        }
        if(tlTimestamp > 0)
        {
            // Check if the current TL timestamp and the previous one are closed enough (according to the tolerance)
            if(std::abs(frameTimestamp - tlTimestamp) < m_tolerance)
            {
                // Sets the reference timestamp as the minimum value
                frameTimestamp = std::min(frameTimestamp, tlTimestamp);
                availableFramesTL.push_back(i);
            }
            // Otherwise keep the most recent timestamp as frameTimestamp
            else
            {
                // If the difference between the TLs timestamp is superior to the tolerance
                // we set the reference timestamp as the maximum of them
                frameTimestamp = std::max(frameTimestamp, tlTimestamp);
                availableFramesTL.push_back(i);

                // Now remove all frames that are too far away
                availableFramesTL.erase(
                    std::remove_if(
                        availableFramesTL.begin(),
                        availableFramesTL.end(),
                        [ = ](std::size_t const& idx)
                    {
                        const auto frametl = m_frameTLs[idx].lock();
                        SIGHT_ASSERT("Frame TL does not exist", frametl);
                        const auto ts = frametl->getNewerTimestamp();
                        return std::abs(frameTimestamp - ts) >= m_tolerance;
                    }),
                    availableFramesTL.end()
                );
            }
        }
        else
        {
            SIGHT_INFO("no available frame for timeline 'frame" << i << ".");
        }
    }

    // Now we compute the time stamp available in the matrix timelines starting from the frame timestamp
    core::HiResClock::HiResClockType matrixTimestamp = frameTimestamp;

    std::vector<std::size_t> availableMatricesTL;
    availableMatricesTL.reserve(m_matrixTLs.size());
    for(std::size_t i = 0 ; i != m_matrixTLs.size() ; ++i)
    {
        const auto tl = m_matrixTLs[i].lock();
        SIGHT_ASSERT("Matrix TL does not exist", tl);

        core::HiResClock::HiResClockType tlTimestamp = tl->getNewerTimestamp();
        if((tlTimestamp > 0) && (std::abs(frameTimestamp - tlTimestamp) < m_tolerance))
        {
            matrixTimestamp = std::min(matrixTimestamp, tlTimestamp);
            availableMatricesTL.push_back(i);
        }
        else
        {
            SIGHT_INFO_IF("no available matrix for timeline 'matrix" << i << "'.", tlTimestamp > 0);

            // Notify each matrices in the ith TL that they are unsynchronized
            for(const int sendStatus : m_sendMatricesStatus[i])
            {
                if(sendStatus != -1)
                {
                    m_sigMatrixUnsynchronized->asyncEmit(sendStatus);
                }
            }
        }
    }

    // Skip synchronization if nothing has changed or if the synchronizer decided to go back into the past
    if(matrixTimestamp <= m_lastTimestamp)
    {
        // Notify that the synchronization is skipped
        m_sigSynchronizationSkipped->asyncEmit();
        return;
    }

    m_lastTimestamp = matrixTimestamp;

    for(std::size_t i = 0 ; i != m_frameTLs.size() ; ++i)
    {
        const auto image = m_images[i].lock();
        CSPTR(data::FrameTL::BufferType) buffer;
        {
            const auto frameTL = m_frameTLs[i].lock();
            SIGHT_ASSERT("Image with index '" << i << "' does not exist", image);

            const data::Image::Size size = {frameTL->getWidth(), frameTL->getHeight(), 0};
            // Check if image dimensions have changed
            if(size != image->getSize() || frameTL->numComponents() != image->numComponents())
            {
                m_imagesInitialized = false;
            }

            if(!m_imagesInitialized)
            {
                data::Image::PixelFormat format;
                switch(frameTL->getPixelFormat())
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
                        format = data::Image::UNDEFINED;
                        FW_DEPRECATED_MSG(
                            "FrameTL pixel format should be defined, we temporary assume that the format "
                            "is GrayScale, RGB or RGBA according to the number of components.",
                            "22.0"
                        );
                        // FIXME Support old FrameTL API (sight 22.0)
                        switch(frameTL->numComponents())
                        {
                            case 1:
                                format = data::Image::GRAY_SCALE;
                                break;

                            case 3:
                                format = data::Image::RGB;
                                break;

                            case 4:
                                format = data::Image::RGBA;
                                break;

                            default:
                                SIGHT_ERROR("Number of component not managed.")
                                return;
                        }
                }

                image->resize(size, frameTL->getType(), format);
                const data::Image::Origin origin = {0., 0., 0.};
                image->setOrigin(origin);
                const data::Image::Spacing spacing = {1., 1., 1.};
                image->setSpacing(spacing);
                image->setWindowWidth(1);
                image->setWindowCenter(0);

                m_imagesInitialized = true;
            }

            buffer = frameTL->getClosestBuffer(matrixTimestamp - m_delay);
        }

        if(!buffer)
        {
            SIGHT_INFO("Buffer not found for timestamp " << matrixTimestamp << " in timeline 'frame" << i << "'.");
            continue;
        }

        const std::uint8_t* frameBuff = &buffer->getElement(0);
        auto iter                     = image->begin<std::uint8_t>();
        std::memcpy(&*iter, frameBuff, buffer->getSize());

        // Notify
        auto sig = image->signal<data::Image::BufferModifiedSignalType>(
            data::Image::s_BUFFER_MODIFIED_SIG
        );
        sig->asyncEmit();
    }

    bool matrixFound            = false;
    std::size_t syncMatricesNbr = 0;
    for(const auto& tlIdx : availableMatricesTL)
    {
        CSPTR(data::MatrixTL::BufferType) buffer;
        {
            const auto matrixTL = m_matrixTLs[tlIdx].lock();
            buffer = matrixTL->getClosestBuffer(matrixTimestamp);
        }

        if(buffer)
        {
            auto& matrixVector = m_matrices[tlIdx];

            for(std::size_t k = 0 ; k < matrixVector.size() ; ++k)
            {
                const auto matrix = matrixVector[k].lock();
                SIGHT_ASSERT("Matrix with indices '" << tlIdx << ", " << k << "' does not exist", matrix);

                const int sendStatus = m_sendMatricesStatus[tlIdx][k];

                if(buffer->isPresent(static_cast<unsigned int>(k)))
                {
                    const auto& values = buffer->getElement(static_cast<unsigned int>(k));
                    for(std::uint8_t i = 0 ; i < 4 ; ++i)
                    {
                        for(std::uint8_t j = 0 ; j < 4 ; ++j)
                        {
                            matrix->setCoefficient(i, j, static_cast<double>(values[i * 4 + j]));
                        }
                    }

                    if(sendStatus != -1)
                    {
                        m_sigMatrixSynchronized->asyncEmit(sendStatus);
                    }

                    auto sig = matrix->signal<data::Object::ModifiedSignalType>(data::Object::s_MODIFIED_SIG);
                    sig->asyncEmit();

                    matrixFound = true;
                    ++syncMatricesNbr;
                }
                else if(sendStatus != -1)
                {
                    m_sigMatrixUnsynchronized->asyncEmit(sendStatus);
                }
            }
        }
        else
        {
            for(const int sendStatus : m_sendMatricesStatus[tlIdx])
            {
                if(sendStatus != -1)
                {
                    m_sigMatrixUnsynchronized->asyncEmit(sendStatus);
                }
            }
        }
    }

    m_updateMask = m_timeStep != 0 ? OBJECT_RECEIVED : 0;

    if(matrixFound)
    {
        m_sigSynchronizationDone->asyncEmit(matrixTimestamp);
    }

    m_sigAllMatricesFound->asyncEmit(m_totalOutputMatrices == syncMatricesNbr);
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::updating()
{
    m_updateMask |= OBJECT_RECEIVED;

    if(m_updateMask & SYNC_REQUESTED)
    {
        this->synchronize();
    }
}

//-----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::setFrameDelay(int val, std::string key)
{
    if(key == "frameDelay")
    {
        m_delay = val;
    }
    else
    {
        SIGHT_WARN("Unknown key");
    }
}

//-----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::resetTimeline()
{
    // Reset the last timestamp in case the grabber uses a different time scale
    m_lastTimestamp = std::numeric_limits<double>::lowest();
}

// ----------------------------------------------------------------------------

} // namespace sight::module::sync
