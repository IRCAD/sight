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

#include "io/vtk/ImageReader.hpp"

#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <io/__/reader/registry/macros.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

SIGHT_REGISTER_IO_READER(sight::io::vtk::ImageReader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

ImageReader::ImageReader() :
    m_job(std::make_shared<core::jobs::observer>("VTK image reader"))
{
}

//------------------------------------------------------------------------------

ImageReader::~ImageReader()
= default;

//------------------------------------------------------------------------------

void ImageReader::read()
{
    using helper::vtkLambdaCommand;

    assert(!m_object.expired());
    assert(m_object.lock());

    data::image::sptr pImage = getConcreteObject();

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(this->get_file().string().c_str());

    vtkSmartPointer<vtkLambdaCommand> progress_callback;
    progress_callback = vtkSmartPointer<vtkLambdaCommand>::New();
    progress_callback->SetCallback(
        [this](vtkObject* caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkGenericDataObjectReader*>(caller);
            m_job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    m_job->add_simple_cancel_hook(
        [&]()
        {
            reader->AbortExecuteOn();
        });

    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();

    vtkDataObject* obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    SIGHT_THROW_IF("ImageReader cannot read VTK image file :" << this->get_file().string(), !img);
    try
    {
        io::vtk::fromVTKImage(img, pImage);
    }
    catch(std::exception& e)
    {
        SIGHT_THROW("VTKImage to data::image failed " << e.what());
    }
}

//------------------------------------------------------------------------------

std::string ImageReader::extension() const
{
    return ".vtk";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr ImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
