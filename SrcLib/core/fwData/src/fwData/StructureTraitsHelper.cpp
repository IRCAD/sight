/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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


#include "fwData/StructureTraitsHelper.hpp"

#include <boost/assign.hpp>

namespace fwData
{

//------------------------------------------------------------------------------

const StructureTraitsHelper::CategoryTranslatorType StructureTraitsHelper::s_CATEGORYTRANSLATOR =
    ::boost::assign::list_of< StructureTraitsHelper::CategoryTranslatorType::relation >
        ( ::fwData::StructureTraits::BODY,               std::string("Body"))
        ( ::fwData::StructureTraits::HEAD,               std::string("Head"))
        ( ::fwData::StructureTraits::NECK,               std::string("Neck"))
        ( ::fwData::StructureTraits::THORAX,             std::string("Thorax"))
        ( ::fwData::StructureTraits::ABDOMEN,            std::string("Abdomen"))
        ( ::fwData::StructureTraits::PELVIS,             std::string("Pelvis"))
        ( ::fwData::StructureTraits::ARM,                std::string("Arm"))
        ( ::fwData::StructureTraits::LEG,                std::string("Leg"))
        ( ::fwData::StructureTraits::LIVER_SEGMENTS,     std::string("Liver_segments"))
        ( ::fwData::StructureTraits::OTHER,              std::string("Other"));

//------------------------------------------------------------------------------

const StructureTraitsHelper::ClassTranslatorType StructureTraitsHelper::s_CLASSTRANSLATOR =
    ::boost::assign::list_of< StructureTraitsHelper::ClassTranslatorType::relation >
        ( ::fwData::StructureTraits::TOOL,          std::string("Tool"))
        ( ::fwData::StructureTraits::ENVIRONMENT,   std::string("Environment"))
        ( ::fwData::StructureTraits::VESSEL,        std::string("Vessel"))
        ( ::fwData::StructureTraits::LESION,        std::string("Lesion"))
        ( ::fwData::StructureTraits::ORGAN,         std::string("Organ"))
        ( ::fwData::StructureTraits::FUNCTIONAL,    std::string("Functional"))
        ( ::fwData::StructureTraits::NO_CONSTRAINT, std::string("No_constraint"));

//------------------------------------------------------------------------------

} // namespace fwData


