/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/MetaImageReader.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <vtkSmartPointer.h>
#include <vtkMetaImageReader.h>
#include <vtkImageData.h>


fwDataIOReaderRegisterMacro( ::fwVtkIO::MetaImageReader );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

MetaImageReader::MetaImageReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this),
    m_job(::fwJobs::Observer::New("Meta image reader"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

MetaImageReader::~MetaImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void MetaImageReader::read()
{
    using namespace fwVtkIO::helper;
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = this->getConcreteObject();

    vtkSmartPointer< vtkMetaImageReader > reader = vtkSmartPointer< vtkMetaImageReader >::New();
    reader->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkMetaImageReader*>(caller);
            m_job->doneWork( filter->GetProgress()*100 );
        }
        );
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&] { reader->AbortExecuteOn(); });

    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();

    vtkDataObject *obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    FW_RAISE_IF("MetaImageReader cannot read mhd image file :"<<this->getFile().string(), !img);
    try
    {
        ::fwVtkIO::fromVTKImage( img, pImage);
    }
    catch( std::exception &e)
    {
        FW_RAISE("MetaImage to fwData::Image failed : "<<e.what());
    }
}

//------------------------------------------------------------------------------

std::string MetaImageReader::extension()
{
    return ".mhd";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr MetaImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
