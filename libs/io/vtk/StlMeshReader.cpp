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

#include "io/vtk/StlMeshReader.hpp"

#include "io/vtk/helper/Mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <io/__/reader/registry/macros.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>

SIGHT_REGISTER_IO_READER(sight::io::vtk::StlMeshReader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

StlMeshReader::StlMeshReader() :
    m_job(std::make_shared<core::jobs::observer>("STL Mesh reader"))
{
}

//------------------------------------------------------------------------------

StlMeshReader::~StlMeshReader()
= default;

//------------------------------------------------------------------------------

void StlMeshReader::read()
{
    SIGHT_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SIGHT_ASSERT("Object Lock null.", objectLock);

    const data::Mesh::sptr pMesh = getConcreteObject();

    using helper::vtkLambdaCommand;

    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(this->get_file().string().c_str());

    vtkSmartPointer<vtkLambdaCommand> progress_callback;

    progress_callback = vtkSmartPointer<vtkLambdaCommand>::New();
    progress_callback->SetCallback(
        [&](vtkObject* caller, std::uint64_t, void*)
        {
            auto* const filter = static_cast<vtkSTLReader*>(caller);
            m_job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    m_job->add_simple_cancel_hook([&]{reader->AbortExecuteOn();});

    reader->Update();

    vtkDataObject* obj = reader->GetOutput();
    vtkPolyData* mesh  = vtkPolyData::SafeDownCast(obj);
    SIGHT_THROW_IF("StlMeshReader cannot read VTK Mesh file : " << this->get_file().string(), !mesh);
    io::vtk::helper::Mesh::fromVTKMesh(mesh, pMesh);

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string StlMeshReader::extension() const
{
    return ".stl";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr StlMeshReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
