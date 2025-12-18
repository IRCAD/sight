/************************************************************************
 *
 * Copyright (C) 2020-2025 IRCAD France
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

#include "io/vtk/stl_mesh_writer.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtk_lambda_command.hpp"

#include <core/base.hpp>
#include <core/progress/monitor.hpp>
#include <core/progress/observer.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSTLWriter.h>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

void stl_mesh_writer::write(sight::core::progress::observer::sptr _progress)
{
    using helper::vtk_lambda_command;

    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto object_lock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", object_lock);

    const data::mesh::csptr p_mesh = get_concrete_object();

    vtkSmartPointer<vtkSTLWriter> writer  = vtkSmartPointer<vtkSTLWriter>::New();
    vtkSmartPointer<vtkPolyData> vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(p_mesh, vtk_mesh);
    writer->SetInputData(vtk_mesh);
    writer->SetFileName(this->get_file().string().c_str());
    writer->SetFileTypeToBinary();

    vtkSmartPointer<vtk_lambda_command> progress_callback;

    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* const filter = static_cast<vtkSTLWriter*>(_caller);
            _progress->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    writer->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    _progress->add_cancel_hook([&]{writer->AbortExecuteOn();});

    writer->Update();
}

//------------------------------------------------------------------------------

std::string stl_mesh_writer::extension() const
{
    return ".stl";
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
