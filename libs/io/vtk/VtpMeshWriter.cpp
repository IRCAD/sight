/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "io/vtk/VtpMeshWriter.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>

SIGHT_REGISTER_IO_WRITER(sight::io::vtk::VtpMeshWriter);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

VtpMeshWriter::VtpMeshWriter() :
    m_job(std::make_shared<core::jobs::observer>("VTP Mesh writer"))
{
}

//------------------------------------------------------------------------------

VtpMeshWriter::~VtpMeshWriter()
= default;

//------------------------------------------------------------------------------

void VtpMeshWriter::write()
{
    using helper::vtkLambdaCommand;

    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto object_lock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", object_lock);

    const data::mesh::csptr p_mesh = getConcreteObject();

    vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
    vtkSmartPointer<vtkPolyData> vtk_mesh        = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::toVTKMesh(p_mesh, vtk_mesh);
    writer->SetInputData(vtk_mesh);
    writer->SetFileName(this->get_file().string().c_str());
    writer->SetDataModeToBinary();

    vtkSmartPointer<vtkLambdaCommand> progress_callback;

    progress_callback = vtkSmartPointer<vtkLambdaCommand>::New();
    progress_callback->SetCallback(
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

std::string VtpMeshWriter::extension() const
{
    return ".vtp";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr VtpMeshWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
