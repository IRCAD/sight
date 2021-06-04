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

#include "io/patch/structural/data/Camera/V2ToV1.hpp"

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

V2ToV1::V2ToV1() :
    io::atoms::patch::IStructuralPatch()
{
    m_originClassname = "data::Camera";
    m_targetClassname = "data::Camera";
    m_originVersion   = "2";
    m_targetVersion   = "1";
}

// ----------------------------------------------------------------------------

V2ToV1::~V2ToV1()
{
}

// ----------------------------------------------------------------------------

V2ToV1::V2ToV1(const V2ToV1& cpy) :
    io::atoms::patch::IStructuralPatch(cpy)
{
}

// ----------------------------------------------------------------------------

void V2ToV1::apply(
    const sight::atoms::Object::sptr& previous,
    const sight::atoms::Object::sptr& current,
    io::atoms::patch::IPatch::NewVersionsType& newVersions
)
{
    IStructuralPatch::apply(previous, current, newVersions);

    // Update object version
    this->updateVersion(current);

    // Create helper
    io::atoms::patch::helper::Object helper(current);

    sight::atoms::Sequence::sptr intrinsicParam = sight::atoms::Sequence::dynamicCast(
        previous->getAttribute(
            "intrinsic"
        )
    );
    sight::atoms::Numeric::sptr width    = sight::atoms::Numeric::dynamicCast(previous->getAttribute("width"));
    sight::atoms::Numeric::sptr height   = sight::atoms::Numeric::dynamicCast(previous->getAttribute("height"));
    sight::atoms::String::sptr camSource =
        sight::atoms::String::dynamicCast(previous->getAttribute("camera_source"));
    sight::atoms::String::sptr videoFile =
        sight::atoms::String::dynamicCast(previous->getAttribute("video_file"));
    sight::atoms::String::sptr streamUrl =
        sight::atoms::String::dynamicCast(previous->getAttribute("stream_url"));
    sight::atoms::String::sptr cameraID =
        sight::atoms::String::dynamicCast(previous->getAttribute("camera_id"));
    sight::atoms::String::sptr cameraDesc = sight::atoms::String::dynamicCast(
        previous->getAttribute(
            "description"
        )
    );

    std::string camID;
    std::string description;
    if(camSource->getString() == "FILE")
    {
        camID       = "file";
        description = videoFile->getString();
    }
    else if(camSource->getString() == "STREAM")
    {
        camID       = "stream";
        description = streamUrl->getString();
    }
    else if(camSource->getString() == "DEVICE")
    {
        camID       = cameraID->getString();
        description = cameraDesc->getString();
    }

    sight::atoms::Sequence::sptr newIntrinsic = sight::atoms::Sequence::dynamicCast(intrinsicParam->clone());

    newIntrinsic->push_back(width->clone());
    newIntrinsic->push_back(height->clone());

    helper.replaceAttribute("camera_id", sight::atoms::String::New(camID));
    helper.replaceAttribute("description", sight::atoms::String::New(description));
    helper.replaceAttribute("intrinsic", newIntrinsic);

    helper.removeAttribute("max_framerate");
    helper.removeAttribute("camera_source");
    helper.removeAttribute("stream_url");
    helper.removeAttribute("video_file");
    helper.removeAttribute("pixel_format");
}

} // namespace Camera

} // namespace data

} // namespace sight::io::patch::structural
