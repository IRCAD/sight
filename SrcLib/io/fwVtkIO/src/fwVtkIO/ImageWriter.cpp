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

#include "fwVtkIO/ImageWriter.hpp"

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"
#include "fwVtkIO/vtk.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwJobs/Observer.hpp>

#include <vtkGenericDataObjectWriter.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

fwDataIOWriterRegisterMacro( ::fwVtkIO::ImageWriter );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

ImageWriter::ImageWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(::fwJobs::Observer::New("VTK Image Writer"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ImageWriter::~ImageWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ImageWriter::write()
{
    using namespace fwVtkIO::helper;

    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::csptr pImage = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectWriter > writer = vtkSmartPointer< vtkGenericDataObjectWriter >::New();
    vtkSmartPointer< vtkImageData > vtkImage             = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( pImage, vtkImage );
    writer->SetInputData( vtkImage );
    writer->SetFileName(this->getFile().string().c_str());
    writer->SetFileTypeToBinary();

    vtkSmartPointer<vtkLambdaCommand> progressCallback;
    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback([this](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkGenericDataObjectWriter*>(caller);
            m_job->doneWork( filter->GetProgress()*100 );
        });

    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);
    m_job->addSimpleCancelHook( [&]()
        {
            writer->AbortExecuteOn();
        });
    writer->Write();
    m_job->finish();
}

//------------------------------------------------------------------------------

std::string ImageWriter::extension()
{
    return ".vtk";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr ImageWriter::getJob() const
{
    return m_job;
}

} // namespace fwVtkIO
