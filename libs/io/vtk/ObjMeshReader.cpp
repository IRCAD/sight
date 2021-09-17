/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include "io/vtk/ObjMeshReader.hpp"

#include "io/vtk/helper/Mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <io/base/reader/registry/macros.hpp>

#include <vtkOBJReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

SIGHT_REGISTER_IO_READER(::sight::io::vtk::ObjMeshReader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

ObjMeshReader::ObjMeshReader(io::base::reader::IObjectReader::Key) :
    m_job(core::jobs::Observer::New("OBJ Mesh reader"))
{
}

//------------------------------------------------------------------------------

ObjMeshReader::~ObjMeshReader()
{
}

//------------------------------------------------------------------------------

void ObjMeshReader::read()
{
    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", objectLock);

    const data::Mesh::sptr pMesh = getConcreteObject();

    using namespace sight::io::vtk::helper;

    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            const auto filter = static_cast<vtkOBJReader*>(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&]{reader->AbortExecuteOn();});

    reader->Update();

    vtkDataObject* obj = reader->GetOutput();
    vtkPolyData* mesh  = vtkPolyData::SafeDownCast(obj);
    SIGHT_THROW_IF("ObjMeshReader cannot read VTK Mesh file : " << this->getFile().string(), !mesh);
    io::vtk::helper::Mesh::fromVTKMesh(mesh, pMesh);

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string ObjMeshReader::extension() const
{
    return ".obj";
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr ObjMeshReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
