/************************************************************************
 *
 * Copyright (C) 2017-2022 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "io/vtk/BitmapImageReader.hpp"

#include "io/vtk/helper/vtkLambdaCommand.hpp"
#include "io/vtk/vtk.hpp"

#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>

#include <io/base/reader/registry/macros.hpp>

#include <boost/tokenizer.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Collection.h>
#include <vtkImageReader2Factory.h>
#include <vtkSmartPointer.h>

SIGHT_REGISTER_IO_READER(sight::io::vtk::BitmapImageReader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

BitmapImageReader::BitmapImageReader(io::base::reader::IObjectReader::Key /*unused*/) :
    m_job(core::jobs::Observer::New("Bitmap image reader"))
{
    /* Initialize the available extensions */
    std::vector<std::string> ext;
    BitmapImageReader::getAvailableExtensions(ext);

    if(!ext.empty())
    {
        m_availableExtensions = ext.at(0);
        for(std::vector<std::string>::size_type i = 1 ; i < ext.size() ; i++)
        {
            m_availableExtensions = m_availableExtensions + " " + ext.at(i);
        }
    }
}

//------------------------------------------------------------------------------

BitmapImageReader::~BitmapImageReader()
= default;

//------------------------------------------------------------------------------

void BitmapImageReader::read()
{
    SIGHT_ASSERT("The current object has expired.", !m_object.expired());
    SIGHT_ASSERT("Unable to lock object", m_object.lock());

    data::Image::sptr pImage = getConcreteObject();

    // Use a vtkImageReader2Factory to automatically detect the type of the input file
    // And select the right reader for the file
    vtkSmartPointer<vtkImageReader2Factory> factory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkImageReader2* reader                         = factory->CreateImageReader2(this->getFile().string().c_str());

    SIGHT_THROW_IF("BitmapImageReader cannot read Bitmap image file :" << this->getFile().string(), !reader);

    reader->SetFileName(this->getFile().string().c_str());

    using helper::vtkLambdaCommand;
    vtkSmartPointer<vtkLambdaCommand> progressCallback;

    progressCallback = vtkSmartPointer<vtkLambdaCommand>::New();
    progressCallback->SetCallback(
        [&](vtkObject* caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkGenericDataObjectReader*>(caller);
            m_job->doneWork(static_cast<uint64_t>(filter->GetProgress() * 100.0));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progressCallback);

    m_job->addSimpleCancelHook([&]{reader->AbortExecuteOn();});

    reader->Update();
    vtkSmartPointer<vtkImageData> img = reader->GetOutput();
    reader->Delete();

    m_job->finish();

    SIGHT_THROW_IF("BitmapImageReader cannot read Bitmap image file :" << this->getFile().string(), !img);
    try
    {
        io::vtk::fromVTKImage(img, pImage);
    }
    catch(std::exception& e)
    {
        SIGHT_THROW("BitmapImage to data::Image failed " << e.what());
    }
}

//------------------------------------------------------------------------------

std::string BitmapImageReader::extension() const
{
    return m_availableExtensions;
}

//------------------------------------------------------------------------------

core::jobs::IJob::sptr BitmapImageReader::getJob() const
{
    return m_job;
}

//------------------------------------------------------------------------------

void BitmapImageReader::getAvailableExtensions(std::vector<std::string>& ext)
{
    /* Get the collection of available bitmap image readers */
    vtkSmartPointer<vtkImageReader2Collection> ir2c = vtkSmartPointer<vtkImageReader2Collection>::New();
    vtkImageReader2Factory::GetRegisteredReaders(ir2c);

    const boost::char_separator<char> sep(" ");

    /* Iterate over the elements of the collection */
    ir2c->InitTraversal();
    for(int i = 0 ; i < ir2c->GetNumberOfItems() ; i++)
    {
        vtkImageReader2* ir2 = ir2c->GetNextItem();

        /* Split the string returned by GetFileExtensions() (several extensions can be available for the same type) */
        const std::string s = ir2->GetFileExtensions();
        const boost::tokenizer<boost::char_separator<char> > tokens {s, sep};

        for(const auto& token : tokens)
        {
            ext.push_back(token);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
