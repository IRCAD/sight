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
#include <ui/__/preferences.hpp>

#include <opencv2/opencv.hpp>

namespace sight::module::io::vision
{

static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "updateChessboardSize";

//------------------------------------------------------------------------------

calibration_info_reader::calibration_info_reader() noexcept
{
    new_slot(UPDATE_CHESSBOARD_SIZE_SLOT, &calibration_info_reader::updateChessboardSize, this);
}

//------------------------------------------------------------------------------

calibration_info_reader::~calibration_info_reader() noexcept =
    default;

//------------------------------------------------------------------------------

sight::io::service::IOPathType calibration_info_reader::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void calibration_info_reader::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Select a folder holding calibration inputs" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setOption(ui::dialog::location::READ);
    dialog_file.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());

    if(result)
    {
        this->set_folder(result->get_folder());
        default_directory->set_folder(result->get_folder().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
    }
    else
    {
        this->clearLocations();
    }
}

//------------------------------------------------------------------------------

void calibration_info_reader::configuring()
{
    sight::io::service::reader::configuring();

    const config_t config       = this->get_config();
    const config_t board_config = config.get_child("board");

    m_widthKey = board_config.get<std::string>("<xmlattr>.width");
    SIGHT_ASSERT("Missing board width preference key.", !m_widthKey.empty());
    m_heightKey = board_config.get<std::string>("<xmlattr>.height");
    SIGHT_ASSERT("Missing board height preference key.", !m_heightKey.empty());
    m_scaleKey = board_config.get<std::string>("<xmlattr>.scale", "");
}

//------------------------------------------------------------------------------

void calibration_info_reader::starting()
{
    this->updateChessboardSize();
}

//------------------------------------------------------------------------------

void calibration_info_reader::updating()
{
    if(this->hasLocationDefined())
    {
        const auto data       = m_data.lock();
        const auto calib_info = std::dynamic_pointer_cast<data::calibration_info>(data.get_shared());
        SIGHT_ASSERT("Missing calibration info.", calib_info);

        data::mt::locked_ptr calib_info_lock(calib_info);

        sight::ui::cursor cursor;
        cursor.setCursor(ui::cursor_base::BUSY);

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
                    m_width,
                    m_height,
                    m_scale
                );

                if(chessboard_pts)
                {
                    data::image::sptr calib_img = std::make_shared<data::image>();
                    sight::io::opencv::image::copy_from_cv(*calib_img.get(), img);

                    calib_img->setSpacing({{1., 1., 1.}});
                    calib_img->setOrigin({{0., 0., 0.}});

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
                                                       sight::ui::dialog::message::WARNING);

                message_box.addButton(ui::dialog::message::YES_NO);

                if((message_box.show() & sight::ui::dialog::message::YES) != 0)
                {
                    filename_detection_map.clear();
                    m_readFailed = true;
                    break;
                }

                error_message.clear();
            }
        }

        cursor.setDefaultCursor();

        if(!filename_detection_map.empty())
        {
            for(auto& [fname, detection] : filename_detection_map)
            {
                auto& [img, chessboard] = detection;
                calib_info->addRecord(img, chessboard);
            }

            auto sig = calib_info->signal<data::calibration_info::added_record_signal_t>
                           (data::calibration_info::MODIFIED_SIG);

            sig->async_emit();
        }
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void calibration_info_reader::stopping()
{
}

// ----------------------------------------------------------------------------

void calibration_info_reader::updateChessboardSize()
{
    try
    {
        ui::preferences preferences;

        if(const auto& saved = preferences.get_optional<decltype(m_width)>(m_widthKey); saved)
        {
            m_width = *saved;
        }

        if(const auto& saved = preferences.get_optional<decltype(m_height)>(m_heightKey); saved)
        {
            m_height = *saved;
        }

        if(const auto& saved = preferences.get_optional<decltype(m_scale)>(m_scaleKey); saved)
        {
            m_scale = *saved;

            if(m_scale > 1.F)
            {
                m_scale = 1.F;
                SIGHT_ERROR("It is pointless to upscale the image for chessboard detection.");
            }
        }
    }
    catch(const ui::preferences_disabled&)
    {
        // Nothing to do..
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vision
