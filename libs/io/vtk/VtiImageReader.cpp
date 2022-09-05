/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "io/vtk/VtiImageReader.hpp"

#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <io/base/reader/registry/macros.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkXMLImageDataReader.h>

SIGHT_REGISTER_IO_READER(sight::io::vtk::VtiImageReader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

VtiImageReader::VtiImageReader(io::base::reader::IObjectReader::Key /*unused*/) :
    m_job(core::jobs::Observer::New("Vti image reader"))
{
}

//------------------------------------------------------------------------------

VtiImageReader::~VtiImageReader()
= default;

//------------------------------------------------------------------------------

void VtiImageReader::read()
{
    assert(!m_object.expired());
    assert(m_object.lock());

    data::Image::sptr pImage = getConcreteObject();

    vtkSmartPointer<vtkXMLImageDataReader> reader = vtkSmartPointer<vtkXMLImageDataReader>::New();
    reader->SetFileName(this->getFile().string().c_str());

    using helper::vtkLambdaCommand;
    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkGenericDataObjectReader*>(caller);
            m_job->doneWork(static_cast<std::uint64_t>(filter->GetProgress() * 100.));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&]{reader->AbortExecuteOn();});

    reader->Update();
    reader->UpdateInformation();
    reader->PropagateUpdateExtent();

    vtkDataObject* obj = reader->GetOutput();
    vtkImageData* img  = vtkImageData::SafeDownCast(obj);

    m_job->finish();

    SIGHT_THROW_IF("VtiImageReader cannot read Vti image file :" << this->getFile().string(), !img);
    try
    {
        io::vtk::fromVTKImage(img, pImage);
    }
    catch(std::exception& e)
    {
        SIGHT_THROW("VTIImage to data::Image failed " << e.what());
    }
}

//------------------------------------------------------------------------------

std::string VtiImageReader::extension() const
{
    return ".vti";
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr VtiImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
