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

#include "io/patch/structural/data/Camera/V1ToV2.hpp"

#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

namespace sight::io::patch::structural
{

namespace data
{

namespace Camera
{

V1ToV2::V1ToV2() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "data::Camera";
    m_targetClassname = "data::Camera";
    m_originVersion   = "1";
    m_targetVersion   = "2";

}

// ----------------------------------------------------------------------------

V1ToV2::~V1ToV2()
{
}

// ----------------------------------------------------------------------------

V1ToV2::V1ToV2( const V1ToV2& cpy ) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V1ToV2::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    io::atoms::patch::helper::Object helper(current);

    sight::atoms::Sequence::sptr intrinsicParam = sight::atoms::Sequence::dynamicCast(previous->getAttribute(
                                                                                          "intrinsic"));
    sight::atoms::Numeric::sptr width  = sight::atoms::Numeric::dynamicCast((*intrinsicParam)[4]);
    sight::atoms::Numeric::sptr height = sight::atoms::Numeric::dynamicCast((*intrinsicParam)[5]);

    sight::atoms::String::sptr camID       = sight::atoms::String::dynamicCast(previous->getAttribute("camera_id"));
    sight::atoms::String::sptr description = sight::atoms::String::dynamicCast(previous->getAttribute("description"));

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

    sight::atoms::Sequence::sptr newIntrinsic = sight::atoms::Sequence::New();

    for (size_t i = 0; i < 4; ++i)
    {
        newIntrinsic->push_back((*intrinsicParam)[i]->clone());
    }

    helper.replaceAttribute("intrinsic", newIntrinsic);
    helper.addAttribute("width", width->clone());
    helper.addAttribute("height", height->clone());
    helper.addAttribute("max_framerate", sight::atoms::Numeric::New(30.0f));
    helper.addAttribute("camera_source", sight::atoms::String::New(camSource));
    helper.addAttribute("stream_url", sight::atoms::String::New(streamUrl));
    helper.addAttribute("video_file", sight::atoms::String::New(videoFile));
    helper.addAttribute("pixel_format", sight::atoms::String::New("RGBA32"));
}

} // namespace Camera

} // namespace rdData

} // namespace sight::io::patch::structural
