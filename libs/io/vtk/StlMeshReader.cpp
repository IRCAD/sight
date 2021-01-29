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

#include "io/vtk/StlMeshReader.hpp"

#include "io/vtk/helper/Mesh.hpp"
#include "io/vtk/helper/vtkLambdaCommand.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <io/base/reader/registry/macros.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkSTLReader.h>

fwDataIOReaderRegisterMacro( ::sight::io::vtk::StlMeshReader );

namespace sight::io::vtk
{
//------------------------------------------------------------------------------

StlMeshReader::StlMeshReader(io::base::reader::IObjectReader::Key) :
    data::location::enableSingleFile< io::base::reader::IObjectReader >(this),
    m_job(core::jobs::Observer::New("STL Mesh reader"))
{
}

//------------------------------------------------------------------------------

StlMeshReader::~StlMeshReader()
{
}

//------------------------------------------------------------------------------

void StlMeshReader::read()
{
    SLM_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SLM_ASSERT("Object Lock null.", objectLock );

    const data::Mesh::sptr pMesh = getConcreteObject();

    using namespace sight::io::vtk::helper;

    vtkSmartPointer< vtkSTLReader > reader = vtkSmartPointer< vtkSTLReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer< vtkLambdaCommand > progressCallback;

    progressCallback = vtkSmartPointer< vtkLambdaCommand >::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            const auto filter = static_cast< vtkSTLReader* >(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        }
        );
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { reader->AbortExecuteOn(); });

    reader->Update();

    vtkDataObject* obj = reader->GetOutput();
    vtkPolyData* mesh  = vtkPolyData::SafeDownCast(obj);
    FW_RAISE_IF("StlMeshReader cannot read VTK Mesh file : "<< this->getFile().string(), !mesh);
    io::vtk::helper::Mesh::fromVTKMesh(mesh, pMesh);

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string StlMeshReader::extension()
{
    return ".stl";
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr StlMeshReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
