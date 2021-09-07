/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/patch/semantic/V1/V2/MedicalWorkspacePatcher.hpp"

#include <atoms/Map.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>

#include <io/atoms/patch/helper/functions.hpp>
#include <io/atoms/patch/IPatch.hpp>
#include <io/atoms/patch/IStructuralPatch.hpp>
#include <io/atoms/patch/patcher/registry/macros.hpp>
#include <io/atoms/patch/StructuralPatchDB.hpp>

namespace sight::io::patch::semantic
{

namespace V1
{

namespace V2
{

patcherRegisterMacro(io::patch::semantic::V1::V2::MedicalWorkspacePatcher, "MedicalWorkspacePatcher");

MedicalWorkspacePatcher::MedicalWorkspacePatcher(io::atoms::patch::patcher::IPatcher::Key key) :
    io::atoms::patch::patcher::DefaultPatcher(key)
{
}

// ----------------------------------------------------------------------------

MedicalWorkspacePatcher::~MedicalWorkspacePatcher()
{
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr MedicalWorkspacePatcher::transformObject(
    sight::atoms::Object::sptr object,
    const std::string& context,
    const std::string& currentVersion,
    const std::string& targetVersion
)
{
    this->addCompositeTypes(object);
    return io::atoms::patch::patcher::DefaultPatcher::transformObject(object, context, currentVersion, targetVersion);
}

// ----------------------------------------------------------------------------

void MedicalWorkspacePatcher::addCompositeTypes(sight::atoms::Object::sptr object)
{
    if(io::atoms::patch::helper::getClassname(object) == "sight::data::Composite")
    {
        sight::atoms::Map::sptr values = object->getAttribute<sight::atoms::Map>("values");
        if((*values)["patientDB"] && (*values)["planningDB"] && (*values)["processingDB"])
        {
            object->setMetaInfo("compositeType", "MedicalWorkspace");
        }
    }

    for(sight::atoms::Object::AttributesType::value_type elem : object->getAttributes())
    {
        sight::atoms::Object::sptr att = sight::atoms::Object::dynamicCast(elem.second);
        if(att)
        {
            this->addCompositeTypes(att);
        }
    }
}

} // namespace V2

} // namespace V1

} // namespace sight::io::patch::semantic
