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

#include "modules/io/vtk/image_reader.hpp"

#include <core/base.hpp>
#include <core/com/has_signals.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/jobs/aggregator.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/job.hpp>
#include <core/location/single_folder.hpp>

#include <data/image.hpp>

#include <io/__/reader/object_reader.hpp>
#include <io/__/service/reader.hpp>
#include <io/vtk/BitmapImageReader.hpp>
#include <io/vtk/ImageReader.hpp>
#include <io/vtk/MetaImageReader.hpp>
#include <io/vtk/VtiImageReader.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <boost/algorithm/string.hpp>

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <thread>

namespace sight::module::io::vtk
{

//------------------------------------------------------------------------------

// Register a new reader of data::image

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

sight::io::service::IOPathType image_reader::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void image_reader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::single_folder>();

    /* Initialize the available extensions for BitmapImageReader */
    std::vector<std::string> ext;
    sight::io::vtk::BitmapImageReader::getAvailableExtensions(ext);
    std::string availableExtensions;

    if(!ext.empty())
    {
        availableExtensions = "*" + ext.at(0);
        for(std::size_t i = 1 ; i < ext.size() ; i++)
        {
            availableExtensions.append(" *").append(ext.at(i));
        }
    }

    sight::ui::dialog::location dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? "Choose a file to load an image" : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.addFilter("Vtk", "*.vtk");
    dialogFile.addFilter("Vti", "*.vti");
    dialogFile.addFilter("MetaImage", "*.mhd");
    dialogFile.addFilter("Bitmap image", availableExtensions);
    dialogFile.setOption(ui::dialog::location::READ);
    dialogFile.setOption(ui::dialog::location::FILE_MUST_EXIST);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialogFile.show());
    if(result)
    {
        this->set_file(result->get_file());
        defaultDirectory->set_folder(result->get_file().parent_path());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

image_reader::image_reader() noexcept
{
    m_sigJobCreated = new_signal<JobCreatedSignalType>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void image_reader::starting()
{
}

//------------------------------------------------------------------------------

void image_reader::stopping()
{
}

//------------------------------------------------------------------------------

void image_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void image_reader::info(std::ostream& _sstream)
{
    _sstream << "image_reader::info";
}

//------------------------------------------------------------------------------

void image_reader::updating()
{
    m_readFailed = true;

    if(this->hasLocationDefined())
    {
        const auto locked = m_data.lock();
        const auto image  = std::dynamic_pointer_cast<data::image>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::image::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            image
        );

        // Read new image path and update image. If the reading process is a success, we notify all listeners that image
        // has been modified.

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);
        try
        {
            // Notify other image services that a new image has been loaded.
            if(image_reader::loadImage(this->get_file(), image, m_sigJobCreated))
            {
                m_readFailed = false;

                auto sig = image->signal<data::object::ModifiedSignalType>(data::object::MODIFIED_SIG);
                {
                    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                    sig->async_emit();
                }
            }
        }
        catch(core::tools::failed& e)
        {
            cursor.setDefaultCursor();
            SIGHT_THROW_EXCEPTION(e);
        }

        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

template<typename READER>
typename READER::sptr configureReader(const std::filesystem::path& imgFile)
{
    typename READER::sptr reader = std::make_shared<READER>();
    reader->set_file(imgFile);
    return reader;
}

//------------------------------------------------------------------------------

bool image_reader::loadImage(
    const std::filesystem::path& imgFile,
    std::shared_ptr<data::image> img,
    const SPTR(JobCreatedSignalType)& sigJobCreated
)
{
    bool ok = true;

    std::string ext = imgFile.extension().string();
    boost::algorithm::to_lower(ext);

    sight::io::reader::object_reader::sptr imageReader;
    if(ext == ".vtk")
    {
        imageReader = configureReader<sight::io::vtk::ImageReader>(imgFile);
    }
    else if(ext == ".vti")
    {
        imageReader = configureReader<sight::io::vtk::VtiImageReader>(imgFile);
    }
    else if(ext == ".mhd")
    {
        imageReader = configureReader<sight::io::vtk::MetaImageReader>(imgFile);
    }
    else
    {
        /* Handle BitmapImageReader extensions */
        std::vector<std::string> availableExtensions;
        sight::io::vtk::BitmapImageReader::getAvailableExtensions(availableExtensions);

        /* If we find the current extensions in the available readers, we use it */
        std::size_t i = 0;
        for( ; i < availableExtensions.size() ; i++)
        {
            if(availableExtensions.at(i) == ext)
            {
                imageReader = configureReader<sight::io::vtk::BitmapImageReader>(imgFile);
                break;
            }
        }

        // If we didn't find any suitable extension with BitmapImageReader, we raise an exception */
        if(i == availableExtensions.size())
        {
            i = 0;
            std::string bitmapExtensions;
            for( ; i < availableExtensions.size() ; i++)
            {
                bitmapExtensions.append(availableExtensions.at(i)).append(", ");
            }

            SIGHT_THROW_EXCEPTION(
                core::tools::failed(
                    "Only " + bitmapExtensions
                    + ".vtk, .vti and .mhd are supported."
                )
            );
        }
    }

    // Set the image (already created, but empty) that will be modified
    imageReader->set_object(img);

    sigJobCreated->emit(imageReader->getJob());

    try
    {
        imageReader->read();
    }
    catch(core::tools::failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
        ok = false;

        // Raise exception  for superior level
        SIGHT_THROW_EXCEPTION(e);
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
        ok = false;
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during loading.",
            sight::ui::dialog::message::WARNING
        );
        ok = false;
    }

    return ok;
}

} // namespace sight::module::io::vtk
