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

#include <data/mesh.hpp>

#include <io/vtk/helper/mesh.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

namespace sight::io::session::detail::mesh
{

constexpr static auto UUID {"uuid"};
constexpr static auto MESH {"/mesh.vtp"};

//------------------------------------------------------------------------------

inline static void write(
    zip::archive_writer& _archive,
    boost::property_tree::ptree& _tree,
    data::object::csptr _object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& _password = ""
)
{
    const auto mesh = helper::safe_cast<data::mesh>(_object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::mesh>(_tree, 1);

    // Convert the mesh to VTK
    const auto& vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(mesh, vtk_mesh);

    // Create the vtk writer
    const auto& vtk_writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    vtk_writer->SetCompressorTypeToNone();
    vtk_writer->SetDataModeToBinary();
    vtk_writer->WriteToOutputStringOn();
    vtk_writer->SetInputData(vtk_mesh);

    // Write to internal string...
    vtk_writer->Update();

    // Create the output file inside the archive
    const auto& ostream = _archive.open_file(
        std::filesystem::path(mesh->get_uuid() + MESH),
        _password
    );

    // Write back to the archive
    (*ostream) << vtk_writer->GetOutputString();
}

//------------------------------------------------------------------------------

inline static data::mesh::sptr read(
    zip::archive_reader& _archive,
    const boost::property_tree::ptree& _tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr _object,
    const core::crypto::secure_string& _password = ""
)
{
    // Create or reuse the object
    auto mesh = helper::cast_or_create<data::mesh>(_object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::mesh>(_tree, 0, 1);

    // Create the istream from the input file inside the archive
    const auto& uuid    = _tree.get<std::string>(UUID);
    const auto& istream = _archive.open_file(
        std::filesystem::path(uuid + MESH),
        _password
    );

    // "Convert" it to a string
    const std::string content {std::istreambuf_iterator<char>(*istream), std::istreambuf_iterator<char>()};

    // Create the vtk reader
    const auto& vtk_reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    vtk_reader->ReadFromInputStringOn();
    vtk_reader->SetInputString(content);
    vtk_reader->Update();

    // Convert from VTK
    io::vtk::helper::mesh::from_vtk_mesh(vtk_reader->GetOutput(), mesh);

    return mesh;
}

SIGHT_REGISTER_SERIALIZER(data::mesh, write, read);

} // namespace sight::io::session::detail::mesh
