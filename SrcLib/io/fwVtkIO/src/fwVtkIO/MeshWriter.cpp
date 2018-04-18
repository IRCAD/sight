/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
