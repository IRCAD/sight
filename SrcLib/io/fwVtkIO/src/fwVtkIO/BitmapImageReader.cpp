/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVtkIO/BitmapImageReader.hpp"

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"
#include "fwVtkIO/vtk.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Factory.h>
#include <vtkSmartPointer.h>

fwDataIOReaderRegisterMacro( ::fwVtkIO::BitmapImageReader );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

BitmapImageReader::BitmapImageReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this),
    m_job(::fwJobs::Observer::New("Bitmap image reader"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

BitmapImageReader::~BitmapImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void BitmapImageReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    // Use a vtkImageReader2Factory to automatically detect the type of the input file
    // And select the right reader for the file
    vtkSmartPointer<vtkImageReader2Factory> factory =
        vtkSmartPointer< vtkImageReader2Factory >::New();
    vtkSmartPointer<vtkImageReader2> reader =
        factory->CreateImageReader2( this->getFile().string().c_str() );

    FW_RAISE_IF("BitmapImageReader cannot read Bitmap image file :" << this->getFile().string(), !reader);

    reader->SetFileName(this->getFile().string().c_str());

    using namespace fwVtkIO::helper;
    vtkSmartPointer< vtkLambdaCommand > progressCallback;

    progressCallback = vtkSmartPointer< vtkLambdaCommand >::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            auto filter = static_cast<vtkGenericDataObjectReader*>(caller);
            m_job->doneWork(filter->GetProgress() * 100);
        }
        );
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { reader->AbortExecuteOn(); });

    reader->Update();

    vtkDataObject* obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    FW_RAISE_IF("BitmapImageReader cannot read Bitmap image file :"<<this->getFile().string(), !img);
    try
    {
        ::fwVtkIO::fromVTKImage( img, pImage);
    }
    catch( std::exception& e)
    {
        FW_RAISE("BitmapImage to fwData::Image failed "<<e.what());
    }
}

//------------------------------------------------------------------------------

std::string BitmapImageReader::extension()
{
    return "*.jpg *.jpeg *.bmp *.png *.tiff";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr BitmapImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
