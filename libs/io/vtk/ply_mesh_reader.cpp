/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#include "io/vtk/ply_mesh_reader.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtk_lambda_command.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <vtkPLYReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

ply_mesh_reader::ply_mesh_reader() :
    m_job(std::make_shared<core::jobs::observer>("PLY Mesh reader"))
{
}

//------------------------------------------------------------------------------

ply_mesh_reader::~ply_mesh_reader()
= default;

//------------------------------------------------------------------------------

void ply_mesh_reader::read()
{
    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto object_lock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", object_lock);

    const data::mesh::sptr p_mesh = get_concrete_object();

    using helper::vtk_lambda_command;

    vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
    reader->SetFileName(this->get_file().string().c_str());

    vtkSmartPointer<vtk_lambda_command> progress_callback;

    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* const filter = static_cast<vtkPLYReader*>(_caller);
            m_job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    m_job->add_simple_cancel_hook([&]{reader->AbortExecuteOn();});

    reader->Update();

    vtkDataObject* obj = reader->GetOutput();
    vtkPolyData* mesh  = vtkPolyData::SafeDownCast(obj);
    SIGHT_THROW_IF("PlyMeshReader cannot read VTK Mesh file : " << this->get_file().string(), !mesh);
    io::vtk::helper::mesh::from_vtk_mesh(mesh, p_mesh);

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string ply_mesh_reader::extension() const
{
    return ".ply";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr ply_mesh_reader::get_job() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
