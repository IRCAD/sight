/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arStructuralPatch/arData/Camera/V1ToV2.hpp"

#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>


namespace arStructuralPatch
{

namespace arData
{

namespace Camera
{

V1ToV2::V1ToV2() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::arData::Camera";
    m_targetClassname = "::arData::Camera";
    m_originVersion   = "1";
    m_targetVersion   = "2";

}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2( const V1ToV2 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply(
    const ::fwAtoms::Object::sptr& previous,
    const ::fwAtoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    ::fwAtoms::Sequence::sptr intrinsicParam = ::fwAtoms::Sequence::dynamicCast(previous->getAttribute("intrinsic"));
    ::fwAtoms::Numeric::sptr width           = ::fwAtoms::Numeric::dynamicCast((*intrinsicParam)[4]);
    ::fwAtoms::Numeric::sptr height          = ::fwAtoms::Numeric::dynamicCast((*intrinsicParam)[5]);

    ::fwAtoms::String::sptr camID       = ::fwAtoms::String::dynamicCast(previous->getAttribute("camera_id"));
    ::fwAtoms::String::sptr description = ::fwAtoms::String::dynamicCast(previous->getAttribute("description"));

    std::string camSource = "UNKNOWN";
    std::string videoFile;
    std::string streamUrl;
    if (camID->getString() == "file")
    {
        camSource = "FILE";
        videoFile = description->getString();
    }
    else if (camID->getString() == "stream")
    {
        camSource = "STREAM";
        streamUrl = description->getString();
    }
    else if (!camID->getString().empty())
    {
        camSource = "DEVICE";
    }

    ::fwAtoms::Sequence::sptr newIntrinsic = ::fwAtoms::Sequence::New();

    for (size_t i = 0; i<4; ++i)
    {
        newIntrinsic->push_back((*intrinsicParam)[i]->clone());
    }

    helper.replaceAttribute("intrinsic", newIntrinsic);
    helper.addAttribute("width", width->clone());
    helper.addAttribute("height", height->clone());
    helper.addAttribute("max_framerate", ::fwAtoms::Numeric::New(30.0f));
    helper.addAttribute("camera_source", ::fwAtoms::String::New(camSource));
    helper.addAttribute("stream_url", ::fwAtoms::String::New(streamUrl));
    helper.addAttribute("video_file", ::fwAtoms::String::New(videoFile));
    helper.addAttribute("pixel_format", ::fwAtoms::String::New("RGBA32"));
}

} // namespace Camera

} // namespace rdData

} // namespace arStructuralPatch

