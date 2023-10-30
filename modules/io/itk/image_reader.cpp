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

#include "image_reader.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/image.hpp>

#include <io/__/service/reader.hpp>
#include <io/itk/InrImageReader.hpp>
#include <io/itk/NiftiImageReader.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::io::itk
{

//------------------------------------------------------------------------------

image_reader::image_reader() noexcept =
    default;

//------------------------------------------------------------------------------

image_reader::~image_reader() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::path_type_t image_reader::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

void image_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a file to load an image" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialog_file.add_filter("Inr (.inr.gz)", "*.inr.gz");
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

//
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
        const auto data  = m_data.lock();
        const auto image = std::dynamic_pointer_cast<data::image>(data.get_shared());
        SIGHT_ASSERT(
            "The object is not a '"
            + data::image::classname()
            + "' or '"
            + sight::io::service::DATA_KEY
            + "' is not correctly set.",
            image
        );

        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);

        try
        {
            if(sight::module::io::itk::image_reader::load_image(this->get_file(), image))
            {
                m_read_failed = false;
                auto sig = image->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
                {
                    core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
                    sig->async_emit();
                }
            }
        }
        catch(core::tools::failed& e)
        {
            cursor.set_default_cursor();
            SIGHT_THROW_EXCEPTION(e);
        }
        cursor.set_default_cursor();
    }
    else
    {
        m_read_failed = true;
    }
}

//------------------------------------------------------------------------------

bool image_reader::load_image(
    const std::filesystem::path& _img_file,
    const data::image::sptr& _img
)
{
    bool ok = true;

    std::string ext = _img_file.extension().string();
    boost::algorithm::to_lower(ext);

    sight::io::reader::object_reader::sptr image_reader;
    if(boost::algorithm::ends_with(_img_file.string(), ".inr.gz"))
    {
        auto inr_reader = std::make_shared<sight::io::itk::inr_image_reader>();
        inr_reader->set_file(_img_file);
        image_reader = inr_reader;
    }
    else if(ext == ".nii" || boost::algorithm::ends_with(_img_file.string(), ".nii.gz"))
    {
        auto nifti_reader = std::make_shared<sight::io::itk::nifti_image_reader>();
        nifti_reader->set_file(_img_file);
        image_reader = nifti_reader;
    }
    else
    {
        std::stringstream ss;
        ss << "The file extension " << ext
        << " is not supported by the image reader. Please choose either *.inr.gz, *.nii or *.nii.gz files";
        sight::ui::dialog::message::show(
            "Error",
            ss.str(),
            sight::ui::dialog::message::critical
        );
        return false;
    }

    image_reader->set_object(_img);

    try
    {
        image_reader->read();
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
            "Warning during loading",
            sight::ui::dialog::message::warning
        );
        ok = false;
    }
    return ok;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
