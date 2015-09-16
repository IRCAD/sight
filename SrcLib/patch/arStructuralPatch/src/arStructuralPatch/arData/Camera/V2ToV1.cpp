/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "arStructuralPatch/arData/Camera/V2ToV1.hpp"

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

V2ToV1::V2ToV1() : ::fwAtomsPatch::IStructuralPatch()
{
    m_originClassname = "::arData::Camera";
    m_targetClassname = "::arData::Camera";
    m_originVersion   = "2";
    m_targetVersion   = "1";

}

// ----------------------------------------------------------------------------

V2ToV1::~V2ToV1()
{
}

// ----------------------------------------------------------------------------

V2ToV1::V2ToV1( const V2ToV1 &cpy ) : ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV1::apply(
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
    ::fwAtoms::Numeric::sptr width           = ::fwAtoms::Numeric::dynamicCast(previous->getAttribute("width"));
    ::fwAtoms::Numeric::sptr height          = ::fwAtoms::Numeric::dynamicCast(previous->getAttribute("height"));
    ::fwAtoms::String::sptr camSource        = ::fwAtoms::String::dynamicCast(previous->getAttribute("camera_source"));
    ::fwAtoms::String::sptr videoFile        = ::fwAtoms::String::dynamicCast(previous->getAttribute("video_file"));
    ::fwAtoms::String::sptr streamUrl        = ::fwAtoms::String::dynamicCast(previous->getAttribute("stream_url"));
    ::fwAtoms::String::sptr cameraID         = ::fwAtoms::String::dynamicCast(previous->getAttribute("camera_id"));
    ::fwAtoms::String::sptr cameraDesc       = ::fwAtoms::String::dynamicCast(previous->getAttribute("description"));


    std::string camID;
    std::string description;
    if (camSource->getString() == "FILE")
    {
        camID       = "file";
        description = videoFile->getString();
    }
    else if (camSource->getString() == "STREAM")
    {
        camID       = "stream";
        description = streamUrl->getString();
    }
    else if (camSource->getString() == "DEVICE")
    {
        camID       = cameraID->getString();
        description = cameraDesc->getString();
    }

    ::fwAtoms::Sequence::sptr newIntrinsic = ::fwAtoms::Sequence::dynamicCast(intrinsicParam->clone());

    newIntrinsic->push_back(width->clone());
    newIntrinsic->push_back(height->clone());

    helper.replaceAttribute("camera_id", ::fwAtoms::String::New(camID));
    helper.replaceAttribute("description", ::fwAtoms::String::New(description));
    helper.replaceAttribute("intrinsic", newIntrinsic);

    helper.removeAttribute("max_framerate");
    helper.removeAttribute("camera_source");
    helper.removeAttribute("stream_url");
    helper.removeAttribute("video_file");
    helper.removeAttribute("pixel_format");
}

} // namespace Camera

} // namespace arData

} // namespace arStructuralPatch

