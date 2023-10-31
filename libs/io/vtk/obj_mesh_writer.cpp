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

#include "io/vtk/obj_mesh_writer.hpp"

#include "io/vtk/helper/mesh.hpp"
#include "io/vtk/helper/vtk_lambda_command.hpp"

#include <core/base.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <data/mesh.hpp>
#include <data/material.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <vtkVersion.h>
#if VTK_MAJOR_VERSION >= 8 && VTK_MINOR_VERSION >= 2
#define USE_OBJ_WRITER // Shorter than test major.minor version later.
#include <vtkOBJWriter.h>
#else
#include <vtkActor.h>
#include <vtkOBJExporter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#endif

SIGHT_REGISTER_IO_WRITER(sight::io::vtk::obj_mesh_writer);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

obj_mesh_writer::obj_mesh_writer() :
    m_job(std::make_shared<core::jobs::observer>("OBJ Mesh writer"))
{
}

//------------------------------------------------------------------------------

obj_mesh_writer::~obj_mesh_writer()
= default;

//------------------------------------------------------------------------------

#ifdef USE_OBJ_WRITER
void obj_mesh_writer::write()
{
    using namespace sight::io::vtk::helper;

    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", objectLock);

    const data::mesh::csptr pMesh = get_concrete_object();

    vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
    vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(pMesh, vtkMesh);
    writer->SetInputData(vtkMesh);
    writer->SetFileName(this->get_file().string().c_str());

    vtkSmartPointer<vtk_lambda_command> progress_callback;

    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            const auto filter = static_cast<vtkOBJWriter*>(caller);
            m_job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    writer->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    m_job->add_simple_cancel_hook([&]{writer->AbortExecuteOn();});

    writer->Update();

    m_job->finish();
}
#else
//------------------------------------------------------------------------------

void obj_mesh_writer::write()
{
    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto object_lock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", object_lock);

    const data::mesh::csptr p_mesh        = get_concrete_object();
    vtkSmartPointer<vtkPolyData> vtk_mesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::mesh::to_vtk_mesh(p_mesh, vtk_mesh);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkActor> actor       = vtkSmartPointer<vtkActor>::New();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(vtk_mesh);
    actor->SetMapper(mapper);

    vtkProperty* property = actor->GetProperty();

    property->SetSpecularColor(1., 1., 1.);
    property->SetSpecularPower(100.); //Shininess

    property->SetInterpolationToPhong();

    renderer->AddActor(actor);

    vtkSmartPointer<vtkRenderWindow> render_window = vtkSmartPointer<vtkRenderWindow>::New();
    render_window->AddRenderer(renderer);

    std::filesystem::path file = this->get_file();
    const std::string filename = file.extension() == ".obj"
                                 ? file.replace_extension().string()
                                 : file.string();

    vtkSmartPointer<vtkOBJExporter> exporter = vtkSmartPointer<vtkOBJExporter>::New();
    exporter->SetRenderWindow(render_window);
    exporter->SetFilePrefix(filename.c_str());
    exporter->Write();
    m_job->finish();
}
#endif // ifdef USE_OBJ_WRITER

//------------------------------------------------------------------------------

std::string obj_mesh_writer::extension() const
{
    return ".obj";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr obj_mesh_writer::get_job() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
