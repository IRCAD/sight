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

constexpr static auto s_uuid {"uuid"};
constexpr static auto s_mesh {"/mesh.vtp"};

//------------------------------------------------------------------------------

inline static void write(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::object::csptr object,
    std::map<std::string, data::object::csptr>& /*unused*/,
    const core::crypto::secure_string& password = ""
)
{
    const auto mesh = helper::safe_cast<data::mesh>(object);

    // Add a version number. Not mandatory, but could help for future release
    helper::write_version<data::mesh>(tree, 1);

    // Convert the mesh to VTK
    const auto& vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::toVTKMesh(mesh, vtk_mesh);

    // Create the vtk writer
    const auto& vtkWriter = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    vtkWriter->SetCompressorTypeToNone();
    vtkWriter->SetDataModeToBinary();
    vtkWriter->WriteToOutputStringOn();
    vtkWriter->SetInputData(vtk_mesh);

    // Write to internal string...
    vtkWriter->Update();

    // Create the output file inside the archive
    const auto& ostream = archive.openFile(
        std::filesystem::path(mesh->get_uuid() + s_mesh),
        password
    );

    // Write back to the archive
    (*ostream) << vtkWriter->GetOutputString();
}

//------------------------------------------------------------------------------

inline static data::mesh::sptr read(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::object::sptr>& /*unused*/,
    data::object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto mesh = helper::cast_or_create<data::mesh>(object);

    // Check version number. Not mandatory, but could help for future release
    helper::read_version<data::mesh>(tree, 0, 1);

    // Create the istream from the input file inside the archive
    const auto& uuid    = tree.get<std::string>(s_uuid);
    const auto& istream = archive.openFile(
        std::filesystem::path(uuid + s_mesh),
        password
    );

    // "Convert" it to a string
    const std::string content {std::istreambuf_iterator<char>(*istream), std::istreambuf_iterator<char>()};

    // Create the vtk reader
    const auto& vtk_reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    vtk_reader->ReadFromInputStringOn();
    vtk_reader->SetInputString(content);
    vtk_reader->Update();

    // Convert from VTK
    io::vtk::helper::mesh::fromVTKMesh(vtk_reader->GetOutput(), mesh);

    return mesh;
}

SIGHT_REGISTER_SERIALIZER(data::mesh, write, read);

} // namespace sight::io::session::detail::mesh
