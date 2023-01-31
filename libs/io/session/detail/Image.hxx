/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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
#include "io/session/macros.hpp"

#include <data/Image.hpp>

#include <io/vtk/vtk.hpp>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLImageDataWriter.h>

#define USE_VTK

namespace sight::io::session::detail::Image
{

constexpr static auto s_uuid {"uuid"};
constexpr static auto s_WindowCenters {"WindowCenters"};
constexpr static auto s_WindowWidths {"WindowWidths"};
constexpr static auto s_WindowCenter {"WindowCenter"};
constexpr static auto s_WindowWidth {"WindowWidth"};
constexpr static auto s_Type {"Type"};
constexpr static auto s_PixelFormat {"PixelFormat"};
constexpr static auto s_Spacing {"Spacing"};
constexpr static auto s_Origin {"Origin"};
constexpr static auto s_Size {"Size"};
constexpr static auto s_X {"X"};
constexpr static auto s_Y {"Y"};
constexpr static auto s_Z {"Z"};
constexpr static auto s_Width {"Width"};
constexpr static auto s_Height {"Height"};
constexpr static auto s_Depth {"Depth"};

//------------------------------------------------------------------------------

inline static std::filesystem::path get_file_path(const std::string& uuid)
{
#if defined(USE_VTK)
    constexpr auto ext = ".vti";
#else
    constexpr auto ext = ".raw";
#endif

    return std::filesystem::path(uuid + "/" + data::Image::leafClassname() + ext);
}

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>& /*unused*/,
    const core::crypto::secure_string& password = ""
)
{
    auto image = Helper::safe_cast<data::Image>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Image>(tree, 1);

    // Serialize image
    const auto& size = image->getSize();
    boost::property_tree::ptree sizeTree;
    sizeTree.add(s_Width, size[0]);
    sizeTree.add(s_Height, size[1]);
    sizeTree.add(s_Depth, size[2]);
    tree.add_child(s_Size, sizeTree);

    const auto& type = image->getType();
    tree.put(s_Type, type.name());

    const auto& format = image->getPixelFormat();
    tree.put(s_PixelFormat, format);

    // Write image metadata
    const auto& spacing = image->getSpacing();
    boost::property_tree::ptree spacingTree;
    spacingTree.add(s_X, spacing[0]);
    spacingTree.add(s_Y, spacing[1]);
    spacingTree.add(s_Z, spacing[2]);
    tree.add_child(s_Spacing, spacingTree);

    const auto& origin = image->getOrigin();
    boost::property_tree::ptree originTree;
    originTree.add(s_X, origin[0]);
    originTree.add(s_Y, origin[1]);
    originTree.add(s_Z, origin[2]);
    tree.add_child(s_Origin, originTree);

    boost::property_tree::ptree windowCentersTree;
    for(std::size_t index = 0 ; const auto& windowCenter : image->getWindowCenter())
    {
        windowCentersTree.put(s_WindowCenter + std::to_string(index++), windowCenter);
    }

    tree.add_child(s_WindowCenters, windowCentersTree);

    boost::property_tree::ptree windowWidthsTree;
    for(std::size_t index = 0 ; const auto& windowWidth : image->getWindowWidth())
    {
        windowWidthsTree.put(s_WindowWidth + std::to_string(index++), windowWidth);
    }

    tree.add_child(s_WindowWidths, windowWidthsTree);

    // Create the output file inside the archive
    const auto& ostream = archive.openFile(
        get_file_path(image->getUUID()),
        password,
        sight::io::zip::Method::DEFAULT,
        sight::io::zip::Level::BEST
    );

#if defined(USE_VTK)
    /// @todo For now, toVTKImage doesn't handle all pixel formats we handle. For example, BGR is written as RGB.
    /// It should be better to convert it, even if, here we don't really care as we save the real pixel format and
    /// restore it back. It is faster, but produce a non standard vti file.
    auto vtkImage = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(image, vtkImage);

    // Create the vtk writer
    const auto& vtkWriter = vtkSmartPointer<vtkXMLImageDataWriter>::New();
    vtkWriter->SetCompressorTypeToNone();
    vtkWriter->SetDataModeToBinary();
    vtkWriter->WriteToOutputStringOn();
    vtkWriter->SetInputData(vtkImage);

    // Write to internal string...
    vtkWriter->Update();

    // Write back to the archive
    (*ostream) << vtkWriter->GetOutputString();
#else
    // Write the image data as raw bytes
    ostream->write(reinterpret_cast<const char*>(image->getBuffer()), std::streamsize(image->getSizeInBytes()));
#endif
}

//------------------------------------------------------------------------------

inline static data::Image::sptr read(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>& /*unused*/,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto image           = Helper::cast_or_create<data::Image>(object);
    const auto dump_lock = image->dump_lock();

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Image>(tree, 0, 1);

    // Deserialize Image
    const auto& sizeTree         = tree.get_child(s_Size);
    const data::Image::Size size = {
        sizeTree.get<size_t>(s_Width),
        sizeTree.get<size_t>(s_Height),
        sizeTree.get<size_t>(s_Depth)
    };

    core::Type type(tree.get<std::string>(s_Type));

    const auto format = static_cast<data::Image::PixelFormat>(tree.get<int>(s_PixelFormat));

    ///@note This is not saved in VTK files.
    std::vector<double> windowCenters;
    for(const auto& value : tree.get_child(s_WindowCenters))
    {
        windowCenters.push_back(value.second.get_value<double>());
    }

    image->setWindowCenter(windowCenters);

    std::vector<double> windowWidths;
    for(const auto& value : tree.get_child(s_WindowWidths))
    {
        windowWidths.push_back(value.second.get_value<double>());
    }

    image->setWindowWidth(windowWidths);

    const auto& spacingTree = tree.get_child(s_Spacing);
    image->setSpacing(
        {
            spacingTree.get<double>(s_X),
            spacingTree.get<double>(s_Y),
            spacingTree.get<double>(s_Z)
        });

    const auto& originTree = tree.get_child(s_Origin);
    image->setOrigin(
        {
            originTree.get<double>(s_X),
            originTree.get<double>(s_Y),
            originTree.get<double>(s_Z)
        });

    // If pixelFormart == UNDEFINED it is ALWAYS an empty image, so early return here.
    if(format == data::Image::PixelFormat::UNDEFINED)
    {
        return image;
    }

    const auto& serialized_uuid = tree.get<std::string>(s_uuid);

    // Read the image data
    // Create the istream from the input file inside the archive
    const auto& istream = archive.openFile(
        get_file_path(serialized_uuid),
        password
    );

#if defined(USE_VTK)
    // "Convert" it to a string
    const std::string content {std::istreambuf_iterator<char>(*istream), std::istreambuf_iterator<char>()};

    // Create the vtk reader
    auto vtk_reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    vtk_reader->ReadFromInputStringOn();
    vtk_reader->SetInputString(content);
    vtk_reader->Update();

    // Convert from VTK
    io::vtk::fromVTKImage(vtk_reader->GetOutput(), image);
#endif

    /// @todo We should convert VTK RGB back to BGR if the original image is BGR and we produced a real vti files by
    /// converting BGR to RGB, which is not the case right now (see io::vtk::toVTKImage). For now, we simply switch
    /// back
    /// to the correct pixel type.
    image->resize(size, type, format);

#if !defined(USE_VTK)
    // Read the image data as raw bytes
    istream->read(reinterpret_cast<char*>(image->getBuffer()), std::streamsize(image->getSizeInBytes()));
#endif

    return image;
}

SIGHT_REGISTER_SERIALIZER(data::Image, write, read);

} // namespace sight::io::session::detail::Image
