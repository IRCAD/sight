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

#include "io/vtk/MetaImageReader.hpp"

#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <io/base/reader/registry/macros.hpp>

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkSmartPointer.h>

SIGHT_REGISTER_IO_READER(::sight::io::vtk::MetaImageReader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

MetaImageReader::MetaImageReader(io::base::reader::IObjectReader::Key) :
    m_job(core::jobs::Observer::New("Meta image reader"))
{
}

//------------------------------------------------------------------------------

MetaImageReader::~MetaImageReader()
{
}

//------------------------------------------------------------------------------

void MetaImageReader::read()
{
    using namespace sight::io::vtk::helper;
    assert(!m_object.expired());
    assert(m_object.lock());

    data::Image::sptr pImage = this->getConcreteObject();

    vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
    reader->SetFileName(this->getFile().string().c_str());

    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, long unsigned int, void*)
        {
            auto filter = static_cast<vtkMetaImageReader*>(caller);
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

    SIGHT_THROW_IF("MetaImageReader cannot read mhd image file :" << this->getFile().string(), !img);
    try
    {
        io::vtk::fromVTKImage(img, pImage);
    }
    catch(std::exception& e)
    {
        SIGHT_THROW("MetaImage to data::Image failed : " << e.what());
    }
}

//------------------------------------------------------------------------------

std::string MetaImageReader::extension()
{
    return ".mhd";
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr MetaImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
