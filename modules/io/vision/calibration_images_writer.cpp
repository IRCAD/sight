/************************************************************************
 *
 * Copyright (C) 2019-2023 IRCAD France
 * Copyright (C) 2019-2020 IHU Strasbourg
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

#include "calibration_images_writer.hpp"

#include <core/location/single_folder.hpp>

#include <data/calibration_info.hpp>
#include <data/image.hpp>

#include <io/opencv/image.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <opencv2/opencv.hpp>

#include <filesystem>
#include <sstream>

namespace sight::module::io::vision
{

//------------------------------------------------------------------------------

calibration_images_writer::calibration_images_writer() noexcept =
    default;

//------------------------------------------------------------------------------

calibration_images_writer::~calibration_images_writer() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::path_type_t calibration_images_writer::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

void calibration_images_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a folder to save the images" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(ui::dialog::location::write);
    dialog_file.set_type(ui::dialog::location::folder);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());

    if(result)
    {
        this->set_folder(result->get_folder());
        default_directory->set_folder(result->get_folder().parent_path());
        dialog_file.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void calibration_images_writer::configuring()
{
    sight::io::service::writer::configuring();

    const auto config_tree = this->get_config();
    m_file_extension = config_tree.get("format", ".tiff");
}

//------------------------------------------------------------------------------

void calibration_images_writer::starting()
{
}

//------------------------------------------------------------------------------

void calibration_images_writer::updating()
{
    if(!m_file_extension.empty() && this->has_location_defined())
    {
        const auto data       = m_data.lock();
        const auto calib_info = std::dynamic_pointer_cast<const data::calibration_info>(data.get_shared());
        SIGHT_ASSERT("Missing calibration info input.", calib_info);

        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);

        std::size_t count(0);
        for(const auto& calib_img : calib_info->get_image_container())
        {
            std::ostringstream image_number;
            image_number << std::setw(4) << std::setfill('0') << count++;

            const std::string filename       = "img_" + image_number.str() + m_file_extension;
            const std::filesystem::path path = this->get_folder() / filename;

            cv::Mat cv_img = sight::io::opencv::image::copy_to_cv(calib_img);

            if(cv_img.dims == 3)
            {
                // Ensure that we have a true depth-less 2D image.
                cv_img = cv_img.reshape(0, 2, cv_img.size + 1);
            }

            try
            {
                if(cv_img.type() == CV_8UC3 || cv_img.type() == CV_8UC4)
                {
                    // convert the image from BGR to RGB
                    const auto col_conv_type = cv_img.type() == CV_8UC3 ? cv::COLOR_BGR2RGB : cv::COLOR_BGRA2RGBA;
                    cv::cvtColor(cv_img, cv_img, col_conv_type);
                }

                cv::imwrite(path.string(), cv_img);
            }
            catch(const cv::Exception& e)
            {
                m_write_failed = true;
                sight::ui::dialog::message::show(
                    "Error writing calibration images.",
                    e.what(),
                    sight::ui::dialog::message::critical
                );
            }
        }

        cursor.set_default_cursor();
    }
    else
    {
        m_write_failed = true;
    }
}

//------------------------------------------------------------------------------

void calibration_images_writer::stopping()
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vision
