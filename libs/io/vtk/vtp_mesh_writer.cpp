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

#include "io/vtk/vtp_mesh_writer.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtk_lambda_command.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

vtp_mesh_writer::vtp_mesh_writer() :
    m_job(std::make_shared<core::jobs::observer>("VTP Mesh writer"))
{
}

//------------------------------------------------------------------------------

vtp_mesh_writer::~vtp_mesh_writer()
= default;

//------------------------------------------------------------------------------

void vtp_mesh_writer::write()
{
    using helper::vtk_lambda_command;

    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto object_lock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", object_lock);

    const data::mesh::csptr p_mesh = get_concrete_object();

    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    vtkSmartPointer<vtkPolyData> vtk_mesh        = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(p_mesh, vtk_mesh);
    writer->SetInputData(vtk_mesh);
    writer->SetFileName(this->get_file().string().c_str());
    writer->SetDataModeToBinary();

    vtkSmartPointer<vtk_lambda_command> progress_callback;

    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* const filter = static_cast<vtkXMLPolyDataWriter*>(_caller);
            m_job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    writer->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    m_job->add_simple_cancel_hook([&]{writer->AbortExecuteOn();});

    writer->Update();

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string vtp_mesh_writer::extension() const
{
    return ".vtp";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr vtp_mesh_writer::get_job() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
