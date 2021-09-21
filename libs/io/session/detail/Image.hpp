/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#pragma once

#include "io/session/config.hpp"
#include "io/session/Helper.hpp"

#include <data/Image.hpp>

#include <io/vtk/vtk.hpp>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLImageDataWriter.h>

namespace sight::io::session
{

namespace detail::Image
{

constexpr static auto s_uuid {"uuid"};
constexpr static auto s_image {"/image.vti"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& password = ""
)
{
    const auto image = Helper::safeCast<data::Image>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Image>(tree, 1);

    // Convert the image to VTK
    const auto& vtkImage = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(image, vtkImage);

    // Create the vtk writer
    const auto& vtkWriter = vtkSmartPointer<vtkXMLImageDataWriter>::New();
    vtkWriter->SetCompressorTypeToNone();
    vtkWriter->SetDataModeToBinary();
    vtkWriter->WriteToOutputStringOn();
    vtkWriter->SetInputData(vtkImage);

    // Write to internal string...
    vtkWriter->Update();

    // Create the output file inside the archive
    const auto& ostream = archive.openFile(
        std::filesystem::path(image->getUUID() + s_image),
        password,
        zip::Method::ZSTD,
        zip::Level::BEST
    );

    // Write back to the archive
    (*ostream) << vtkWriter->GetOutputString();
}

//------------------------------------------------------------------------------

inline static data::Image::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto image = Helper::safeCast<data::Image>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Image>(tree, 0, 1);

    // Create the istream from the input file inside the archive
    const auto& uuid    = tree.get<std::string>(s_uuid);
    const auto& istream = archive.openFile(
        std::filesystem::path(uuid + s_image),
        password
    );

    // "Convert" it to a string
    const std::string content {std::istreambuf_iterator<char>(*istream), std::istreambuf_iterator<char>()};

    // Create the vtk reader
    const auto& vtkReader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    vtkReader->ReadFromInputStringOn();
    vtkReader->SetInputString(content);
    vtkReader->Update();

    // Convert from VTK
    io::vtk::fromVTKImage(vtkReader->GetOutput(), image);

    return image;
}

} // namespace detail::Image

} // namespace sight::io
