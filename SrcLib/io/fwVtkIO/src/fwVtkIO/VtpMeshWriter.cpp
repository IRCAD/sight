/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "fwVtkIO/VtpMeshWriter.hpp"

#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>

fwDataIOWriterRegisterMacro( ::fwVtkIO::VtpMeshWriter );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

VtpMeshWriter::VtpMeshWriter(::fwDataIO::writer::IObjectWriter::Key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(::fwJobs::Observer::New("VTP Mesh writer"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

VtpMeshWriter::~VtpMeshWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void VtpMeshWriter::write()
{
    using namespace fwVtkIO::helper;

    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Mesh::csptr pMesh = getConcreteObject();

    vtkSmartPointer< vtkXMLPolyDataWriter > writer = vtkSmartPointer< vtkXMLPolyDataWriter >::New();
    vtkSmartPointer< vtkPolyData > vtkMesh         = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( pMesh, vtkMesh);
    writer->SetInputData( vtkMesh );
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetDataModeToBinary();

    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkXMLPolyDataWriter*>(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        }
        );
    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { writer->AbortExecuteOn(); });

    writer->Update();

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string VtpMeshWriter::extension()
{
    return ".vtp";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr VtpMeshWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
