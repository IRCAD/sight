/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Map.hpp>

#include <fwAtomsPatch/IPatch.hpp>
#include <fwAtomsPatch/IStructuralPatch.hpp>
#include <fwAtomsPatch/StructuralPatchDB.hpp>
#include <fwAtomsPatch/patcher/registry/macros.hpp>
#include <fwAtomsPatch/helper/functions.hpp>

#include "fwMDSemanticPatch/V1/V2/MedicalWorkspacePatcher.hpp"

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

    BOOST_FOREACH( ::fwAtoms::Object::AttributesType::value_type elem, object->getAttributes() )
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

