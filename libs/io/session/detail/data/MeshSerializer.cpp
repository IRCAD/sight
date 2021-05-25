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

#include "MeshSerializer.hpp"

#include <data/Mesh.hpp>

#include <io/vtk/helper/Mesh.hpp>

#include <vtkOutputStream.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>

namespace sight::io::session
{

namespace detail::data
{

/// Serialization function
void MeshSerializer::serialize(
    const zip::ArchiveWriter::sptr& archive,
    boost::property_tree::ptree& tree,
    const sight::data::Object::csptr& object,
    std::map<std::string, sight::data::Object::csptr>& children,
    const core::crypto::secure_string& password
) const
{
    const auto& mesh = sight::data::Mesh::dynamicCast(object);
    SIGHT_ASSERT(
        "Object '"
        << (object ? object->getClassname() : sight::data::Object::classname())
        << "' is not a '"
        << sight::data::Mesh::classname()
        << "'",
        mesh
    );

    // Add a version number. Not mandatory, but could help for future release
    tree.put("version", 1);

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
    const auto& ostream = archive->openFile(
        std::filesystem::path(mesh->getUUID() + "/mesh.vtp"),
        password,
        zip::Method::ZSTD,
        zip::Level::BEST
    );

    // Write back to the archive
    (*ostream) << vtkWriter->GetOutputString();
}

} // detail::data

} // namespace sight::io::session
