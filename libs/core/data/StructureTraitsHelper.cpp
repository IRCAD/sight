/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "data/StructureTraitsHelper.hpp"

#include <boost/assign.hpp>

namespace sight::data
{

//------------------------------------------------------------------------------

const StructureTraitsHelper::CategoryTranslatorType StructureTraitsHelper::s_CATEGORYTRANSLATOR =
    ::boost::assign::list_of<StructureTraitsHelper::CategoryTranslatorType::relation>
        (data::StructureTraits::BODY, std::string("Body"))
        (data::StructureTraits::HEAD, std::string("Head"))
        (data::StructureTraits::NECK, std::string("Neck"))
        (data::StructureTraits::THORAX, std::string("Thorax"))
        (data::StructureTraits::ABDOMEN, std::string("Abdomen"))
        (data::StructureTraits::PELVIS, std::string("Pelvis"))
        (data::StructureTraits::ARM, std::string("Arm"))
        (data::StructureTraits::LEG, std::string("Leg"))
        (data::StructureTraits::LIVER_SEGMENTS, std::string("Liver_segments"))
        (data::StructureTraits::OTHER, std::string("Other"));

//------------------------------------------------------------------------------

const StructureTraitsHelper::ClassTranslatorType StructureTraitsHelper::s_CLASSTRANSLATOR =
    ::boost::assign::list_of<StructureTraitsHelper::ClassTranslatorType::relation>
        (data::StructureTraits::TOOL, std::string("Tool"))
        (data::StructureTraits::ENVIRONMENT, std::string("Environment"))
        (data::StructureTraits::VESSEL, std::string("Vessel"))
        (data::StructureTraits::LESION, std::string("Lesion"))
        (data::StructureTraits::ORGAN, std::string("Organ"))
        (data::StructureTraits::FUNCTIONAL, std::string("Functional"))
        (data::StructureTraits::NO_CONSTRAINT, std::string("No_constraint"));

//------------------------------------------------------------------------------

} // namespace sight::data
