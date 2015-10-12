/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOTOOLS_SFRAMEMATRIXSYNCHRONIZER_HPP__
#define __VIDEOTOOLS_SFRAMEMATRIXSYNCHRONIZER_HPP__

#include "videoTools/config.hpp"

#include <arServices/ISynchronizer.hpp>
#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>
#include <fwData/Composite.hpp>
#include <fwServices/IController.hpp>
#include <fwThread/Timer.hpp>


namespace extData
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
 * @brief   SFrameMatrixSynchronizer service synchronizes video frame and tracking matrix.
 * @class   SFrameMatrixSynchronizer
 */
class VIDEOTOOLS_CLASS_API SFrameMatrixSynchronizer : public ::arServices::ISynchronizer
{

public:

    fwCoreServiceClassDefinitionsMacro((SFrameMatrixSynchronizer)(fwServices::IController));

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API SFrameMatrixSynchronizer() throw ();

    /**
     * @brief Destructor.
     */
    VIDEOTOOLS_API virtual ~SFrameMatrixSynchronizer() throw ()
    {
    }

    typedef std::map<std::string, std::string> FrameKeysType;
    typedef std::pair< std::string, unsigned int > MatrixVectorTypePair;
    typedef std::vector<MatrixVectorTypePair> MatrixVectorType;
    typedef std::map<std::string, MatrixVectorType> MatrixKeysType;
    typedef std::map<std::string, SPTR(::extData::FrameTL)> FrameTLKeyType;
    typedef std::map<std::string, SPTR(::extData::MatrixTL)> MatrixTLKeyType;
    typedef std::map<std::string, SPTR(::fwData::Image)> ImageKeyType;
    typedef std::pair< SPTR(::fwData::TransformationMatrix3D), unsigned int > MatrixKeyVectorTypePair;
    typedef std::vector<MatrixKeyVectorTypePair> MatrixKeyVectorType;
    typedef std::map<std::string, MatrixKeyVectorType> MatrixKeyType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @verbatim
       <service impl="::videoTools::SFrameMatrixSynchronizer" type="::arServices::ISynchronizer">
           <frames>
               <frame from="frame1TL" to="frame1" />
               <frame from="frame2TL" to="frame2" />
           </frames>
           <matrices>
               <timeline from="matrixTL1">
                   <matrix index="0" to="matrix0" />
                   <matrix index="1" to="matrix1" />
                   <matrix index="2" to="matrix2" />
               </timeline>
               <timeline from="matrixTL2">
                   <matrix index="0" to="matrix3" />
                   <matrix index="8" to="matrix4" />
               </timeline>
           </matrices>
           <framerate>30</framerate>
       </service>
       @endverbatim
     * - \b frames defines the frameTL to synchronize.
     *   - \b from: key of the frame timeline to extract image.
     *   - \b to: key of the image where to extract the image.
     * - \b matrices defines the matrixTL to synchronize.
     *   - \b from: key of the matrix timeline to extract matrix.
     *   - \b to: key of the TransformationMatrix3D where to extract the matrix.
     * - \b framerate defines the framerate to call synchronization.
     * @note If the matrix timeline last update delay is greater than 500ms, the matrix is not synchronized.
     */
    VIDEOTOOLS_API void configuring() throw (fwTools::Failed);

    /// This method is used to initialize the service.
    VIDEOTOOLS_API void starting() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOTOOLS_API void stopping() throw (fwTools::Failed);

    /// Does nothing.
    VIDEOTOOLS_API void swapping() throw (fwTools::Failed)
    {
    }

    /// Does nothing.
    VIDEOTOOLS_API void updating() throw (fwTools::Failed)
    {
    }

    /// Synchronize
    VIDEOTOOLS_API void synchronize();

private:

    ::fwCore::HiResClock::HiResClockType m_tolerance; ///< Tolerance to take into account matrix

    bool m_imagesInitialized;

    /// registers FrameTL keys to synchronize and the associated fwData::Image keys
    FrameKeysType m_frameKeys;

    /// registers TrackerTL keys to synchronize and the associated fwData::TransformationMatrix3D keys
    MatrixKeysType m_matrixKeys;
    ///< registers frame timeline with its id in composite
    FrameTLKeyType m_frameTLs;
    ///< registers matrix timeline with its id in composite
    MatrixTLKeyType m_matrixTLs;
    ///< registers frames with associated timeline key
    ImageKeyType m_images;
    ///< registers matrices with associated timeline key
    MatrixKeyType m_matrices;

    ///< Time step used for the update
    unsigned int m_timeStep;

    ///< Timer used for the update
    ::fwThread::Timer::sptr m_timer;
};

} //namespace videoTools
#endif  // __VIDEOTOOLS_SFRAMEMATRIXSYNCHRONIZER_HPP__
