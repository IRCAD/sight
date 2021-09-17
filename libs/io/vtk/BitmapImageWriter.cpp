/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "io/vtk/BitmapImageWriter.hpp"

#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <io/base/writer/registry/macros.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

#include <vtkBMPWriter.h>
#include <vtkImageData.h>
#include <vtkImageWriter.h>
#include <vtkJPEGWriter.h>
#include <vtkPNGWriter.h>
#include <vtkSmartPointer.h>
#include <vtkTIFFWriter.h>

SIGHT_REGISTER_IO_WRITER(sight::io::vtk::BitmapImageWriter);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

BitmapImageWriter::BitmapImageWriter(io::base::writer::IObjectWriter::Key) :
    m_job(core::jobs::Observer::New("Bitmap image writer"))
{
}

//------------------------------------------------------------------------------

BitmapImageWriter::~BitmapImageWriter()
{
}

//------------------------------------------------------------------------------

void BitmapImageWriter::write()
{
    SIGHT_ASSERT("The current object has expired.", !m_object.expired());
    SIGHT_ASSERT("Unable to lock object", m_object.lock());

    const data::Image::csptr pImage = getConcreteObject();

    vtkSmartPointer<vtkImageWriter> writer;

    std::string ext = this->getFile().extension().string();
    ::boost::algorithm::to_lower(ext);

    if(ext == ".bmp")
    {
        writer = vtkSmartPointer<vtkBMPWriter>::New();
    }
    else if(ext == ".jpg" || ext == ".jpeg")
    {
        writer = vtkSmartPointer<vtkJPEGWriter>::New();
    }
    else if(ext == ".png")
    {
        writer = vtkSmartPointer<vtkPNGWriter>::New();
    }
    else if(ext == ".tiff")
    {
        writer = vtkSmartPointer<vtkTIFFWriter>::New();
    }

    const vtkSmartPointer<vtkImageData> vtkImage = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(pImage, vtkImage);

    writer->SetInputData(vtkImage);
    writer->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer<io::vtk::helper::vtkLambdaCommand> progressCallback;
    progressCallback = vtkSmartPointer<io::vtk::helper::vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            auto filter = static_cast<vtkImageWriter*>(caller);
            m_job->doneWork(static_cast<uint64_t>(filter->GetProgress() * 100.0));
        });
    writer->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&]{writer->AbortExecuteOn();});

    writer->Write();

    m_job->finish();
}

//------------------------------------------------------------------------------

std::string BitmapImageWriter::extension() const
{
    return ".bmp .jpg .jpeg .png .pnm .tiff";
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr BitmapImageWriter::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
