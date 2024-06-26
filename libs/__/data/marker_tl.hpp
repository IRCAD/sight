/************************************************************************
 *
 * Copyright (C) 2014-2024 IRCAD France
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

#include <sight/data/config.hpp>

#include "data/generic_tl.hpp"
#include "data/generic_tl.hxx"
#include "data/timeline/generic_object.hpp"
#include "data/timeline/generic_object.hxx"

#include <array>

namespace sight::data
{

/**
 * @brief   This class defines a timeline that stores groups of markers.
 */
class SIGHT_DATA_CLASS_API marker_tl : public data::generic_tl<std::array<float,
                                                                          8> >
{
public:

    SIGHT_DECLARE_CLASS(marker_tl, data::timeline::base);

    /**
     * @brief Constructor
     */
    SIGHT_DATA_API marker_tl() :
        data::generic_tl<std::array<float, 8> >()
    {
    }
};

} // namespace sight::data
