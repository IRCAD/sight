/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "data/GenericTL.hpp"
#include "data/GenericTL.hxx"
#include "data/timeline/GenericObject.hpp"
#include "data/timeline/GenericObject.hxx"

namespace sight::data
{

/**
 * @brief   This class defines a timeline that stores groups of matrices.
 */
class DATA_CLASS_API MatrixTL : public GenericTL< float[16] >
{
public:
    fwCoreClassMacro(MatrixTL, data::TimeLine, data::factory::New< MatrixTL >)

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    MatrixTL( data::Object::Key key ) :
        GenericTL< float[16] >(key)
    {
    }
};

} // namespace sight::data
