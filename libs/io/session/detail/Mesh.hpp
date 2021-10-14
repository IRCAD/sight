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

#include <data/Mesh.hpp>

#include <io/vtk/helper/Mesh.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>

namespace sight::io::session
{

namespace detail::Mesh
{

constexpr static auto s_uuid {"uuid"};
constexpr static auto s_mesh {"/mesh.vtp"};

//------------------------------------------------------------------------------

inline static void serialize(
    zip::ArchiveWriter& archive,
    boost::property_tree::ptree& tree,
    data::Object::csptr object,
    std::map<std::string, data::Object::csptr>&,
    const core::crypto::secure_string& password = ""
)
{
    const auto mesh = Helper::safeCast<data::Mesh>(object);

    // Add a version number. Not mandatory, but could help for future release
    Helper::writeVersion<data::Mesh>(tree, 1);

    // Convert the mesh to VTK
    const auto& vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(mesh, vtkMesh);

    // Create the vtk writer
    const auto& vtkWriter = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    vtkWriter->SetCompressorTypeToNone();
    vtkWriter->SetDataModeToBinary();
    vtkWriter->WriteToOutputStringOn();
    vtkWriter->SetInputData(vtkMesh);

    // Write to internal string...
    vtkWriter->Update();

    // Create the output file inside the archive
    const auto& ostream = archive.openFile(
        std::filesystem::path(mesh->getUUID() + s_mesh),
        password
    );

    // Write back to the archive
    (*ostream) << vtkWriter->GetOutputString();
}

//------------------------------------------------------------------------------

inline static data::Mesh::sptr deserialize(
    zip::ArchiveReader& archive,
    const boost::property_tree::ptree& tree,
    const std::map<std::string, data::Object::sptr>&,
    data::Object::sptr object,
    const core::crypto::secure_string& password = ""
)
{
    // Create or reuse the object
    auto mesh = Helper::safeCast<data::Mesh>(object);

    // Check version number. Not mandatory, but could help for future release
    Helper::readVersion<data::Mesh>(tree, 0, 1);

    // Create the istream from the input file inside the archive
    const auto& uuid    = tree.get<std::string>(s_uuid);
    const auto& istream = archive.openFile(
        std::filesystem::path(uuid + s_mesh),
        password
    );

    // "Convert" it to a string
    const std::string content {std::istreambuf_iterator<char>(*istream), std::istreambuf_iterator<char>()};

    // Create the vtk reader
    const auto& vtkReader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    vtkReader->ReadFromInputStringOn();
    vtkReader->SetInputString(content);
    vtkReader->Update();

    // Convert from VTK
    io::vtk::helper::Mesh::fromVTKMesh(vtkReader->GetOutput(), mesh);

    return mesh;
}

} // namespace detail::Mesh

} // namespace sight::io
