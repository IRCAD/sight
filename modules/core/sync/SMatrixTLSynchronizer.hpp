/************************************************************************
 *
 * Copyright (C) 2014-2022 IRCAD France
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

#include "modules/sync/config.hpp"

#include <data/Matrix4.hpp>
#include <data/MatrixTL.hpp>

#include <service/ISynchronizer.hpp>

namespace sight::module::sync
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
     <service uid="matrixToolsSynchronizer" type="sight::module::sync::SMatrixTLSynchronizer">
         <in key="matrixTL" uid="matrixToolsTL" autoConnect="true" />
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
 * - \b matrixTL [sight::data::MatrixTL]: matrix timeline used to extract matrices.
 * @subsection In-Out In-Out
 * - \b matrices [sight::data::Matrix4]: list of Matrix4 used to store extracted matrices.
 */
class MODULE_SYNC_CLASS_API SMatrixTLSynchronizer : public service::ISynchronizer
{
public:

    SIGHT_DECLARE_SERVICE(SMatrixTLSynchronizer, service::ISynchronizer);

    typedef core::com::Signal<void (int)> MatrixSynchronizedSignalType;
    typedef core::com::Signal<void (int)> MatrixUnsynchronizedSignalType;

    /**
     * @brief Constructor.
     */
    MODULE_SYNC_API SMatrixTLSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    ~SMatrixTLSynchronizer() noexcept override =
        default;

    typedef std::map<std::uint64_t, std::string> MatrixIndexNameType;

protected:

    /// Does nothing
    MODULE_SYNC_API void configuring() override;

    /// This method is used to initialize the service.
    MODULE_SYNC_API void starting() override;

    /// Does nothing.
    MODULE_SYNC_API void stopping() override;

    /// Does nothing.
    MODULE_SYNC_API void updating() override;

    /// Synchronize
    MODULE_SYNC_API void synchronize();

    MODULE_SYNC_API service::IService::KeyConnectionsMap getAutoConnections() const override;

private:

    MatrixIndexNameType m_matrixIndexName;

    static constexpr std::string_view s_MATRIXTL_INPUT = "matrixTL";
    static constexpr std::string_view s_MATRICES_INOUT = "matrices";

    data::ptr<data::MatrixTL, data::Access::in> m_matrixTL {this, s_MATRIXTL_INPUT, true};
    data::ptr_vector<data::Matrix4, data::Access::inout> m_matrices {this, s_MATRICES_INOUT};
};

} //namespace sight::module::sync
