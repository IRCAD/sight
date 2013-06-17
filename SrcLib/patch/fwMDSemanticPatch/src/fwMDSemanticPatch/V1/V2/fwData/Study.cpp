/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

#include "fwMDSemanticPatch/V1/V2/fwData/Study.hpp"

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace fwData
{

Study::Study() : ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Study";
    m_originVersion = "1";
    this->addContext("MedicalData", "V1", "V2");
}

// ----------------------------------------------------------------------------

Study::~Study()
{}

// ----------------------------------------------------------------------------

Study::Study( const Study &cpy ) : ::fwAtomsPatch::ISemanticPatch(cpy)
{}

// ----------------------------------------------------------------------------

void Study::apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );
    ::fwAtomsPatch::helper::Object helper( current );

    ::fwAtoms::String::sptr uid = current->getAttribute< ::fwAtoms::String >("instance_uid");
    if( uid->getValue().empty() )
    {
        helper.replaceAttribute("instance_uid", ::fwAtoms::String::New(::fwTools::UUID::generateUUID()));
    }
}

// ----------------------------------------------------------------------------

} // namespace fwData
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch

