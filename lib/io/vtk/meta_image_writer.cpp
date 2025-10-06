/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

#include "io/vtk/meta_image_writer.hpp"

#include "io/vtk/helper/vtk_lambda_command.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/progress/monitor.hpp>
#include <core/progress/observer.hpp>

#include <vtkImageData.h>
#include <vtkMetaImageWriter.h>
#include <vtkSmartPointer.h>

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

void meta_image_writer::write(sight::core::progress::observer::sptr _progress)
{
    using helper::vtk_lambda_command;

    assert(!m_object.expired());
    assert(m_object.lock());

    data::image::csptr p_image = get_concrete_object();

    vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
    vtkSmartPointer<vtkImageData> vtk_image    = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(p_image, vtk_image);
    writer->SetInputData(vtk_image);
    writer->SetFileName(this->get_file().string().c_str());
    writer->SetCompression(true);

    vtkSmartPointer<vtk_lambda_command> progress_callback;
    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&_progress](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkMetaImageWriter*>(_caller);
            _progress->done_work(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });

    writer->AddObserver(vtkCommand::ProgressEvent, progress_callback);
    _progress->add_cancel_hook(
        [&]()
        {
            writer->AbortExecuteOn();
        });
    writer->Write();
}

//------------------------------------------------------------------------------

std::string meta_image_writer::extension() const
{
    return ".mhd";
}

} // namespace sight::io::vtk
