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

const structure_traits_helper::CategoryTranslatorType structure_traits_helper::s_CATEGORYTRANSLATOR =
    boost::assign::list_of<structure_traits_helper::CategoryTranslatorType::relation>
        (data::structure_traits::BODY, std::string("Body"))
        (data::structure_traits::HEAD, std::string("Head"))
        (data::structure_traits::NECK, std::string("Neck"))
        (data::structure_traits::THORAX, std::string("Thorax"))
        (data::structure_traits::ABDOMEN, std::string("Abdomen"))
        (data::structure_traits::PELVIS, std::string("Pelvis"))
        (data::structure_traits::ARM, std::string("Arm"))
        (data::structure_traits::LEG, std::string("Leg"))
        (data::structure_traits::LIVER_SEGMENTS, std::string("Liver_segments"))
        (data::structure_traits::OTHER, std::string("Other"));

//------------------------------------------------------------------------------

const structure_traits_helper::ClassTranslatorType structure_traits_helper::s_CLASSTRANSLATOR =
    boost::assign::list_of<structure_traits_helper::ClassTranslatorType::relation>
        (data::structure_traits::TOOL, std::string("Tool"))
        (data::structure_traits::ENVIRONMENT, std::string("Environment"))
        (data::structure_traits::VESSEL, std::string("Vessel"))
        (data::structure_traits::LESION, std::string("Lesion"))
        (data::structure_traits::ORGAN, std::string("Organ"))
        (data::structure_traits::FUNCTIONAL, std::string("Functional"))
        (data::structure_traits::NO_CONSTRAINT, std::string("No_constraint"));

//------------------------------------------------------------------------------

} // namespace sight::data
