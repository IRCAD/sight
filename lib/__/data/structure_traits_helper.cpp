/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/structure_traits_helper.hpp"

#include <boost/assign.hpp>

namespace sight::data
{

//------------------------------------------------------------------------------

const structure_traits_helper::category_translator_t structure_traits_helper::CATEGORYTRANSLATOR =
    boost::assign::list_of<structure_traits_helper::category_translator_t::relation>
        (data::structure_traits::body, std::string("Body"))
        (data::structure_traits::head, std::string("Head"))
        (data::structure_traits::neck, std::string("Neck"))
        (data::structure_traits::thorax, std::string("Thorax"))
        (data::structure_traits::abdomen, std::string("Abdomen"))
        (data::structure_traits::pelvis, std::string("Pelvis"))
        (data::structure_traits::arm, std::string("Arm"))
        (data::structure_traits::leg, std::string("Leg"))
        (data::structure_traits::liver_segments, std::string("Liver_segments"))
        (data::structure_traits::other, std::string("Other"));

//------------------------------------------------------------------------------

const structure_traits_helper::class_translator_t structure_traits_helper::CLASSTRANSLATOR =
    boost::assign::list_of<structure_traits_helper::class_translator_t::relation>
        (data::structure_traits::tool, std::string("Tool"))
        (data::structure_traits::environment, std::string("Environment"))
        (data::structure_traits::vessel, std::string("Vessel"))
        (data::structure_traits::lesion, std::string("Lesion"))
        (data::structure_traits::organ, std::string("Organ"))
        (data::structure_traits::functional, std::string("Functional"))
        (data::structure_traits::no_constraint, std::string("No_constraint"));

//------------------------------------------------------------------------------

} // namespace sight::data
