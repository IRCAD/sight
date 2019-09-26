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

#include "syncTimeline/config.hpp"

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

namespace syncTimeline
{
/**
 * @brief   SMatrixTLSynchronizer service synchronizes tracking matrices.
 *
 * @section Signals Signals
 * - \b matrixSynchronized(): Emitted when the  matrix is synchronized
 * - \b matrixUnsynchronized(): Emitted when the matrix is not present in the buffer and can not be synchronized
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="matrixToolsSynchronizer" type="::syncTimeline::SMatrixTLSynchronizer">
         <in key="matrixTL" uid="matrixToolsTL" autoConnect="yes" />
         <inout group="matrices">
             <key uid="markerEndoMX" />
             <key uid="tipEndoMX" />
             <key uid="markerTool1MX" />
             <key uid="tipTool1MX" />
             <key uid="markerTool2MX" />
             <key uid="tipTool2MX" />
         </inout>
     </service>
   @endcode
 * @subsection Input Input
 * - \b matrixTL [::arData::MatrixTL]: matrix timeline used to extract matrices.
 * @subsection In-Out In-Out
 * - \b matrices [::fwData::TransformationMatrix3D]: list of TransformationMatrix3D used to store extracted matrices.
 */
class SYNCTIMELINE_CLASS_API SMatrixTLSynchronizer : public ::arServices::ISynchronizer
{

public:

    fwCoreServiceMacro(SMatrixTLSynchronizer, ::arServices::ISynchronizer);

    typedef ::fwCom::Signal< void (int) > MatrixSynchronizedSignalType;
    typedef ::fwCom::Signal< void (int) > MatrixUnsynchronizedSignalType;

    /**
     * @brief Constructor.
     */
    SYNCTIMELINE_API SMatrixTLSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SMatrixTLSynchronizer() noexcept
    {
    }

    typedef std::map< unsigned long, std::string > MatrixIndexNameType;

protected:

    /// Does nothing
    SYNCTIMELINE_API void configuring() override;

    /// This method is used to initialize the service.
    SYNCTIMELINE_API void starting() override;

    /// Does nothing.
    SYNCTIMELINE_API void stopping() override;

    /// Does nothing.
    SYNCTIMELINE_API void updating() override;

    /// Synchronize
    SYNCTIMELINE_API void synchronize();

    SYNCTIMELINE_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    MatrixIndexNameType m_matrixIndexName;
};

} //namespace syncTimeline
