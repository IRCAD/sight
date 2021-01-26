/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "fwVtkIO/MetaImageWriter.hpp"

#include "fwVtkIO/helper/vtkLambdaCommand.hpp"
#include "fwVtkIO/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <fwDataIO/writer/registry/macros.hpp>

#include <vtkImageData.h>
#include <vtkMetaImageWriter.h>
#include <vtkSmartPointer.h>

fwDataIOWriterRegisterMacro( ::fwVtkIO::MetaImageWriter );

namespace fwVtkIO
{
//------------------------------------------------------------------------------

MetaImageWriter::MetaImageWriter(::fwDataIO::writer::IObjectWriter::Key) :
    data::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this),
    m_job(core::jobs::Observer::New("MetaImage writer"))
{
}

//------------------------------------------------------------------------------

MetaImageWriter::~MetaImageWriter()
{
}

//------------------------------------------------------------------------------

void MetaImageWriter::write()
{
    using namespace fwVtkIO::helper;

    assert( !m_object.expired() );
    assert( m_object.lock() );

    data::Image::csptr pImage = getConcreteObject();

    vtkSmartPointer< vtkMetaImageWriter > writer = vtkSmartPointer< vtkMetaImageWriter >::New();
    vtkSmartPointer< vtkImageData > vtkImage     = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( pImage, vtkImage );
    writer->SetInputData( vtkImage );
    writer->SetFileName( this->getFile().string().c_str() );
    writer->SetCompression(true);

    vtkSmartPointer<vtkLambdaCommand> progressCallback;
    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback([this](vtkObject* caller, long unsigned int, void* )
        {
            auto filter = static_cast<vtkMetaImageWriter*>(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
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

std::string MetaImageWriter::extension()
{
    return ".mhd";
}

//------------------------------------------------------------------------------

::core::jobs::IJob::sptr MetaImageWriter::getJob() const
{
    return m_job;
}

} // namespace fwVtkIO
