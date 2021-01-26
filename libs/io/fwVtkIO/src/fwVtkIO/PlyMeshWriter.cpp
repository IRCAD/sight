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

#include "fwVtkIO/PlyMeshWriter.hpp"

#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <vtkPLYWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

fwDataIOWriterRegisterMacro( ::fwVtkIO::PlyMeshWriter );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

PlyMeshWriter::PlyMeshWriter(::fwDataIO::writer::IObjectWriter::Key) :
    data::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(core::jobs::Observer::New("PLY Mesh writer"))
{
}

//------------------------------------------------------------------------------

PlyMeshWriter::~PlyMeshWriter()
{
}

//------------------------------------------------------------------------------

void PlyMeshWriter::write()
{
    using namespace fwVtkIO::helper;

    SLM_ASSERT("Object pointer expired", !m_object.expired());

    [[maybe_unused]] const auto objectLock = m_object.lock();

    SLM_ASSERT("Object Lock null.", objectLock );

    const data::Mesh::csptr pMesh = getConcreteObject();

    vtkSmartPointer< vtkPLYWriter > writer = vtkSmartPointer< vtkPLYWriter >::New();
    vtkSmartPointer< vtkPolyData > vtkMesh = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( pMesh, vtkMesh);
    writer->SetInputData( vtkMesh );
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetFileTypeToBinary();

    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void* )
        {
            const auto filter = static_cast<vtkPLYWriter*>(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        }
        );
    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { writer->AbortExecuteOn(); });

    writer->Update();

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string PlyMeshWriter::extension()
{
    return ".ply";
}

//------------------------------------------------------------------------------

::core::jobs::IJob::sptr PlyMeshWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
