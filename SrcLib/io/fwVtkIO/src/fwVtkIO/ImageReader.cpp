/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/ImageReader.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

fwDataIOReaderRegisterMacro( ::fwVtkIO::ImageReader );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

ImageReader::ImageReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this),
    m_job(::fwJobs::Observer::New("VTK Image reader"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

ImageReader::~ImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void ImageReader::read()
{
    using namespace fwVtkIO::helper;

    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer<vtkLambdaCommand> progressCallback;
    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [this](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkGenericDataObjectReader*>(caller);
            m_job->doneWork( filter->GetProgress()*100 );
        }
        );
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);


    m_job->addSimpleCancelHook( [&]()
        {
            reader->AbortExecuteOn();
        } );

    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();

    vtkDataObject *obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    FW_RAISE_IF("ImageReader cannot read VTK image file :"<<this->getFile().string(), !img);
    try
    {
        ::fwVtkIO::fromVTKImage( img, pImage);
    }
    catch( std::exception &e)
    {
        FW_RAISE("VTKImage to fwData::Image failed "<<e.what());
    }
}

//------------------------------------------------------------------------------

std::string ImageReader::extension()
{
    return ".vtk";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr ImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------


} // namespace fwVtkIO
