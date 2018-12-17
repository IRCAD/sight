/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "fwVtkIO/vtk.hpp"
#include "fwVtkIO/VtiImageReader.hpp"
#include "fwVtkIO/helper/vtkLambdaCommand.hpp"

#include <fwCore/base.hpp>

#include <fwDataIO/reader/registry/macros.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>
#include <vtkImageData.h>

fwDataIOReaderRegisterMacro( ::fwVtkIO::VtiImageReader );


namespace fwVtkIO
{
//------------------------------------------------------------------------------

VtiImageReader::VtiImageReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::reader::IObjectReader >(this),
    m_job(::fwJobs::Observer::New("Vti image reader"))
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

VtiImageReader::~VtiImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void VtiImageReader::read()
{
    assert( !m_object.expired() );
    assert( m_object.lock() );

    ::fwData::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer< vtkXMLImageDataReader > reader = vtkSmartPointer< vtkXMLImageDataReader >::New();
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
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();

    vtkDataObject *obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    FW_RAISE_IF("VtiImageReader cannot read Vti image file :"<<this->getFile().string(), !img);
    try
    {
        ::fwVtkIO::fromVTKImage( img, pImage);
    }
    catch( std::exception &e)
    {
        FW_RAISE("VTIImage to fwData::Image failed "<<e.what());
    }
}

//------------------------------------------------------------------------------

std::string VtiImageReader::extension()
{
    return ".vti";
}

//------------------------------------------------------------------------------

::fwJobs::IJob::sptr VtiImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace fwVtkIO
