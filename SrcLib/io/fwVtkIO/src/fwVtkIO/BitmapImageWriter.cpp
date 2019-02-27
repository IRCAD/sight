/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "fwVtkIO/BitmapImageWriter.hpp"

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"
#include "fwVtkIO/vtk.hpp"

#include <fwDataIO/writer/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include <vtkBMPWriter.h>
#include <vtkImageData.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkPNMWriter.h>
#include <vtkSmartPointer.h>
#include <vtkTIFFWriter.h>

fwDataIOWriterRegisterMacro( ::fwVtkIO::BitmapImageWriter );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

BitmapImageWriter::BitmapImageWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(::fwJobs::Observer::New("Bitmap image writer"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

BitmapImageWriter::~BitmapImageWriter()
{
}

//------------------------------------------------------------------------------

void BitmapImageWriter::write()
{
    SLM_ASSERT("The current object has expired.", !m_object.expired() );
    SLM_ASSERT("Unable to lock object", m_object.lock() );

    const ::fwData::Image::csptr pImage = getConcreteObject();

    vtkSmartPointer< vtkImageWriter > writer;

    std::string ext = ::boost::filesystem::extension(this->getFile());
    ::boost::algorithm::to_lower(ext);

    if(ext == ".bmp")
    {
        writer = vtkSmartPointer< vtkBMPWriter >::New();
    }
    else if(ext == ".jpg" || ext == ".jpeg")
    {
        writer = vtkSmartPointer< vtkJPEGWriter >::New();
    }
    else if(ext == ".png")
    {
        writer = vtkSmartPointer< vtkPNGWriter >::New();
    }
    else if(ext == ".pnm")
    {
        writer = vtkSmartPointer< vtkPNMWriter >::New();
    }
    else if(ext == ".tiff")
    {
        writer = vtkSmartPointer< vtkTIFFWriter >::New();
    }

    const vtkSmartPointer< vtkImageData > vtkImage = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( pImage, vtkImage );

    writer->SetInputData( vtkImage );
    writer->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand > progressCallback;
    progressCallback = vtkSmartPointer< ::fwVtkIO::helper::vtkLambdaCommand >::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            auto filter = static_cast<vtkImageWriter*>(caller);
            m_job->doneWork(static_cast<uint64_t>(filter->GetProgress() * 100.0));
        }
        );
    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { writer->AbortExecuteOn(); });

    writer->Write();

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string BitmapImageWriter::extension()
{
    return ".bmp .jpg .jpeg .png .pnm .tiff";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr BitmapImageWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
