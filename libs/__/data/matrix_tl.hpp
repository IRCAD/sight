/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "data/generic_tl.hpp"
#include "data/generic_tl.hxx"
#include "data/timeline/generic_object.hpp"
#include "data/timeline/generic_object.hxx"

#include <array>

namespace sight::data
{

/**
 * @brief   This class defines a timeline that stores groups of matrices.
 */
class SIGHT_DATA_CLASS_API matrix_tl final : public generic_tl<std::array<float,
                                                                          16> >
{
public:

    SIGHT_DECLARE_CLASS(matrix_tl, timeline::base);

    /**
     * @brief Constructor
     */
    matrix_tl() :
        generic_tl<std::array<float, 16> >()
    {
    }
};

} // namespace sight::data
