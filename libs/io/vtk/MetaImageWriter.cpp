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

#include "io/vtk/MetaImageWriter.hpp"

#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <io/__/writer/registry/macros.hpp>

#include <vtkImageData.h>
#include <vtkMetaImageWriter.h>
#include <vtkSmartPointer.h>

SIGHT_REGISTER_IO_WRITER(sight::io::vtk::MetaImageWriter);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

MetaImageWriter::MetaImageWriter() :
    m_job(std::make_shared<core::jobs::observer>("MetaImage writer"))
{
}

//------------------------------------------------------------------------------

MetaImageWriter::~MetaImageWriter()
= default;

//------------------------------------------------------------------------------

void MetaImageWriter::write()
{
    using helper::vtkLambdaCommand;

    assert(!m_object.expired());
    assert(m_object.lock());

    data::image::csptr p_image = getConcreteObject();

    vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
    vtkSmartPointer<vtkImageData> vtk_image    = vtkSmartPointer<vtkImageData>::New();
    io::vtk::to_vtk_image(p_image, vtk_image);
    writer->SetInputData(vtk_image);
    writer->SetFileName(this->get_file().string().c_str());
    writer->SetCompression(true);

    vtkSmartPointer<vtkLambdaCommand> progress_callback;
    progress_callback = vtkSmartPointer<vtkLambdaCommand>::New();
    progress_callback->SetCallback(
        [this](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkMetaImageWriter*>(_caller);
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

std::string MetaImageWriter::extension() const
{
    return ".mhd";
}

//------------------------------------------------------------------------------

core::jobs::base::sptr MetaImageWriter::getJob() const
{
    return m_job;
}

} // namespace sight::io::vtk
