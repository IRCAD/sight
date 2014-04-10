/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/UUID.hpp>

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>

#include <fwAtomsPatch/helper/functions.hpp>

#include "fwMDSemanticPatch/V2/V03AGO/fwData/Image.hpp"

namespace fwMDSemanticPatch
{
namespace V2
{
namespace V03AGO
{
namespace fwData
{

Image::Image() : ::fwAtomsPatch::ISemanticPatch()
{
    m_originClassname = "::fwData::Image";
    m_originVersion = "1";
    this->addContext("MedicalData", "V2", "V03AGO");
}

// ----------------------------------------------------------------------------

Image::~Image()
{}

// ----------------------------------------------------------------------------

Image::Image( const Image &cpy ) : ::fwAtomsPatch::ISemanticPatch(cpy)
{}

// ----------------------------------------------------------------------------

void Image::apply(
        const ::fwAtoms::Object::sptr& previous,
        const ::fwAtoms::Object::sptr& current,
        ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    ISemanticPatch::apply(previous, current, newVersions);
    ::fwAtomsPatch::helper::cleanFields( current );
    ::fwAtomsPatch::helper::Object helper( current );

    ::fwAtoms::Object::sptr array = ::fwAtoms::Object::dynamicCast(previous->getAttribute("array"));
    ::fwAtoms::Numeric::sptr nbComponent = ::fwAtoms::Numeric::dynamicCast(array->getAttribute("nb_of_components"));

    helper.replaceAttribute("nb_components", nbComponent->clone());
}

// ----------------------------------------------------------------------------

} // namespace fwData
} // namespace V03AGO
} // namespace V2
} // namespace fwMDSemanticPatch

