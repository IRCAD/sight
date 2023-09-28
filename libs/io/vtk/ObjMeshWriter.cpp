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

#include "io/vtk/ObjMeshWriter.hpp"

#include "io/vtk/helper/Mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"

#include <core/base.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <data/Mesh.hpp>
#include <data/Material.hpp>

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

SIGHT_REGISTER_IO_WRITER(sight::io::vtk::ObjMeshWriter);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

ObjMeshWriter::ObjMeshWriter() :
    m_job(std::make_shared<core::jobs::observer>("OBJ Mesh writer"))
{
}

//------------------------------------------------------------------------------

ObjMeshWriter::~ObjMeshWriter()
= default;

//------------------------------------------------------------------------------

#ifdef USE_OBJ_WRITER
void ObjMeshWriter::write()
{
    using namespace sight::io::vtk::helper;

    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", objectLock);

    const data::Mesh::csptr pMesh = getConcreteObject();

    vtkSmartPointer<vtkOBJWriter> writer = vtkSmartPointer<vtkOBJWriter>::New();
    vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(pMesh, vtkMesh);
    writer->SetInputData(vtkMesh);
    writer->SetFileName(this->get_file().string().c_str());

    vtkSmartPointer<vtkLambdaCommand> progress_callback;

    progress_callback = vtkSmartPointer<vtkLambdaCommand>::New();
    progress_callback->SetCallback(
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

void ObjMeshWriter::write()
{
    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", objectLock);

    const data::Mesh::csptr pMesh        = getConcreteObject();
    vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(pMesh, vtkMesh);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkActor> actor       = vtkSmartPointer<vtkActor>::New();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(vtkMesh);
    actor->SetMapper(mapper);

    vtkProperty* property = actor->GetProperty();

    property->SetSpecularColor(1., 1., 1.);
    property->SetSpecularPower(100.); //Shininess

    property->SetInterpolationToPhong();

    renderer->AddActor(actor);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    renderWindow->AddRenderer(renderer);

    std::filesystem::path file = this->get_file();
    const std::string filename = file.extension() == ".obj"
                                 ? file.replace_extension().string()
                                 : file.string();

    vtkSmartPointer<vtkOBJExporter> exporter = vtkSmartPointer<vtkOBJExporter>::New();
    exporter->SetRenderWindow(renderWindow);
    exporter->SetFilePrefix(filename.c_str());
    exporter->Write();
    m_job->finish();
}
#endif // ifdef USE_OBJ_WRITER

//------------------------------------------------------------------------------

std::string ObjMeshWriter::extension() const
{
    return ".obj";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr ObjMeshWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
