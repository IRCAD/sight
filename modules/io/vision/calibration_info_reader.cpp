/************************************************************************
 *
 * Copyright (C) 2019-2024 IRCAD France
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

#include "calibration_info_reader.hpp"

#include <core/com/slots.hxx>
#include <core/location/single_folder.hpp>

#include <data/calibration_info.hpp>
#include <data/image.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/image.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>

#include <opencv2/opencv.hpp>

// cspell:ignore imread

namespace sight::module::io::vision
{

static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "update_chessboard_size";

//------------------------------------------------------------------------------

sight::io::service::path_type_t calibration_info_reader::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

void calibration_info_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Select a folder holding calibration inputs" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(ui::dialog::location::read);
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

void calibration_info_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//------------------------------------------------------------------------------

void calibration_info_reader::starting()
{
}

//------------------------------------------------------------------------------

void calibration_info_reader::updating()
{
    if(this->has_location_defined())
    {
        const auto data       = m_data.lock();
        const auto calib_info = std::dynamic_pointer_cast<data::calibration_info>(data.get_shared());
        SIGHT_ASSERT("Missing calibration info.", calib_info);

        data::mt::locked_ptr calib_info_lock(calib_info);

        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);

        using detection_pair_t = std::pair<data::image::sptr, data::point_list::sptr>;

        const std::filesystem::path folder = this->get_folder();

        // Use a map to sort input images by their filename.
        std::map<std::string, detection_pair_t> filename_detection_map;

        for(const std::filesystem::path& dir_entry : std::filesystem::directory_iterator(folder))
        {
            cv::Mat img = cv::imread(dir_entry.string(), cv::IMREAD_COLOR);
            std::string error_message;

            if(!img.empty())
            {
                cv::cvtColor(img, img, cv::COLOR_BGR2RGB);

                data::point_list::sptr chessboard_pts = geometry::vision::helper::detect_chessboard(
                    img,
                    std::size_t(*m_width),
                    std::size_t(*m_height),
                    float(*m_scale)
                );

                if(chessboard_pts)
                {
                    data::image::sptr calib_img = std::make_shared<data::image>();
                    sight::io::opencv::image::copy_from_cv(*calib_img.get(), img);

                    calib_img->set_spacing({{1., 1., 1.}});
                    calib_img->set_origin({{0., 0., 0.}});

                    const auto detection_pair = std::make_pair(calib_img, chessboard_pts);
                    const auto filename       = dir_entry.filename().string();
                    filename_detection_map[filename] = detection_pair;
                }
                else
                {
                    error_message = "Couldn't detect a chessboard in '" + dir_entry.string() + "'.\n\n"
                                                                                               "Please make sure that the right chessboard parameters are set.";
                }
            }
            else
            {
                error_message = "Couldn't read '" + dir_entry.string() + "'.\n\n"
                                                                         "Make sure it is a valid image format.";
            }

            if(!error_message.empty())
            {
                error_message += "\n\n Abort reading?";
                sight::ui::dialog::message message_box("Reading calibration inputs failed", error_message,
                                                       sight::ui::dialog::message::warning);

                message_box.add_button(ui::dialog::message::yes_no);

                if((message_box.show() & sight::ui::dialog::message::yes) != 0)
                {
                    filename_detection_map.clear();
                    m_read_failed = true;
                    break;
                }

                error_message.clear();
            }
        }

        cursor.set_default_cursor();

        if(!filename_detection_map.empty())
        {
            for(auto& [fname, detection] : filename_detection_map)
            {
                auto& [img, chessboard] = detection;
                calib_info->add_record(img, chessboard);
            }

            auto sig = calib_info->signal<data::calibration_info::added_record_signal_t>
                           (data::calibration_info::MODIFIED_SIG);

            sig->async_emit();
        }
    }
    else
    {
        m_read_failed = true;
    }
}

//------------------------------------------------------------------------------

void calibration_info_reader::stopping()
{
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vision
