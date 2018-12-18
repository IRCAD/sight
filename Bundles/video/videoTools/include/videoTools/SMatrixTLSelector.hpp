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

#pragma once

#include "videoTools/config.hpp"

#include <arServices/ISynchronizer.hpp>

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
 * @brief   SMatrixTLSelector service synchronizes video frame and tracking matrixes.
 * @deprecated This service will be removed in 19.0.
 */
class VIDEOTOOLS_CLASS_API SMatrixTLSelector : public ::arServices::ISynchronizer
{

public:

    fwCoreServiceClassDefinitionsMacro((SMatrixTLSelector)(fwServices::IController));

    /**
     * @brief Constructor.
     */
    VIDEOTOOLS_API SMatrixTLSelector() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SMatrixTLSelector() noexcept
    {
    }

    class MatrixCfgType
    {
    public:
        size_t m_index;

        std::string m_matrixID;
        SPTR(::fwData::TransformationMatrix3D) m_matrix;

        std::vector<std::string> m_timelineKeyVect;
        /// registers matrix timeline with its id in composite
        std::map<std::string, SPTR(::arData::MatrixTL)> m_timelineObjectMap;
    };

    typedef std::vector< MatrixCfgType > MatrixCfgVectorType;

    typedef std::map<std::string, std::string> FrameKeysType;

    typedef std::map<std::string, SPTR(::arData::FrameTL)> FrameTLKeyType;
    typedef std::map<std::string, SPTR(::fwData::Image)> ImageKeyType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @code{.xml}
        <service uid="synchronizer" impl="::videoTools::SMatrixTLSelector" autoConnect="no" worker="videoWorker">
            <frames>
                <frame from="frameTL" to="image" />
            </frames>
            <matrices>
                <matrix index="0" to="transform" />
                    <timeline from="matrixTL1"/>
                    <timeline from="matrixTL2"/>
                    <timeline from="matrixTL3"/>
                </matrix>
                <matrix index="0" to="transform2" />
                    <timeline from="matrixTL11"/>
                    <timeline from="matrixTL114"/>
                </matrix>
                <matrix index="0" to="transform4" />
                    <timeline from="matrixTL11"/>
                </matrix>
            </matrices>
            <framerate>60</framerate>
        </service>

       @endcode
     */
    VIDEOTOOLS_API void configuring() override;

    /// This method is used to initialize the service.
    VIDEOTOOLS_API void starting() override;

    /// Does nothing.
    VIDEOTOOLS_API void stopping() override;

    /// Does nothing.
    void updating() override
    {
    }

private:

    /// Synchronize
    void synchronize();

    void updateFrames(::fwCore::HiResClock::HiResClockType timestamp);

    ::fwCore::HiResClock::HiResClockType m_lastTimestamp;
    ::fwCore::HiResClock::HiResClockType m_tolerance; ///< Tolerance to take into account matrix

    bool m_imagesInitialized;

    ///< registers matrices with associated timeline key
    MatrixCfgVectorType m_matrixCfgVector;

    /// registers FrameTL keys to synchronize and the associated fwData::Image keys
    FrameKeysType m_frameKeys;

    ///< registers frame timeline with its id in composite
    FrameTLKeyType m_frameTLs;

    ///< registers frames with associated timeline key
    ImageKeyType m_images;

    ///< Time step used for the update
    unsigned int m_timeStep;

    ///< Timer used for the update
    ::fwThread::Timer::sptr m_timer;
};

} //namespace videoTools
