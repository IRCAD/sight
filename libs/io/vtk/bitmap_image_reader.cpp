/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
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

#include "io/vtk/bitmap_image_reader.hpp"

#include "io/vtk/helper/vtk_lambda_command.hpp"
#include "io/vtk/vtk.hpp"

#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>

#include <io/__/reader/registry/macros.hpp>

#include <boost/tokenizer.hpp>

#include <vtkGenericDataObjectReader.h>
#include <vtkImageData.h>
#include <vtkImageReader2.h>
#include <vtkImageReader2Collection.h>
#include <vtkImageReader2Factory.h>
#include <vtkSmartPointer.h>

SIGHT_REGISTER_IO_READER(sight::io::vtk::bitmap_image_reader);

namespace sight::io::vtk
{

//------------------------------------------------------------------------------

bitmap_image_reader::bitmap_image_reader() :
    m_job(std::make_shared<core::jobs::observer>("Bitmap image reader"))
{
    /* Initialize the available extensions */
    std::vector<std::string> ext;
    bitmap_image_reader::get_available_extensions(ext);

    if(!ext.empty())
    {
        m_available_extensions = ext.at(0);
        for(std::vector<std::string>::size_type i = 1 ; i < ext.size() ; i++)
        {
            m_available_extensions = m_available_extensions + " " + ext.at(i);
        }
    }
}

//------------------------------------------------------------------------------

bitmap_image_reader::~bitmap_image_reader()
= default;

//------------------------------------------------------------------------------

void bitmap_image_reader::read()
{
    SIGHT_ASSERT("The current object has expired.", !m_object.expired());
    SIGHT_ASSERT("Unable to lock object", m_object.lock());

    data::image::sptr p_image = get_concrete_object();

    // Use a vtkImageReader2Factory to automatically detect the type of the input file
    // And select the right reader for the file
    vtkSmartPointer<vtkImageReader2Factory> factory = vtkSmartPointer<vtkImageReader2Factory>::New();
    vtkImageReader2* reader                         = factory->CreateImageReader2(this->get_file().string().c_str());

    SIGHT_THROW_IF("BitmapImageReader cannot read Bitmap image file :" << this->get_file().string(), !reader);

    reader->SetFileName(this->get_file().string().c_str());

    using helper::vtk_lambda_command;
    vtkSmartPointer<vtk_lambda_command> progress_callback;

    progress_callback = vtkSmartPointer<vtk_lambda_command>::New();
    progress_callback->set_callback(
        [&](vtkObject* _caller, std::uint64_t, void*)
        {
            auto* filter = static_cast<vtkGenericDataObjectReader*>(_caller);
            m_job->done_work(static_cast<uint64_t>(filter->GetProgress() * 100.0));
        });
    reader->AddObserver(vtkCommand::ProgressEvent, progress_callback);

    m_job->add_simple_cancel_hook([&]{reader->AbortExecuteOn();});

    reader->Update();
    vtkSmartPointer<vtkImageData> img = reader->GetOutput();
    reader->Delete();

    m_job->finish();

    SIGHT_THROW_IF("BitmapImageReader cannot read Bitmap image file :" << this->get_file().string(), !img);
    try
    {
        io::vtk::from_vtk_image(img, p_image);
    }
    catch(std::exception& e)
    {
        SIGHT_THROW("BitmapImage to data::image failed " << e.what());
    }
}

//------------------------------------------------------------------------------

std::string bitmap_image_reader::extension() const
{
    return m_available_extensions;
}

//------------------------------------------------------------------------------

core::jobs::base::sptr bitmap_image_reader::get_job() const
{
    return m_job;
}

//------------------------------------------------------------------------------

void bitmap_image_reader::get_available_extensions(std::vector<std::string>& _ext)
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
            _ext.push_back(token);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::io::vtk
