/************************************************************************
 *
 * Copyright (C) 2020-2026 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "io/vtk/vtp_mesh_reader.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtk_lambda_command.hpp"

#include <core/progress/monitor.hpp>
#include <core/progress/observer.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLGenericDataObjectReader.h>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

void vtp_mesh_reader::read(sight::core::progress::observer::sptr _progress)
{
    auto object_lock = get_object();

    SIGHT_ASSERT("Object Lock null.", object_lock);

    const data::mesh::sptr p_mesh = get_concrete_object();

    using helper::vtk_lambda_command;

    vtkSmartPointer<vtkXMLGenericDataObjectReader> reader = vtkSmartPointer<vtkXMLGenericDataObjectReader>::New();
    reader->SetFileName(this->get_file().string().c_str());

    vtkSmartPointer<vtk_lambda_command> progress_callback;

    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* const filter = static_cast<vtkXMLGenericDataObjectReader*>(_caller);
            _progress->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    _progress->add_cancel_hook([&]{reader->AbortExecuteOn();});

    reader->Update();

    vtkDataObject* obj = reader->GetOutput();
    vtkPolyData* mesh  = vtkPolyData::SafeDownCast(obj);
    SIGHT_THROW_IF("VtpMeshReader cannot read VTK Mesh file : " << this->get_file().string(), !mesh);
    io::vtk::helper::mesh::from_vtk_mesh(mesh, p_mesh);
}

//------------------------------------------------------------------------------

std::string vtp_mesh_reader::extension() const
{
    return ".vtp";
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
