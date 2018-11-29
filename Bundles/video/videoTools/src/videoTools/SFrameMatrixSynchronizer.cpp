/************************************************************************
 *
 * Copyright (C) 2014-2018 IRCAD France
 * Copyright (C) 2014-2018 IHU Strasbourg
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

#include "videoTools/SFrameMatrixSynchronizer.hpp"

#include <arData/FrameTL.hpp>
#include <arData/MatrixTL.hpp>
#include <arData/timeline/Buffer.hpp>

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Image.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwMath/Compare.hpp>

#include <fwRuntime/ConfigurationElement.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/fwID.hpp>

#include <algorithm>
#include <functional>

fwServicesRegisterMacro(::arServices::ISynchronizer, ::videoTools::SFrameMatrixSynchronizer);

namespace videoTools
{

const ::fwCom::Signals::SignalKeyType SFrameMatrixSynchronizer::s_SYNCHRONIZATION_DONE_SIG = "synchronizationDone";
const ::fwCom::Signals::SignalKeyType SFrameMatrixSynchronizer::s_ALL_MATRICES_FOUND_SIG   = "allMatricesFound";

const ::fwServices::IService::KeyType s_FRAMETL_INPUT  = "frameTL";
const ::fwServices::IService::KeyType s_MATRIXTL_INPUT = "matrixTL";
const ::fwServices::IService::KeyType s_IMAGE_INOUT    = "image";
const ::fwServices::IService::KeyType s_MATRICES_INOUT = "matrices";

// Private slot
const ::fwCom::Slots::SlotKeyType s_RESET_TIMELINE_SLOT = "reset";
const ::fwCom::Slots::SlotKeyType s_SYNCHRONIZE_SLOT    = "synchronize";

// ----------------------------------------------------------------------------

SFrameMatrixSynchronizer::SFrameMatrixSynchronizer() noexcept :
    m_tolerance(500.),
    m_imagesInitialized(false),
    m_timeStep(33),
    m_lastTimestamp(std::numeric_limits<double>::lowest())
{
    m_sigSynchronizationDone = newSignal<SynchronizationDoneSignalType>(s_SYNCHRONIZATION_DONE_SIG);
    m_sigAllMatricesFound    = newSignal<AllMatricesFoundSignalType>(s_ALL_MATRICES_FOUND_SIG);

    newSlot(s_RESET_TIMELINE_SLOT, &SFrameMatrixSynchronizer::resetTimeline, this);
    newSlot(s_SYNCHRONIZE_SLOT, &SFrameMatrixSynchronizer::synchronize, this);
}

//-----------------------------------------------------------------------------

SFrameMatrixSynchronizer::~SFrameMatrixSynchronizer() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SFrameMatrixSynchronizer::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_FRAMETL_INPUT, ::arData::TimeLine::s_CLEARED_SIG, s_RESET_TIMELINE_SLOT );
    connections.push( s_FRAMETL_INPUT, ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT );
    connections.push( s_MATRIXTL_INPUT, ::arData::TimeLine::s_OBJECT_PUSHED_SIG, s_UPDATE_SLOT );
    return connections;
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::configuring()
{
    const auto cfg = this->getConfigTree();

    const auto framerate = cfg.get<unsigned int>("framerate", 30);
    m_timeStep  = framerate != 0 ? 1000 / cfg.get<unsigned int>("framerate", 30) : 0;
    m_tolerance = cfg.get<unsigned int>("tolerance", 500);

    m_updateMask = framerate != 0 ? OBJECT_RECEIVED : m_updateMask;
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::starting()
{
    const size_t numFrameTLs = this->getKeyGroupSize(s_FRAMETL_INPUT);
    const size_t numImages   = this->getKeyGroupSize(s_IMAGE_INOUT);
    SLM_ASSERT("You should have the same number of 'frameTL' and 'image' keys", numFrameTLs == numImages);

    m_frameTLs.reserve(numFrameTLs);
    m_images.reserve(numFrameTLs);
    for(size_t i = 0; i < numFrameTLs; ++i)
    {
        m_frameTLs.push_back(this->getInput< ::arData::FrameTL>(s_FRAMETL_INPUT, i));
        m_images.push_back(this->getInOut< ::fwData::Image>(s_IMAGE_INOUT, i));
    }

    const size_t numMatrixTLs = this->getKeyGroupSize(s_MATRIXTL_INPUT);

    m_totalOutputMatrices = 0;
    m_matrixTLs.reserve(numMatrixTLs);
    for(size_t i = 0; i < numMatrixTLs; ++i)
    {
        // Get the group corresponding to the 'i' Matrix TimeLine. The name of this group is matrices0 for example.
        // if ever the group is not found 'getKeyGroupSize' will assert.
        const size_t numMatrices = this->getKeyGroupSize(s_MATRICES_INOUT + std::to_string(i));

        m_matrixTLs.push_back(this->getInput< ::arData::MatrixTL>(s_MATRIXTL_INPUT, i));

        std::vector< ::fwData::TransformationMatrix3D::sptr> matricesVector;
        for(size_t j = 0; j < numMatrices; ++j)
        {
            matricesVector.push_back(
                this->getInOut< ::fwData::TransformationMatrix3D >( s_MATRICES_INOUT + std::to_string(i), j));
        }
        m_totalOutputMatrices += numMatrices;
        m_matrices.push_back(matricesVector);
    }

    SLM_ASSERT("No valid worker for timer.", m_associatedWorker);
    if(m_timeStep)
    {
        m_timer = m_associatedWorker->createTimer();
        const auto duration = std::chrono::milliseconds(m_timeStep);
        m_timer->setFunction(std::bind( &SFrameMatrixSynchronizer::synchronize, this));
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

    m_frameTLs.clear();
    m_images.clear();
    m_matrixTLs.clear();
    m_matrices.clear();
}

// ----------------------------------------------------------------------------

void SFrameMatrixSynchronizer::synchronize()
{
    m_updateMask |= SYNC_REQUESTED;

    if( !(m_updateMask & OBJECT_RECEIVED) )
    {
        return;
    }

    // Timestamp reference for the synchronization
    ::fwCore::HiResClock::HiResClockType frameTimestamp = 0;

    // Get timestamp for synchronization
    std::vector<size_t> availableFramesTL;

    // If multiple TLs are set, we want to synchronize their frames together.
    // If TLs are updated, we get the one with the oldest timestamp to synchronize them.
    // In particular case, we could have only one TL updated, we still need to get frames from it.
    // Then we get the one with the newest timestamp and the other ones are not considered.
    for(size_t i = 0; i != m_frameTLs.size(); ++i)
    {
        const auto& tl = m_frameTLs[i];
        ::fwCore::HiResClock::HiResClockType tlTimestamp = tl->getNewerTimestamp();
        if(tlTimestamp > 0)
        {
            // Check if the current TL timestamp and the previous one are closed enough (according to the tolerance)
            if (std::abs(frameTimestamp - tlTimestamp) < m_tolerance)
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
                availableFramesTL.erase(std::remove_if(availableFramesTL.begin(), availableFramesTL.end(),
                                                       [ = ](size_t const& idx)
                    {
                        const auto ts = m_frameTLs[idx]->getNewerTimestamp();
                        return std::abs(frameTimestamp - ts) >= m_tolerance;
                    }), availableFramesTL.end());
            }
        }
        else
        {
            OSLM_INFO("no available frame for timeline 'frame" << i << ".");
        }
    }

    // Now we compute the time stamp available in the matrix timelines starting from the frame timestamp
    ::fwCore::HiResClock::HiResClockType matrixTimestamp = frameTimestamp;

    std::vector<size_t> availableMatricesTL;
    availableMatricesTL.reserve(m_matrixTLs.size());
    for(size_t i = 0; i != m_matrixTLs.size(); ++i)
    {
        const auto& tl = m_matrixTLs[i];
        ::fwCore::HiResClock::HiResClockType tlTimestamp = tl->getNewerTimestamp();
        if(tlTimestamp > 0)
        {
            if (std::abs(frameTimestamp - tlTimestamp) < m_tolerance)
            {
                matrixTimestamp = std::min(matrixTimestamp, tlTimestamp);
                availableMatricesTL.push_back(i);
            }
        }
        else
        {
            OSLM_INFO("no available matrix for timeline 'matrix" << i << "'.");
        }
    }

    // Skip synchzonization if nothing has changed or if the synchronizer decided to go back into the past
    if(matrixTimestamp <= m_lastTimestamp)
    {
        return;
    }

    m_lastTimestamp = matrixTimestamp;

    for(size_t i = 0; i != m_frameTLs.size(); ++i)
    {
        ::arData::FrameTL::csptr frameTL = m_frameTLs[i];
        ::fwData::Image::sptr image      = m_images[i];

        ::fwData::Image::SizeType size(2);
        size[0] = frameTL->getWidth();
        size[1] = frameTL->getHeight();
        // Check if image dimensions have changed
        if(size != image->getSize())
        {
            m_imagesInitialized = false;
        }

        if(!m_imagesInitialized)
        {
            const ::fwData::Image::SpacingType::value_type voxelSize = 1;
            image->allocate(size, frameTL->getType(), frameTL->getNumberOfComponents());
            ::fwData::Image::OriginType origin(2, 0);

            image->setOrigin(origin);
            ::fwData::Image::SpacingType spacing(2, voxelSize);
            image->setSpacing(spacing);
            image->setWindowWidth(1);
            image->setWindowCenter(0);

            m_imagesInitialized = true;
        }

        ::fwData::mt::ObjectWriteLock destLock(image);
        ::fwData::Array::sptr array = image->getDataArray();
        ::fwDataTools::helper::Array arrayHelper(array);
        CSPTR(::arData::FrameTL::BufferType) buffer = frameTL->getClosestBuffer(matrixTimestamp);

        if(!buffer)
        {
            OSLM_INFO("Buffer not found for timestamp "<< matrixTimestamp << " in timeline 'frame" << i << "'.");
            continue;
        }

        const std::uint8_t* frameBuff = &buffer->getElement(0);
        std::uint8_t* index           = arrayHelper.begin< std::uint8_t >();
        std::copy( frameBuff, frameBuff+buffer->getSize(), index);

        // Notify
        auto sig = image->signal< ::fwData::Image::BufferModifiedSignalType >(
            ::fwData::Image::s_BUFFER_MODIFIED_SIG );
        sig->asyncEmit();
    }

    bool matrixFound       = false;
    size_t syncMatricesNbr = 0;
    for(const auto& tlIdx: availableMatricesTL)
    {
        const auto& matrixTL = m_matrixTLs[tlIdx];
        CSPTR(::arData::MatrixTL::BufferType) buffer = matrixTL->getClosestBuffer(matrixTimestamp);

        if(buffer)
        {
            const auto& matrixVector = m_matrices[tlIdx];

            for(unsigned int k = 0; k < matrixVector.size(); ++k)
            {
                ::fwData::TransformationMatrix3D::sptr const& matrix = matrixVector[k];

                if(buffer->isPresent(k))
                {
                    const auto& values = buffer->getElement(k);
                    for(std::uint8_t i = 0; i < 4; ++i)
                    {
                        for(std::uint8_t j = 0; j < 4; ++j)
                        {
                            matrix->setCoefficient(i, j, static_cast<double>(values[i * 4 + j]));
                        }
                    }

                    auto sig = matrix->signal< ::fwData::Object::ModifiedSignalType >(
                        ::fwData::Object::s_MODIFIED_SIG);
                    sig->asyncEmit();

                    matrixFound = true;
                    ++syncMatricesNbr;
                }
            }
        }
    }

    m_updateMask = m_timeStep != 0 ? OBJECT_RECEIVED : 0;

    if (matrixFound)
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

void SFrameMatrixSynchronizer::resetTimeline()
{
    // Reset the last timestamp in case the grabber uses a different time scale
    m_lastTimestamp = std::numeric_limits<double>::lowest();
}

// ----------------------------------------------------------------------------

}  // namespace videoTools
