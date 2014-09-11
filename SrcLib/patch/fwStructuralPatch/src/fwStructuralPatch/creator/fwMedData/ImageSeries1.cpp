/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwAtoms/Object.hpp>
#include <fwAtoms/Blob.hpp>

#include <fwAtomsPatch/StructuralCreatorDB.hpp>
#include <fwAtomsPatch/helper/Object.hpp>

#include "fwStructuralPatch/creator/fwMedData/ImageSeries1.hpp"

namespace fwStructuralPatch
{
namespace creator
{
namespace fwMedData
{

ImageSeries1::ImageSeries1()
{
    m_classname = "::fwMedData::ImageSeries";
    m_version = "1";
}

// ----------------------------------------------------------------------------

ImageSeries1::~ImageSeries1()
{}

// ----------------------------------------------------------------------------

ImageSeries1::ImageSeries1( const ImageSeries1 &cpy ) : Series1(cpy)
{}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr ImageSeries1::create()
{
    ::fwAtoms::Object::sptr imageSeries = this->Series1::create();
    ::fwAtomsPatch::helper::Object helper(imageSeries);

    helper.addAttribute("image", ::fwAtoms::Blob::New());

    return imageSeries;
}

} // namespace fwMedData
} // namespace creator
} // namespace fwStructuralPatch
