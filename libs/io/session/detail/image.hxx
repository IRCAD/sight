/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
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

#include <sight/io/session/config.hpp>

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

constexpr static auto UUID {"uuid"};
constexpr static auto WINDOW_CENTERS {"WindowCenters"};
constexpr static auto WINDOW_WIDTHS {"WindowWidths"};
constexpr static auto WINDOW_CENTER {"WindowCenter"};
constexpr static auto WINDOW_WIDTH {"WindowWidth"};
constexpr static auto TYPE {"Type"};
constexpr static auto PIXEL_FORMAT {"PixelFormat"};
constexpr static auto SPACING {"Spacing"};
constexpr static auto ORIGIN {"Origin"};
constexpr static auto ORIENTATION {"Orientation"};
constexpr static auto SIZE {"Size"};
constexpr static auto X {"X"};
constexpr static auto Y {"Y"};
constexpr static auto Z {"Z"};
constexpr static auto X_X {"X_X"};
constexpr static auto X_Y {"X_Y"};
constexpr static auto X_Z {"X_Z"};
constexpr static auto Y_X {"Y_X"};
constexpr static auto Y_Y {"Y_Y"};
constexpr static auto Y_Z {"Y_Z"};
constexpr static auto Z_X {"Z_X"};
constexpr static auto Z_Y {"Z_Y"};
constexpr static auto Z_Z {"Z_Z"};
constexpr static auto WIDTH {"Width"};
constexpr static auto HEIGHT {"Height"};
constexpr static auto DEPTH {"Depth"};

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
    zip::archive_writer& _archive,
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
    size_tree.add(WIDTH, size[0]);
    size_tree.add(HEIGHT, size[1]);
    size_tree.add(DEPTH, size[2]);
    _tree.add_child(SIZE, size_tree);

    const auto& type = image->type();
    _tree.put(TYPE, type.name());

    const auto& format = image->pixel_format();
    _tree.put(PIXEL_FORMAT, format);

    // Write image metadata
    const auto& spacing = image->spacing();
    boost::property_tree::ptree spacing_tree;
    spacing_tree.add(X, spacing[0]);
    spacing_tree.add(Y, spacing[1]);
    spacing_tree.add(Z, spacing[2]);
    _tree.add_child(SPACING, spacing_tree);

    const auto& origin = image->origin();
    boost::property_tree::ptree origin_tree;
    origin_tree.add(X, origin[0]);
    origin_tree.add(Y, origin[1]);
    origin_tree.add(Z, origin[2]);
    _tree.add_child(ORIGIN, origin_tree);

    const auto& orientation = image->orientation();
    boost::property_tree::ptree orientation_tree;
    orientation_tree.add(X_X, orientation[0]);
    orientation_tree.add(Y_X, orientation[1]);
    orientation_tree.add(Z_X, orientation[2]);
    orientation_tree.add(X_Y, orientation[3]);
    orientation_tree.add(Y_Y, orientation[4]);
    orientation_tree.add(Z_Y, orientation[5]);
    orientation_tree.add(X_Z, orientation[6]);
    orientation_tree.add(Y_Z, orientation[7]);
    orientation_tree.add(Z_Z, orientation[8]);
    _tree.add_child(ORIENTATION, orientation_tree);

    boost::property_tree::ptree window_centers_tree;
    for(std::size_t index = 0 ; const auto& window_center : image->window_center())
    {
        window_centers_tree.put(WINDOW_CENTER + std::to_string(index++), window_center);
    }

    _tree.add_child(WINDOW_CENTERS, window_centers_tree);

    boost::property_tree::ptree window_widths_tree;
    for(std::size_t index = 0 ; const auto& window_width : image->window_width())
    {
        window_widths_tree.put(WINDOW_WIDTH + std::to_string(index++), window_width);
    }

    _tree.add_child(WINDOW_WIDTHS, window_widths_tree);

    // Create the output file inside the archive
    const auto& ostream = _archive.open_file(
        get_file_path(image->get_uuid()),
        _password,
        sight::io::zip::method::DEFAULT,
        sight::io::zip::level::best
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
    ostream->write(reinterpret_cast<const char*>(image->buffer()), std::streamsize(image->size_in_bytes()));
#endif
}

//------------------------------------------------------------------------------

inline static data::image::sptr read(
    zip::archive_reader& _archive,
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
    const auto& size_tree          = _tree.get_child(SIZE);
    const data::image::size_t size = {
        size_tree.get<size_t>(WIDTH),
        size_tree.get<size_t>(HEIGHT),
        size_tree.get<size_t>(DEPTH)
    };

    core::type type(_tree.get<std::string>(TYPE));

    const auto format = static_cast<enum data::image::pixel_format>(_tree.get<int>(PIXEL_FORMAT));

    ///@note This is not saved in VTK files.
    std::vector<double> window_centers;
    for(const auto& value : _tree.get_child(WINDOW_CENTERS))
    {
        window_centers.push_back(value.second.get_value<double>());
    }

    image->set_window_center(window_centers);

    std::vector<double> window_widths;
    for(const auto& value : _tree.get_child(WINDOW_WIDTHS))
    {
        window_widths.push_back(value.second.get_value<double>());
    }

    image->set_window_width(window_widths);

    const auto& spacing_tree = _tree.get_child(SPACING);
    image->set_spacing(
        {
            spacing_tree.get<double>(X),
            spacing_tree.get<double>(Y),
            spacing_tree.get<double>(Z)
        });

    const auto& origin_tree = _tree.get_child(ORIGIN);
    image->set_origin(
        {
            origin_tree.get<double>(X),
            origin_tree.get<double>(Y),
            origin_tree.get<double>(Z)
        });

    const auto& orientation_tree = _tree.get_child(ORIENTATION);
    image->set_orientation(
        {
            orientation_tree.get<double>(X_X),
            orientation_tree.get<double>(Y_X),
            orientation_tree.get<double>(Z_X),
            orientation_tree.get<double>(X_Y),
            orientation_tree.get<double>(Y_Y),
            orientation_tree.get<double>(Z_Y),
            orientation_tree.get<double>(X_Z),
            orientation_tree.get<double>(Y_Z),
            orientation_tree.get<double>(Z_Z)
        });

    // If pixelFormart == UNDEFINED it is ALWAYS an empty image, so early return here.
    if(format == data::image::pixel_format::undefined)
    {
        return image;
    }

    const auto& serialized_uuid = _tree.get<std::string>(UUID);

    // Read the image data
    // Create the istream from the input file inside the archive
    const auto& istream = _archive.open_file(
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
    istream->read(reinterpret_cast<char*>(image->buffer()), std::streamsize(image->size_in_bytes()));
#endif

    return image;
}

SIGHT_REGISTER_SERIALIZER(data::image, write, read);

} // namespace sight::io::session::detail::image
