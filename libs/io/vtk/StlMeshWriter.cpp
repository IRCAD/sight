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

#include "io/vtk/StlMeshWriter.hpp"

#include "io/vtk/helper/Mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <io/base/writer/registry/macros.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSTLWriter.h>

SIGHT_REGISTER_IO_WRITER(sight::io::vtk::StlMeshWriter);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

StlMeshWriter::StlMeshWriter(io::base::writer::IObjectWriter::Key) :
    m_job(core::jobs::Observer::New("STL Mesh writer"))
{
}

//------------------------------------------------------------------------------

StlMeshWriter::~StlMeshWriter()
{
}

//------------------------------------------------------------------------------

void StlMeshWriter::write()
{
    using namespace sight::io::vtk::helper;

    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", objectLock);

    const data::Mesh::csptr pMesh = getConcreteObject();

    vtkSmartPointer<vtkSTLWriter> writer = vtkSmartPointer<vtkSTLWriter>::New();
    vtkSmartPointer<vtkPolyData> vtkMesh = vtkSmartPointer<vtkPolyData>::New();
    io::vtk::helper::Mesh::toVTKMesh(pMesh, vtkMesh);
    writer->SetInputData(vtkMesh);
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetFileTypeToBinary();

    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            const auto filter = static_cast<vtkSTLWriter*>(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&]{writer->AbortExecuteOn();});

    writer->Update();

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string StlMeshWriter::extension() const
{
    return ".stl";
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr StlMeshWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
