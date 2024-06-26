/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/vtk/vti_image_reader.hpp"

#include "io/vtk/helper/vtk_lambda_command.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

vti_image_reader::vti_image_reader() :
    m_job(std::make_shared<core::jobs::observer>("Vti image reader"))
{
}

//------------------------------------------------------------------------------

vti_image_reader::~vti_image_reader()
= default;

//------------------------------------------------------------------------------

void vti_image_reader::read()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    data::image::sptr p_image = get_concrete_object();

    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(this->get_file().string().c_str());

    using helper::vtk_lambda_command;
    vtkSmartPointer<vtk_lambda_command> progress_callback;

    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkGenericDataObjectReader*>(_caller);
            m_job->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    m_job->add_simple_cancel_hook([&]{reader->AbortExecuteOn();});

    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();

    vtkDataObject* obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    SIGHT_THROW_IF("VtiImageReader cannot read Vti image file :" << this->get_file().string(), !img);
    try
    {
        io::vtk::from_vtk_image(img, p_image);
    }
    catch(std::exception& e)
    {
        SIGHT_THROW("VTIImage to data::image failed " << e.what());
    }
}

//------------------------------------------------------------------------------

std::string vti_image_reader::extension() const
{
    return ".vti";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr vti_image_reader::get_job() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
