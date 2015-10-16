/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


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


