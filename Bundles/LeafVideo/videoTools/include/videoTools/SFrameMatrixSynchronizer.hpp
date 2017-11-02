/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SFRAMEMATRIXSYNCHRONIZER_HPP__
#define __VIDEOTOOLS_SFRAMEMATRIXSYNCHRONIZER_HPP__

#include "videoTools/config.hpp"

#include <arServices/ISynchronizer.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>

#include <fwData/Composite.hpp>

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
 * @section Signals Signals
 * - \b synchronizationDone(::fwCore::HiResClock::HiResClockType) : Emitted when the sync is done.
 * - \b allMatricesFound(::fwCore::HiResClock::HiResClockType) : Emitted when the sync is done, contains a boolean to
 *  signal if all the matrices are synchronized.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::videoTools::SFrameMatrixSynchronizer">
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
                <key uid="matrix0" />
                <key uid="matrix1" />
                <key uid="matrix2" />
            </inout>
            <inout group="matrices1">
                <key uid="matrix3" />
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
 * - \b matrices defines the matrixTL to synchronize.
 *   - \b from: key of the matrix timeline to extract matrix.
 *   - \b to: key of the TransformationMatrix3D where to extract the matrix.
 * - \b framerate defines the framerate to call synchronization.
 * - \b tolerance defines the maximum distance between two frames (default = 500).
 *  If a timeline exceeds this tolerance it will not be synchronized.
 */
class VIDEOTOOLS_CLASS_API SFrameMatrixSynchronizer : public ::arServices::ISynchronizer
{
public:

    fwCoreServiceClassDefinitionsMacro((SFrameMatrixSynchronizer)(fwServices::IController));

    /**
     * @name Signal API
     * @{
     */
    typedef ::fwCom::Signal< void (::fwCore::HiResClock::HiResClockType timestamp) > SynchronizationDoneSignalType;
    VIDEOTOOLS_API static const ::fwCom::Signals::SignalKeyType s_SYNCHRONIZATION_DONE_SIG;

    typedef ::fwCom::Signal< void (bool) > AllMatricesFoundSignalType;
    VIDEOTOOLS_API static const ::fwCom::Signals::SignalKeyType s_ALL_MATRICES_FOUND_SIG;
    /** @} */

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API SFrameMatrixSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    VIDEOTOOLS_API virtual ~SFrameMatrixSynchronizer() noexcept
    {
    }

    typedef std::map<std::string, std::string> FrameKeysType;
    typedef std::pair< std::string, unsigned int > MatrixVectorTypePair;
    typedef std::vector<MatrixVectorTypePair> MatrixVectorType;
    typedef std::map<std::string, MatrixVectorType> MatrixKeysType;
    typedef std::map<std::string, CSPTR(::arData::FrameTL)> FrameTLKeyType;
    typedef std::map<std::string, CSPTR(::arData::MatrixTL)> MatrixTLKeyType;
    typedef std::map<std::string, SPTR(::fwData::Image)> ImageKeyType;
    typedef std::pair< SPTR(::fwData::TransformationMatrix3D), unsigned int > MatrixKeyVectorTypePair;
    typedef std::vector<MatrixKeyVectorTypePair> MatrixKeyVectorType;
    typedef std::map<std::string, MatrixKeyVectorType> MatrixKeyType;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
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

    /// Do nothing.
    VIDEOTOOLS_API void updating() override;

private:

    /// Reset the last timestamp when the timeline is cleared
    VIDEOTOOLS_API void resetTimeline();

    /// Tolerance to take into account matrix
    ::fwCore::HiResClock::HiResClockType m_tolerance;

    /// Check if output images are initialized
    bool m_imagesInitialized;

    /// registers FrameTL keys to synchronize and the associated fwData::Image keys
    FrameKeysType m_frameKeys;

    /// registers TrackerTL keys to synchronize and the associated fwData::TransformationMatrix3D keys
    MatrixKeysType m_matrixKeys;
    /// registers frame timeline with its id in composite
    FrameTLKeyType m_frameTLs;
    /// registers matrix timeline with its id in composite
    MatrixTLKeyType m_matrixTLs;
    /// registers frames with associated timeline key
    ImageKeyType m_images;
    /// registers matrices with associated timeline key
    MatrixKeyType m_matrices;

    /// Time step used for the update
    unsigned int m_timeStep;

    /// Total number of matrices in output (used to know if all the matrices are synchronized)
    size_t m_totalOutputMatrices;

    /// Timer used for the update
    ::fwThread::Timer::sptr m_timer;

    /// Signal emitted when the synchronization of the frame timeline and the matrix timeline is done.
    SynchronizationDoneSignalType::sptr m_sigSynchronizationDone;

    /// Signal emitted when the synchronization is done, contains a boolean to signal if all the matrices
    ///  are synchronized.
    AllMatricesFoundSignalType::sptr m_sigAllMatricesFound;

    /// Remember last time stamp to skip synchronization if nothing has changed
    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;
};

} // namespace videoTools
#endif  // __VIDEOTOOLS_SFRAMEMATRIXSYNCHRONIZER_HPP__
