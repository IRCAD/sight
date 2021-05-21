/************************************************************************
 *
 * Copyright (C) 2014-2021 IRCAD France
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

#pragma once

#include "modules/sync/config.hpp"

#include <core/base.hpp>
#include <core/com/Signal.hpp>
#include <core/com/Signals.hpp>
#include <core/HiResClock.hpp>
#include <core/thread/Timer.hpp>

#include <service/IController.hpp>
#include <service/ISynchronizer.hpp>

namespace sight::data
{

class FrameTL;
class MatrixTL;

}

namespace sight::data
{

class Image;
class Matrix4;

}

namespace sight::module::sync
{

/**
 * @brief SFrameMatrixSynchronizer service synchronizes video frame and tracking matrix.
 *
 * @note
 * `autoConnect="true"` is required in order to play nice with streams whose timelines might get reset, e.g. when
 * replaying a recorded video several times.
 *
 * @section Signals Signals
 * - \b synchronizationDone(core::HiResClock::HiResClockType) : Emitted when the sync is done.
 * - \b synchronizationSkipped(core::HiResClock::HiResClockType) : Emitted when a sync loop is skipped if nothing
 * has changed or if the synchronizer decided to go back into the past.
 * - \b allMatricesFound(core::HiResClock::HiResClockType) : Emitted when the sync is done, contains a boolean to
 *  signal if all the matrices are synchronized.
 * - \b matrixSynchronized(int): Emitted when the matrix is synchronized, contains the index of the matrix with
 *  `sendStatus` set to "on", in the declaration order.
 * - \b matrixUnsynchronized(int): Emitted when the matrix is not present in the buffer and can not be synchronized,
 *  contains the index of the matrix with `sendStatus` set to "on", in the declaration order.
 *
 * @section Slots Slots
 * - \b synchronize(): Actual synchronization function.
 * - \b setFrameDelay(int, std::string): Apply delay between frames and matrices
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="sight::module::sync::SFrameMatrixSynchronizer" autoConnect="true">
            <in group="frameTL">
                <key uid="frameTL1" />
                <key uid="frameTL4" />
                <key uid="frameTL6" />
            </in>
            <inout group="image">
                <key uid="videoImage1" />
                <key uid="videoImage2" />
                <key uid="videoImage3" />
            </inout>
            <in group="matrixTL">
                <key uid="matrixTL1" />
                <key uid="matrixTL2" />
            </in>
            <inout group="matrices0">
                <key uid="matrix0" sendStatus="true"/>
                <key uid="matrix1" />
                <key uid="matrix2" />
            </inout>
            <inout group="matrices1">
                <key uid="matrix3" sendStatus="false"/>
                <key uid="matrix4" />
            </inout>
            <framerate>30</framerate>
            <tolerance>500</tolerance>
       </service>
   @endcode
 * @subsection Input Input
 * - \b frameTL [sight::data::FrameTL]: defines the frameTL to synchronize. The number of \b frameTL keys must match the
 * number of \b image keys.
 * - \b matrixTL [::extData::MatrixTL]: defines the matrixTL to synchronize. The number of \b matrixTL keys must match
 * the number of \b matricesX group.
 *
 * @subsection In-Out In-Out
 * - \b image [sight::data::Image]: defines the images where to extract the image. The number of \b image keys must
 * match
 * the number of \b frameTL keys.
 * - \b matricesX [sight::data::Matrix4]: defines the matrices where to extract the matrices from the
 * timeline. X must be replaced by the index of the associated \b MatrixTL key (index begin at 0).
 *
 * @subsection Configuration Configuration
 * - \b sendStatus defines for each matrix if we want to send the given matrix/Un/Synchronized (default: false)
 * - \b framerate defines the framerate to call synchronization (default: 30). If it is set to 0, then the service does
 * not synchronize periodically. You'll have then to call the slot "synchronize" yourself.
 * - \b tolerance defines the maximum distance between two frames (default: 500).
 *  If a timeline exceeds this tolerance it will not be synchronized.
 */
class MODULE_SYNC_CLASS_API SFrameMatrixSynchronizer : public service::ISynchronizer
{
public:

    SIGHT_DECLARE_SERVICE(SFrameMatrixSynchronizer, service::IController)

    /**
     * @name Signal API
     * @{
     */
    typedef core::com::Signal<void (core::HiResClock::HiResClockType timestamp)> SynchronizationDoneSignalType;
    MODULE_SYNC_API static const core::com::Signals::SignalKeyType s_SYNCHRONIZATION_DONE_SIG;

    typedef core::com::Signal<void (void)> synchronizationSkippedSignalType;
    MODULE_SYNC_API static const core::com::Signals::SignalKeyType s_SYNCHRONIZATION_SKIPPED_SIG;

