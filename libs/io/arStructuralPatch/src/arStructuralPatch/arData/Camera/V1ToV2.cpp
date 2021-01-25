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

#include "arStructuralPatch/arData/Camera/V1ToV2.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

namespace arStructuralPatch
{

namespace arData
{

namespace Camera
{

V1ToV2::V1ToV2() :
    ::fwAtomsPatch::IStructuralPatch()
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

V1ToV2::V1ToV2( const V1ToV2& cpy ) :
    ::fwAtomsPatch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply(
    const atoms::Object::sptr& previous,
    const atoms::Object::sptr& current,
    ::fwAtomsPatch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    ::fwAtomsPatch::helper::Object helper(current);

    atoms::Sequence::sptr intrinsicParam = atoms::Sequence::dynamicCast(previous->getAttribute("intrinsic"));
    atoms::Numeric::sptr width           = atoms::Numeric::dynamicCast((*intrinsicParam)[4]);
    atoms::Numeric::sptr height          = atoms::Numeric::dynamicCast((*intrinsicParam)[5]);

    atoms::String::sptr camID       = atoms::String::dynamicCast(previous->getAttribute("camera_id"));
    atoms::String::sptr description = atoms::String::dynamicCast(previous->getAttribute("description"));

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

    atoms::Sequence::sptr newIntrinsic = atoms::Sequence::New();

    for (size_t i = 0; i < 4; ++i)
    {
        newIntrinsic->push_back((*intrinsicParam)[i]->clone());
    }

    helper.replaceAttribute("intrinsic", newIntrinsic);
    helper.addAttribute("width", width->clone());
    helper.addAttribute("height", height->clone());
    helper.addAttribute("max_framerate", atoms::Numeric::New(30.0f));
    helper.addAttribute("camera_source", atoms::String::New(camSource));
    helper.addAttribute("stream_url", atoms::String::New(streamUrl));
    helper.addAttribute("video_file", atoms::String::New(videoFile));
    helper.addAttribute("pixel_format", atoms::String::New("RGBA32"));
}

} // namespace Camera

} // namespace rdData

} // namespace arStructuralPatch
