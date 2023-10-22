/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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
#include "io/session/helper.hpp"
#include "io/session/macros.hpp"

#include <data/image.hpp>

#include <io/vtk/vtk.hpp>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkXMLImageDataWriter.h>

#define USE_VTK

namespace sight::io::session::detail::image
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

inline static std::filesystem::path get_file_path(const std::string& _uuid)
{
#if defined(USE_VTK)
    constexpr auto ext = ".vti";
#else
    constexpr auto ext = ".raw";
#endif

    return std::filesystem::path(_uuid + "/" + data::image::leaf_classname() + ext);
}

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& _archive,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& _password = ""
)
{
    auto image = helper::safe_cast<data::image>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::image>(_tree, 1);

    // Serialize image
    const auto& size = image->size();
    boost::property_tree::ptree size_tree;
    size_tree.add(s_Width, size[0]);
    size_tree.add(s_Height, size[1]);
    size_tree.add(s_Depth, size[2]);
    _tree.add_child(s_Size, size_tree);

    const auto& type = image->getType();
    _tree.put(s_Type, type.name());

    const auto& format = image->getPixelFormat();
    _tree.put(s_PixelFormat, format);

    // Write image metadata
    const auto& spacing = image->getSpacing();
    boost::property_tree::ptree spacing_tree;
    spacing_tree.add(s_X, spacing[0]);
    spacing_tree.add(s_Y, spacing[1]);
    spacing_tree.add(s_Z, spacing[2]);
    _tree.add_child(s_Spacing, spacing_tree);

    const auto& origin = image->getOrigin();
    boost::property_tree::ptree origin_tree;
    origin_tree.add(s_X, origin[0]);
    origin_tree.add(s_Y, origin[1]);
    origin_tree.add(s_Z, origin[2]);
    _tree.add_child(s_Origin, origin_tree);

    boost::property_tree::ptree window_centers_tree;
    for(std::size_t index = 0 ; const auto& window_center : image->getWindowCenter())
    {
        window_centers_tree.put(s_WindowCenter + std::to_string(index++), window_center);
    }

    _tree.add_child(s_WindowCenters, window_centers_tree);

    boost::property_tree::ptree window_widths_tree;
    for(std::size_t index = 0 ; const auto& window_width : image->getWindowWidth())
    {
        window_widths_tree.put(s_WindowWidth + std::to_string(index++), window_width);
    }

    _tree.add_child(s_WindowWidths, window_widths_tree);

    // Create the output file inside the archive
    const auto& ostream = _archive.openFile(
        get_file_path(image->get_uuid()),
        _password,
        sight::io::zip::Method::DEFAULT,
        sight::io::zip::Level::BEST
    );

#if defined(USE_VTK)
    /// @todo For now, toVTKImage doesn't handle all pixel formats we handle. For example, BGR is written as RGB.
    /// It should be better to convert it, even if, here we don't really care as we save the real pixel format and
    /// restore it back. It is faster, but produce a non standard vti file.
    auto vtk_image = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(image, vtk_image);

    // Create the vtk writer
    const auto& vtk_writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
    vtk_writer->SetCompressorTypeToNone();
    vtk_writer->SetDataModeToBinary();
    vtk_writer->WriteToOutputStringOn();
    vtk_writer->SetInputData(vtk_image);

    // Write to internal string...
    vtk_writer->Update();

    // Write back to the archive
    (*ostream) << vtk_writer->GetOutputString();
#else
    // Write the image data as raw bytes
    ostream->write(reinterpret_cast<const char*>(image->buffer()), std::streamsize(image->getSizeInBytes()));
#endif
}

//------------------------------------------------------------------------------

inline static data::image::sptr read(
    zip::ArchiveReader& _archive,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& _password = ""
)
{
    // Create or reuse the object
    auto image           = helper::cast_or_create<data::image>(_object);
    const auto dump_lock = image->dump_lock();

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::image>(_tree, 0, 1);

    // Deserialize image
    const auto& size_tree        = _tree.get_child(s_Size);
    const data::image::Size size = {
        size_tree.get<size_t>(s_Width),
        size_tree.get<size_t>(s_Height),
        size_tree.get<size_t>(s_Depth)
    };

    core::type type(_tree.get<std::string>(s_Type));

    const auto format = static_cast<data::image::PixelFormat>(_tree.get<int>(s_PixelFormat));

    ///@note This is not saved in VTK files.
    std::vector<double> window_centers;
    for(const auto& value : _tree.get_child(s_WindowCenters))
    {
        window_centers.push_back(value.second.get_value<double>());
    }

    image->setWindowCenter(window_centers);

    std::vector<double> window_widths;
    for(const auto& value : _tree.get_child(s_WindowWidths))
    {
        window_widths.push_back(value.second.get_value<double>());
    }

    image->setWindowWidth(window_widths);

    const auto& spacing_tree = _tree.get_child(s_Spacing);
    image->setSpacing(
        {
            spacing_tree.get<double>(s_X),
            spacing_tree.get<double>(s_Y),
            spacing_tree.get<double>(s_Z)
        });

    const auto& origin_tree = _tree.get_child(s_Origin);
    image->setOrigin(
        {
            origin_tree.get<double>(s_X),
            origin_tree.get<double>(s_Y),
            origin_tree.get<double>(s_Z)
        });

    // If pixelFormart == UNDEFINED it is ALWAYS an empty image, so early return here.
    if(format == data::image::PixelFormat::UNDEFINED)
    {
        return image;
    }

    const auto& serialized_uuid = _tree.get<std::string>(s_uuid);

    // Read the image data
    // Create the istream from the input file inside the archive
    const auto& istream = _archive.openFile(
        get_file_path(serialized_uuid),
        _password
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
    io::vtk::from_vtk_image(vtk_reader->GetOutput(), image);
#endif

    /// @todo We should convert VTK RGB back to BGR if the original image is BGR and we produced a real vti files by
    /// converting BGR to RGB, which is not the case right now (see io::vtk::toVTKImage). For now, we simply switch
    /// back
    /// to the correct pixel type.
    image->resize(size, type, format);

#if !defined(USE_VTK)
    // Read the image data as raw bytes
    istream->read(reinterpret_cast<char*>(image->buffer()), std::streamsize(image->getSizeInBytes()));
#endif

    return image;
}

SIGHT_REGISTER_SERIALIZER(data::image, write, read);

} // namespace sight::io::session::detail::image