    typedef core::com::Signal<void (bool)> AllMatricesFoundSignalType;
    MODULE_SYNC_API static const core::com::Signals::SignalKeyType s_ALL_MATRICES_FOUND_SIG;

    typedef core::com::Signal<void (int)> MatrixSynchronizedSignalType;
    MODULE_SYNC_API static const core::com::Signals::SignalKeyType s_MATRIX_SYNCHRONIZED_SIG;

    typedef core::com::Signal<void (int)> MatrixUnsynchronizedSignalType;
    MODULE_SYNC_API static const core::com::Signals::SignalKeyType s_MATRIX_UNSYNCHRONIZED_SIG;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    MODULE_SYNC_API static const core::com::Slots::SlotKeyType s_SYNCHRONIZE_SLOT;
    MODULE_SYNC_API static const core::com::Slots::SlotKeyType s_SET_FRAME_DELAY_SLOT;
    /** @} */

    /**
     * @brief Constructor.
     */
    MODULE_SYNC_API SFrameMatrixSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    MODULE_SYNC_API ~SFrameMatrixSynchronizer() noexcept override;

    /**
     * @brief Return proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connections
     *
     * Connect data::TimeLine::s_CLEARED_SIG to s_RESET_TIMELINE_SLOT
     */
    MODULE_SYNC_API service::IService::KeyConnectionsMap getAutoConnections() const override;

protected:

    /**
     * @brief This method is used to configure the service.
     */
    MODULE_SYNC_API void configuring() override;

    /// This method is used to initialize the timer and get the TLs & Matrixes.
    MODULE_SYNC_API void starting() override;

    /// Stops timer and clear TLs & Matrixes.
    MODULE_SYNC_API void stopping() override;

    /// Synchronizes TLs
    MODULE_SYNC_API void synchronize();

    /// Called when new objects are pushed into the timeline.
    MODULE_SYNC_API void updating() override;

    /// Set a delay between frames and matrices.
    MODULE_SYNC_API void setFrameDelay(int val, std::string key);

private:

    /// Reset the last timestamp when the timeline is cleared
    MODULE_SYNC_API void resetTimeline();

    enum : std::uint8_t
    {
        OBJECT_RECEIVED = 0x1,
        SYNC_REQUESTED  = 0x2
    };

    std::uint8_t m_updateMask {SYNC_REQUESTED};

    /// Tolerance to take into account matrix
    core::HiResClock::HiResClockType m_tolerance;

    /// Check if output images are initialized
    bool m_imagesInitialized;

    /// Contains the input video timelines.
    std::vector<data::mt::weak_ptr<const data::FrameTL> > m_frameTLs;

    /// Contains the input matrix timelines.
    std::vector<data::mt::weak_ptr<const data::MatrixTL> > m_matrixTLs;

    /// Contains the output images.
    std::vector<data::mt::weak_ptr<data::Image> > m_images;

    /// registers matrices with associated timeline key
    std::vector<std::vector<data::mt::weak_ptr<data::Matrix4> > > m_matrices;

    /// registers index of matrices that need to send their status through signals
    std::vector<std::vector<int> > m_sendMatricesStatus;

    /// Time step used for the update
    unsigned int m_timeStep;

    /// Total number of matrices in output (used to know if all the matrices are synchronized)
    size_t m_totalOutputMatrices;

    /// Delay in milliseconds between frames and matrices
    int m_delay {0};

    /// Timer used for the update
    core::thread::Timer::sptr m_timer;

    /// Signal emitted when the synchronization of the frame timeline and the matrix timeline is done.
    SynchronizationDoneSignalType::sptr m_sigSynchronizationDone;

    /// Signal emitted when the synchronization of the frame timeline and the matrix timeline are skipped.
    synchronizationSkippedSignalType::sptr m_sigSynchronizationSkipped;

    /// Signal emitted when the synchronization is done, contains a boolean to signal if all the matrices
    /// are synchronized.
    AllMatricesFoundSignalType::sptr m_sigAllMatricesFound;

    /// Signal emitted when the matrix is synchronized, contains the index of the matrix with `sendStatus` set to "true"
    /// in the declaration order.
    MatrixSynchronizedSignalType::sptr m_sigMatrixSynchronized;

    /// Emitted when the matrix is not present in the buffer and can not be synchronized, contains the index of the
    /// matrix with `sendStatus` set to "true", in the declaration order.
    MatrixUnsynchronizedSignalType::sptr m_sigMatrixUnsynchronized;

    /// Remember last time stamp to skip synchronization if nothing has changed
    core::HiResClock::HiResClockType m_lastTimestamp;
};

} // namespace sight::module::sync
