/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "module/io/vtk/image_reader.hpp"

#include <core/location/single_folder.hpp>
#include <core/tools/failed.hpp>

#include <data/image.hpp>

#include <io/__/reader/object_reader.hpp>
#include <io/__/service/reader.hpp>
#include <io/vtk/bitmap_image_reader.hpp>
#include <io/vtk/image_reader.hpp>
#include <io/vtk/meta_image_reader.hpp>
#include <io/vtk/vti_image_reader.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

//------------------------------------------------------------------------------

image_reader::image_reader() noexcept :
    reader("Choose a file to load an image")
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t image_reader::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

std::vector<std::pair<std::string, std::string> > image_reader::get_supported_extensions()
{
    std::vector<std::string> extensions;
    sight::io::vtk::bitmap_image_reader::get_available_extensions(extensions);

    std::string bitmap_wildcards;
    for(const auto& extension : extensions)
    {
        if(!bitmap_wildcards.empty())
        {
            bitmap_wildcards += ' ';
        }

        bitmap_wildcards += "*" + extension;
    }

    std::vector<std::pair<std::string, std::string> > filters {
        {"Vtk", "*.vtk"},
        {"Vti", "*.vti"},
        {"MetaImage", "*.mhd"}
    };

    if(!bitmap_wildcards.empty())
    {
        filters.emplace_back("Bitmap image", std::move(bitmap_wildcards));
    }

    return filters;
}

//------------------------------------------------------------------------------

void image_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    /* Initialize the available extensions for BitmapImageReader */
    std::vector<std::string> ext;
    sight::io::vtk::bitmap_image_reader::get_available_extensions(ext);
    std::string available_extensions;

    if(!ext.empty())
    {
        available_extensions = "*" + ext.at(0);
        for(std::size_t i = 1 ; i < ext.size() ; i++)
        {
            available_extensions.append(" *").append(ext.at(i));
        }
    }

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(*m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("Vtk", "*.vtk");
    dialog_file.add_filter("Vti", "*.vti");
    dialog_file.add_filter("MetaImage", "*.mhd");
    dialog_file.add_filter("Bitmap image", available_extensions);
    dialog_file.set_option(ui::dialog::location::read);
    dialog_file.set_option(ui::dialog::location::file_must_exist);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        this->set_file(result->get_file());
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
    }
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
    m_read_failed = true;

    if(this->has_location_defined())
    {
        const auto locked = m_data.lock();
        const auto image  = std::dynamic_pointer_cast<data::image>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::image::classname()
            + "' or '"
            + sight::io::service::READER_DATA_KEY
            + "' is not correctly set.",
            image
        );

        // Read new image path and update image. If the reading process is a success, we notify all listeners that image
        // has been modified.

        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);
        auto observer = this->observe("Reading image");

        try
        {
            if(load_image(this->get_file(), image, observer))
            {
                m_read_failed = false;

                image->async_emit(this, data::signals::MODIFIED);
            }
        }
        catch(core::tools::failed& e)
        {
            cursor.set_default_cursor();
            SIGHT_THROW_EXCEPTION(e);
        }

        cursor.set_default_cursor();
    }
}

//------------------------------------------------------------------------------

template<typename READER>
static READER::sptr configure_reader(const std::filesystem::path& _img_file)
{
    typename READER::sptr reader = std::make_shared<READER>();
    reader->set_file(_img_file);
    return reader;
}

//------------------------------------------------------------------------------

bool image_reader::load_image(
    const std::filesystem::path& _vtk_file,
    std::shared_ptr<data::image> _image,
    core::notification::observer::sptr _progress
)
{
    bool ok = true;

    std::string ext = _vtk_file.extension().string();
    boost::algorithm::to_lower(ext);

    sight::io::reader::object_reader::sptr image_reader;
    if(ext == ".vtk")
    {
        image_reader = configure_reader<sight::io::vtk::image_reader>(_vtk_file);
    }
    else if(ext == ".vti")
    {
        image_reader = configure_reader<sight::io::vtk::vti_image_reader>(_vtk_file);
    }
    else if(ext == ".mhd")
    {
        image_reader = configure_reader<sight::io::vtk::meta_image_reader>(_vtk_file);
    }
    else
    {
        /* Handle BitmapImageReader extensions */
        std::vector<std::string> available_extensions;
        sight::io::vtk::bitmap_image_reader::get_available_extensions(available_extensions);

        /* If we find the current extensions in the available readers, we use it */
        std::size_t i = 0;
        for( ; i < available_extensions.size() ; i++)
        {
            if(available_extensions.at(i) == ext)
            {
                image_reader = configure_reader<sight::io::vtk::bitmap_image_reader>(_vtk_file);
                break;
            }
        }

        // If we didn't find any suitable extension with BitmapImageReader, we raise an exception */
        if(i == available_extensions.size())
        {
            i = 0;
            std::string bitmap_extensions;
            for( ; i < available_extensions.size() ; i++)
            {
                bitmap_extensions.append(available_extensions.at(i)).append(", ");
            }

            SIGHT_THROW_EXCEPTION(
                core::tools::failed(
                    "Only " + bitmap_extensions
                    + ".vtk, .vti and .mhd are supported."
                )
            );
        }
    }

    // Set the image (already created, but empty) that will be modified
    image_reader->set_object(_image);

    try
    {
        image_reader->read(_progress);
    }
    catch(core::tools::failed& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();

        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::warning
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
            sight::ui::dialog::message::warning
        );
        ok = false;
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during loading.",
            sight::ui::dialog::message::warning
        );
        ok = false;
    }

    return ok;
}

} // namespace sight::module::io::vtk
