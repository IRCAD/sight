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

#pragma once

#include "videoTools/config.hpp"

#include <arServices/ISynchronizer.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>

#include <fwServices/IController.hpp>

#include <fwThread/Timer.hpp>

namespace arData
{
class FrameTL;
class MatrixTL;
}

namespace fwData
{
class Image;
class TransformationMatrix3D;
}

namespace videoTools
{
/**
 * @brief SFrameMatrixSynchronizer service synchronizes video frame and tracking matrix.
 *
 * @note
 * `autoConnect="yes"` is required in order to play nice with streams whose timelines might get reset, e.g. when
 * replaying a recorded video several times.
 *
 * @section Signals Signals
 * - \b synchronizationDone(::fwCore::HiResClock::HiResClockType) : Emitted when the sync is done.
 * - \b synchronizationSkipped(::fwCore::HiResClock::HiResClockType) : Emitted when a sync loop is skipped if nothing
 * has changed or if the synchronizer decided to go back into the past.
 * - \b allMatricesFound(::fwCore::HiResClock::HiResClockType) : Emitted when the sync is done, contains a boolean to
 *  signal if all the matrices are synchronized.
 * - \b matrixSynchronized(int): Emitted when the matrix is synchronized, contains the index of the matrix with
 *  `sendStatus` set to "on", in the decleration order.
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
        <service type="::videoTools::SFrameMatrixSynchronizer" autoConnect="yes">
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
 * - \b frameTL [::arData::FrameTL]: defines the frameTL to synchronize. The number of \b frameTL keys must match the
 * number of \b image keys.
 * - \b matrixTL [::extData::MatrixTL]: defines the matrixTL to synchronize. The number of \b matrixTL keys must match
 * the number of \b matricesX group.
 *
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: defines the images where to extract the image. The number of \b image keys must match
 * the number of \b frameTL keys.
 * - \b matricesX [::fwData::TransformationMatrix3D]: defines the matrices where to extract the matrices from the
 * timeline. X must be replaced by the index of the associated \b MatrixTL key (index begin at 0).
 *
 * @subsection Configuration Configuration
 * - \b sendStatus defines for each matrix if we want to send the given matrix/Un/Synchronized (default: false)
 * - \b framerate defines the framerate to call synchronization (default: 30). If it is set to 0, then the service does
 * not synchronize periodically. You'll have then to call the slot "synchronize" yourself.
 * - \b tolerance defines the maximum distance between two frames (default: 500).
 *  If a timeline exceeds this tolerance it will not be synchronized.
 */
class VIDEOTOOLS_CLASS_API SFrameMatrixSynchronizer : public ::arServices::ISynchronizer
{
public:

    fwCoreServiceClassDefinitionsMacro((SFrameMatrixSynchronizer)(fwServices::IController))

    /**
     * @name Signal API
     * @{
     */
    typedef ::fwCom::Signal< void (::fwCore::HiResClock::HiResClockType timestamp) > SynchronizationDoneSignalType;
    VIDEOTOOLS_API static const ::fwCom::Signals::SignalKeyType s_SYNCHRONIZATION_DONE_SIG;

    typedef ::fwCom::Signal< void (void) > synchronizationSkippedSignalType;
    VIDEOTOOLS_API static const ::fwCom::Signals::SignalKeyType s_SYNCHRONIZATION_SKIPPED_SIG;

    typedef ::fwCom::Signal< void (bool) > AllMatricesFoundSignalType;
    VIDEOTOOLS_API static const ::fwCom::Signals::SignalKeyType s_ALL_MATRICES_FOUND_SIG;

    typedef ::fwCom::Signal< void (int) > MatrixSynchronizedSignalType;
    VIDEOTOOLS_API static const ::fwCom::Signals::SignalKeyType s_MATRIX_SYNCHRONIZED_SIG;

    typedef ::fwCom::Signal< void (int) > MatrixUnsynchronizedSignalType;
    VIDEOTOOLS_API static const ::fwCom::Signals::SignalKeyType s_MATRIX_UNSYNCHRONIZED_SIG;
    /** @} */

    /**
     * @name Slots API
     * @{
     */
    VIDEOTOOLS_API static const ::fwCom::Slots::SlotKeyType s_SYNCHRONIZE_SLOT;
    VIDEOTOOLS_API static const ::fwCom::Slots::SlotKeyType s_SET_FRAME_DELAY_SLOT;
    /** @} */

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API SFrameMatrixSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    VIDEOTOOLS_API virtual ~SFrameMatrixSynchronizer() noexcept override;

    /**
     * @brief Return proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connections
     *
     * Connect ::arData::TimeLine::s_CLEARED_SIG to s_RESET_TIMELINE_SLOT
     */
    VIDEOTOOLS_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

protected:
    /**
     * @brief This method is used to configure the service.
     */
    VIDEOTOOLS_API void configuring() override;

    /// This method is used to initialize the timer and get the TLs & Matrixes.
    VIDEOTOOLS_API void starting() override;

    /// Stops timer and clear TLs & Matrixes.
    VIDEOTOOLS_API void stopping() override;

    /// Synchronizes TLs
    VIDEOTOOLS_API void synchronize();

    /// Called when new objects are pushed into the timeline.
    VIDEOTOOLS_API void updating() override;

    /// Set a delay between frames and matrices.
    VIDEOTOOLS_API void setFrameDelay(int val, std::string key);

private:

    /// Reset the last timestamp when the timeline is cleared
    VIDEOTOOLS_API void resetTimeline();

    enum : std::uint8_t
    {
        OBJECT_RECEIVED = 0x1,
        SYNC_REQUESTED  = 0x2
    };

    std::uint8_t m_updateMask { SYNC_REQUESTED };

    /// Tolerance to take into account matrix
    ::fwCore::HiResClock::HiResClockType m_tolerance;

    /// Check if output images are initialized
    bool m_imagesInitialized;

    /// Contains the input video timelines.
    std::vector<CSPTR(::arData::FrameTL)> m_frameTLs;
    /// Contains the input matrix timelines.
    std::vector<CSPTR(::arData::MatrixTL)> m_matrixTLs;
    /// Contains the output images.
    std::vector<SPTR(::fwData::Image)> m_images;
    /// registers matrices with associated timeline key
    std::vector<std::vector<SPTR(::fwData::TransformationMatrix3D)> > m_matrices;
    /// registers index of matrices that need to send their status through signals
    std::vector<std::vector<int> > m_sendMatricesStatus;

    /// Time step used for the update
    unsigned int m_timeStep;

    /// Total number of matrices in output (used to know if all the matrices are synchronized)
    size_t m_totalOutputMatrices;

    /// Delay in milliseconds between frames and matrices
    int m_delay { 0 };

    /// Timer used for the update
    ::fwThread::Timer::sptr m_timer;

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
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;
};

} // namespace videoTools
