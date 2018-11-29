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

#include "echoEdSimu/config.hpp"

#include <arServices/ISimulator.hpp>

namespace echoEdSimu
{

/**
 * @brief   Generates a mesh that can be used to display an echographic image.
 * @deprecated This service will be removed in 19.0
 */
class ECHOEDSIMU_CLASS_API SProbeMesh : public ::arServices::ISimulator
{

public:

    fwCoreServiceClassDefinitionsMacro( (SProbeMesh)(::arServices::ISimulator) );

    /// Constructor. Do nothing.
    ECHOEDSIMU_API SProbeMesh() noexcept;

    /// Destructor. Do nothing.
    ECHOEDSIMU_API virtual ~SProbeMesh() noexcept;

protected:

    /// Do nothing
    ECHOEDSIMU_API virtual void starting() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping() override;

    /// Generate the mesh
    ECHOEDSIMU_API virtual void updating() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void swapping() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void configuring() override;

};

} // namespace echoEdSimu
