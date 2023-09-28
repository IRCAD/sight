/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/vtk/ImageWriter.hpp"

#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/observer.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <vtkGenericDataObjectWriter.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

SIGHT_REGISTER_IO_WRITER(sight::io::vtk::ImageWriter);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

ImageWriter::ImageWriter() :
    m_job(std::make_shared<core::jobs::observer>("VTK Image Writer"))
{
}

//------------------------------------------------------------------------------

ImageWriter::~ImageWriter()
= default;

//------------------------------------------------------------------------------

void ImageWriter::write()
{
    using helper::vtkLambdaCommand;

    assert(!m_object.expired());
    assert(m_object.lock());

    data::Image::csptr pImage = getConcreteObject();

    vtkSmartPointer<vtkGenericDataObjectWriter> writer = vtkSmartPointer<vtkGenericDataObjectWriter>::New();
    vtkSmartPointer<vtkImageData> vtkImage             = vtkSmartPointer<vtkImageData>::New();
    io::vtk::toVTKImage(pImage, vtkImage);
    writer->SetInputData(vtkImage);
    writer->SetFileName(this->get_file().string().c_str());
    writer->SetFileTypeToBinary();

    vtkSmartPointer<vtkLambdaCommand> progress_callback;
    progress_callback = vtkSmartPointer<vtkLambdaCommand>::New();
    progress_callback->SetCallback(
        [this](vtkObject* caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkGenericDataObjectWriter*>(caller);
            m_job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });

    writer->AddObserver(vtkCommand::ProgressEvent, progress_callback);
    m_job->add_simple_cancel_hook(
        [&]()
        {
            writer->AbortExecuteOn();
        });
    writer->Write();
    m_job->finish();
}

//------------------------------------------------------------------------------

std::string ImageWriter::extension() const
{
    return ".vtk";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr ImageWriter::getJob() const
{
    return m_job;
}

} // namespace sight::io::vtk
