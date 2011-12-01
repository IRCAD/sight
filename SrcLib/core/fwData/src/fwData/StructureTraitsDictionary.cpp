/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>

#include <fwTools/Factory.hpp>

#include "fwData/StructureTraitsDictionary.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::StructureTraitsDictionary, ::fwData::StructureTraitsDictionary );

namespace fwData
{
//------------------------------------------------------------------------------

StructureTraitsDictionary::StructureTraitsDictionary ()
{
}


//------------------------------------------------------------------------------

StructureTraitsDictionary::~StructureTraitsDictionary ()
{
}

//------------------------------------------------------------------------------

StructureTraits::sptr StructureTraitsDictionary::getStructure(std::string type)
{
    OSLM_ASSERT("Structure of type '" << type << "' not found", m_structureTraitsMap.find(type) != m_structureTraitsMap.end());
    return m_structureTraitsMap[type];
}

//------------------------------------------------------------------------------

void StructureTraitsDictionary::addStructure(StructureTraits::sptr structureTraits)
{
    m_structureTraitsMap[structureTraits->getType()] = structureTraits;
}

//------------------------------------------------------------------------------


} // namespace fwData


