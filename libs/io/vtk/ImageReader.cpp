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

#include "io/vtk/helper/vtk_lambda_command.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <io/__/reader/registry/macros.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

SIGHT_REGISTER_IO_READER(sight::io::vtk::image_reader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

image_reader::image_reader() :
    m_job(std::make_shared<core::jobs::observer>("VTK image reader"))
{
}

//------------------------------------------------------------------------------

image_reader::~image_reader()
= default;

//------------------------------------------------------------------------------

void image_reader::read()
{
    using helper::vtk_lambda_command;

    assert(!m_object.expired());
    assert(m_object.lock());

    data::image::sptr p_image = get_concrete_object();

    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(this->get_file().string().c_str());

    vtkSmartPointer<vtk_lambda_command> progress_callback;
    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [this](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkGenericDataObjectReader*>(_caller);
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
        io::vtk::from_vtk_image(img, p_image);
    }
    catch(std::exception& e)
    {
        SIGHT_THROW("VTKImage to data::image failed " << e.what());
    }
}

//------------------------------------------------------------------------------

std::string image_reader::extension() const
{
    return ".vtk";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr image_reader::get_job() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
