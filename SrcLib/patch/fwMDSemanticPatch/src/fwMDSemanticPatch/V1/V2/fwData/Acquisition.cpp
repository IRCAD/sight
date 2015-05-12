/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/String.hpp>

#include <fwAtomsPatch/helper/functions.hpp>

#include "fwMDSemanticPatch/V1/V2/fwData/Acquisition.hpp"

namespace fwMDSemanticPatch
{
namespace V1
{
namespace V2
{
namespace fwData
{

Acquisition::Acquisition() : ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Acquisition";
    m_originVersion   = "1";
    this->addContext("MedicalData", "V1", "V2");
}

// ----------------------------------------------------------------------------

Acquisition::~Acquisition()
{
}

// ----------------------------------------------------------------------------

Acquisition::Acquisition( const Acquisition &cpy ) : ::fwAtomsPatch::ISemanticPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void Acquisition::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );

    ::fwAtoms::Object::sptr image = current->getAttribute< ::fwAtoms::Object >("image");
    ::fwAtoms::Map::sptr fields   = image->getAttribute< ::fwAtoms::Map >("fields");

    ::fwAtoms::Map::ConstIteratorType it = fields->find("m_commentId");
    ::fwAtomsPatch::helper::Object helper(current);
    if ( it != fields->end() )
    {
        ::fwAtoms::Object::sptr value = ::fwAtoms::Object::dynamicCast( it->second );
        helper.replaceAttribute("description", ::fwAtoms::String::New(value->getAttribute("value")->getString()));
    }

    ::fwAtoms::String::sptr uid = current->getAttribute< ::fwAtoms::String >("instance_uid");
    if( uid->getValue().empty() )
    {
        helper.replaceAttribute("instance_uid", ::fwAtoms::String::New( ::fwTools::UUID::generateUUID() ));
    }

}

// ----------------------------------------------------------------------------

} // namespace fwData
} // namespace V2
} // namespace V1
} // namespace fwMDSemanticPatch
