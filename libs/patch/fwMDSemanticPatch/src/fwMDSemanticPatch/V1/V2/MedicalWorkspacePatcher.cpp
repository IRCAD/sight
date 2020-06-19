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

#include "fwMDSemanticPatch/V1/V2/MedicalWorkspacePatcher.hpp"

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/IPatch.hpp>
#include <fwAtomsPatch/IStructuralPatch.hpp>
#include <fwAtomsPatch/StructuralPatchDB.hpp>
#include <fwAtomsPatch/patcher/registry/macros.hpp>
#include <fwAtomsPatch/helper/functions.hpp>

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{

patcherRegisterMacro(::fwMDSemanticPatch::V1::V2::MedicalWorkspacePatcher, "MedicalWorkspacePatcher");

MedicalWorkspacePatcher::MedicalWorkspacePatcher(::fwAtomsPatch::patcher::IPatcher::Key key) :
    ::fwAtomsPatch::patcher::DefaultPatcher(key)
{
}

// ----------------------------------------------------------------------------

MedicalWorkspacePatcher::~MedicalWorkspacePatcher()
{
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr MedicalWorkspacePatcher::transformObject(::fwAtoms::Object::sptr object,
                                                                 const std::string &context,
                                                                 const std::string &currentVersion,
                                                                 const std::string &targetVersion)
{
    this->addCompositeTypes(object);
    return ::fwAtomsPatch::patcher::DefaultPatcher::transformObject(object,context,currentVersion, targetVersion);
}

// ----------------------------------------------------------------------------

void MedicalWorkspacePatcher::addCompositeTypes(::fwAtoms::Object::sptr object)
{
    if( ::fwAtomsPatch::helper::getClassname( object ) == "::fwData::Composite")
    {
        ::fwAtoms::Map::sptr values = object->getAttribute< ::fwAtoms::Map >("values");
        if((*values)["patientDB"] && (*values)["planningDB"] && (*values)["processingDB"])
        {
            object->setMetaInfo("compositeType", "MedicalWorkspace");
        }
    }

    for( ::fwAtoms::Object::AttributesType::value_type elem :  object->getAttributes() )
    {
        ::fwAtoms::Object::sptr att = ::fwAtoms::Object::dynamicCast(elem.second);
        if(att)
        {
            this->addCompositeTypes(att);
        }
    }
}

} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch

