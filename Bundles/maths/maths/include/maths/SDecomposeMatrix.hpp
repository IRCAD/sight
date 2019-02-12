/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "maths/config.hpp"

#include <fwServices/IOperator.hpp>

namespace maths
{

/**
 * @brief   This service decomposes a matrix into a 3 by 3 rotation matrix, a 3 dimensional translation vector
 * and a 3 by 3 scaling matrix. The skew and the perspective are also decomposed. These information are not in
 * ::fwData::TransformationMatrix3D data for the moment but it is possible to do it later.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::maths::SDecomposeMatrix">
            <in key="source" uid="..." autoConnect="yes" />
            <inout key="translation" uid="..." />
            <inout key="rotation" uid="..." />
            <inout key="scale" uid="..." />
       </service>
   @endcode
 * @subsection Input Input:
 * - \b matrix [::fwData::TransformationMatrix3D]: matrix to decompose.
 * @subsection In-Out In-Out:
 * - \b translation [::fwData::TransformationMatrix3D] (optionnal): translation matrix.
 * - \b rotation [::fwData::TransformationMatrix3D] (optionnal): rotation matrix.
 * - \b scale [::fwData::TransformationMatrix3D] (optionnal): scaling matrix.
 *
 */
class MATHS_CLASS_API SDecomposeMatrix : public ::fwServices::IOperator
{

public:

    fwCoreServiceClassDefinitionsMacro((SDecomposeMatrix)(::fwServices::IOperator))

    /**
     * @brief Constructor.
     */
    MATHS_API SDecomposeMatrix() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SDecomposeMatrix() noexcept
    {
    }

protected:

    /// This method is used to configure the service.
    MATHS_API void configuring() override;

    /// This method is used to initialize the service.
    MATHS_API void starting() override;

    /// Does nothing.
    MATHS_API void stopping() override;

    /// Does nothing.
    MATHS_API void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Object::s_MODIFIED_SIG to s_UPDATE_SLOT
     */
    MATHS_API virtual KeyConnectionsMap getAutoConnections() const override;
};

} //namespace maths
