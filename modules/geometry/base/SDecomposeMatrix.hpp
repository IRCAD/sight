/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "modules/geometry/base/config.hpp"

#include <service/IOperator.hpp>

namespace sight::modules::geometry::base
{

/**
 * @brief   This service decomposes a matrix into a 3 by 3 rotation matrix, a 3 dimensional translation vector
 * and a 3 by 3 scaling matrix. The skew and the perspective are also decomposed. These information are not in
 * data::Matrix4 data for the moment but it is possible to do it later.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::sight::modules::geometry::base::SDecomposeMatrix">
            <in key="source" uid="..." autoConnect="yes" />
            <inout key="translation" uid="..." />
            <inout key="rotation" uid="..." />
            <inout key="scale" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [sight::data::Matrix4]: matrix to decompose.
 * @subsection In-Out In-Out:
 * - \b translation [sight::data::Matrix4] (optional): translation matrix.
 * - \b rotation [sight::data::Matrix4] (optional): rotation matrix.
 * - \b scale [sight::data::Matrix4] (optional): scaling matrix.
 *
 */
class MODULE_GEOMETRY_BASE_CLASS_API SDecomposeMatrix : public service::IOperator
{

public:

    fwCoreServiceMacro(SDecomposeMatrix, ::sight::service::IOperator)

    /**
     * @brief Constructor.
     */
    MODULE_GEOMETRY_BASE_API SDecomposeMatrix() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SDecomposeMatrix() noexcept
    {
    }

protected:

    /// This method is used to configure the service.
    MODULE_GEOMETRY_BASE_API void configuring() override;

    /// This method is used to initialize the service.
    MODULE_GEOMETRY_BASE_API void starting() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void stopping() override;

    /// Does nothing.
    MODULE_GEOMETRY_BASE_API void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Object::s_MODIFIED_SIG to s_UPDATE_SLOT
     */
    MODULE_GEOMETRY_BASE_API virtual KeyConnectionsMap getAutoConnections() const override;
};

} //namespace sight::modules::geometry::base
