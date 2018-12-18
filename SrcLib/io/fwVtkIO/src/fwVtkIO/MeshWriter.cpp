/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwVtkIO/MeshWriter.hpp"

#include "fwVtkIO/helper/Mesh.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <vtkGenericDataObjectWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

fwDataIOWriterRegisterMacro( ::fwVtkIO::MeshWriter );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

MeshWriter::MeshWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(::fwJobs::Observer::New("Mesh writer"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

MeshWriter::~MeshWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void MeshWriter::write()
{
    using namespace fwVtkIO::helper;

    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Mesh::csptr pMesh = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectWriter > writer = vtkSmartPointer< vtkGenericDataObjectWriter >::New();
    vtkSmartPointer< vtkPolyData > vtkMesh               = vtkSmartPointer< vtkPolyData >::New();
    ::fwVtkIO::helper::Mesh::toVTKMesh( pMesh, vtkMesh);
    writer->SetInputData( vtkMesh );
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetFileTypeToBinary();

    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkGenericDataObjectWriter*>(caller);
            m_job->doneWork( filter->GetProgress()*100 );
        }
        );
    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { writer->AbortExecuteOn(); });

    writer->Update();

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string MeshWriter::extension()
{
    return ".vtk";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr MeshWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
