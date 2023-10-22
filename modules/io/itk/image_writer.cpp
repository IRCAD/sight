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

#include "image_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/image.hpp>

#include <io/__/service/writer.hpp>
#include <io/itk/InrImageWriter.hpp>
#include <io/itk/JpgImageWriter.hpp>
#include <io/itk/NiftiImageWriter.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <boost/algorithm/string.hpp>

namespace sight::module::io::itk
{

/**
 * Do not mark `nifti`  as incorrect.
 * cspell:ignore nifti
 */

//------------------------------------------------------------------------------

image_writer::image_writer() noexcept =
    default;

//------------------------------------------------------------------------------

image_writer::~image_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType image_writer::getIOPathType() const
{
    return sight::io::service::FILE;
}

//------------------------------------------------------------------------------

void image_writer::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose a file to save an image" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.addFilter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialog_file.addFilter("Inr (.inr.gz)", "*.inr.gz");
    dialog_file.setOption(ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        this->set_file(result->get_file());
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void image_writer::starting()
{
}

//------------------------------------------------------------------------------

void image_writer::stopping()
{
}

//------------------------------------------------------------------------------

void image_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void image_writer::info(std::ostream& _sstream)
{
    _sstream << "image_writer::info";
}

//------------------------------------------------------------------------------

bool image_writer::saveImage(const std::filesystem::path& _img_save_path, const data::image::csptr& _image)
{
    sight::io::writer::object_writer::sptr my_writer;
    std::string ext = _img_save_path.extension().string();
    boost::algorithm::to_lower(ext);

    if(boost::algorithm::ends_with(_img_save_path.string(), ".inr.gz"))
    {
        auto inr_writer = std::make_shared<sight::io::itk::InrImageWriter>();
        sight::ui::dialog::progress progress_meter_gui("Saving images... ");
        inr_writer->add_handler(progress_meter_gui);
        inr_writer->set_file(_img_save_path);
        my_writer = inr_writer;
    }
    else if(ext == ".nii" || boost::algorithm::ends_with(_img_save_path.string(), ".nii.gz"))
    {
        auto nifti_writer = std::make_shared<sight::io::itk::NiftiImageWriter>();
        nifti_writer->set_file(_img_save_path);
        my_writer = nifti_writer;
    }
    else if(std::filesystem::is_directory(_img_save_path))
    {
        auto jpg_writer = std::make_shared<sight::io::itk::JpgImageWriter>();
        sight::ui::dialog::progress progress_meter_gui("Saving images... ");
        jpg_writer->add_handler(progress_meter_gui);
        jpg_writer->set_folder(_img_save_path);
        my_writer = jpg_writer;
    }
    else
    {
        SIGHT_THROW_EXCEPTION(
            core::tools::failed(
                "Unsupported " + ext + " format (Available formats: "
                + ".inr.gz, .nii, .jpg, .jpeg)"
            )
        );
    }

    my_writer->set_object(_image);

    try
    {
        my_writer->write();
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
        return false;
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving",
            sight::ui::dialog::message::WARNING
        );
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------

void image_writer::updating()
{
    m_writeFailed = true;
    if(this->hasLocationDefined())
    {
        const auto data  = m_data.lock();
        const auto image = std::dynamic_pointer_cast<const data::image>(data.get_shared());
        SIGHT_ASSERT("The input key '" + sight::io::service::s_DATA_KEY + "' is not correctly set.", image);

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);
        try
        {
            saveImage(this->get_file(), image);
            m_writeFailed = false;
        }
        catch(core::tools::failed& e)
        {
            SIGHT_THROW_EXCEPTION(e);
        }
        cursor.setDefaultCursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
