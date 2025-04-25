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

#include "io/vtk/vti_image_writer.hpp"

#include "io/vtk/helper/vtk_lambda_command.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkZLibDataCompressor.h>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

vti_image_writer::vti_image_writer() :
    m_job(std::make_shared<core::jobs::observer>("VTK image Writer"))
{
}

//------------------------------------------------------------------------------

vti_image_writer::~vti_image_writer()
= default;

//------------------------------------------------------------------------------

void vti_image_writer::write()
{
    using helper::vtk_lambda_command;

    assert(!m_object.expired());
    assert(m_object.lock());

    data::image::csptr p_image = get_concrete_object();

    vtkSmartPointer<vtkXMLImageDataWriter> writer = vtkSmartPointer<vtkXMLImageDataWriter>::New();
    vtkSmartPointer<vtkImageData> vtk_image       = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(p_image, vtk_image);
    writer->SetInputData(vtk_image);
    writer->SetFileName(this->get_file().string().c_str());
    writer->SetDataModeToAppended();

    vtkSmartPointer<vtkZLibDataCompressor> compressor = vtkSmartPointer<vtkZLibDataCompressor>::New();
    compressor->SetCompressionLevel(1);
    writer->SetCompressor(compressor);
    writer->EncodeAppendedDataOff();

    vtkSmartPointer<vtk_lambda_command> progress_callback;
    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [this](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkXMLImageDataWriter*>(_caller);
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

std::string vti_image_writer::extension() const
{
    return ".vti";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr vti_image_writer::get_job() const
{
    return m_job;
}

} // namespace sight::io::vtk
