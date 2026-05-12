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

#include "io/vtk/obj_mesh_writer.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtk_lambda_command.hpp"

#include <core/progress/monitor.hpp>
#include <core/progress/observer.hpp>

#include <data/mesh.hpp>

#include <vtkOBJWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <cstdint>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

void obj_mesh_writer::write(sight::core::progress::observer::sptr _progress)
{
    auto object_lock = get_object();

    SIGHT_ASSERT("Object Lock null.", object_lock);

    const data::mesh::csptr mesh = get_concrete_object();

    vtkSmartPointer<vtkOBJWriter> writer  = vtkSmartPointer<vtkOBJWriter>::New();
    vtkSmartPointer<vtkPolyData> vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
    sight::io::vtk::helper::mesh::to_vtk_mesh(mesh, vtk_mesh);
    writer->SetInputData(vtk_mesh);
    writer->SetFileName(this->get_file().string().c_str());

    vtkSmartPointer<helper::vtk_lambda_command> progress_callback;

    progress_callback = vtkSmartPointer<helper::vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkOBJWriter*>(_caller);
            _progress->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    writer->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    _progress->add_cancel_hook([&]{writer->AbortExecuteOn();});

    writer->Update();
}

//------------------------------------------------------------------------------

std::string obj_mesh_writer::extension() const
{
    return ".obj";
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
