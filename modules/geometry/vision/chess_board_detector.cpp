/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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

#include "chess_board_detector.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>

#include <geometry/vision/helper.hpp>

#include <io/opencv/image.hpp>

#include <ui/__/preferences.hpp>

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

#include <thread>

namespace sight::module::geometry::vision
{

static const core::com::slots::key_t RECORD_POINTS_SLOT          = "record_points";
static const core::com::slots::key_t UPDATE_CHESSBOARD_SIZE_SLOT = "update_chessboard_size";

static const core::com::signals::key_t CHESSBOARD_DETECTED_SIG = "chessboard_detected";
static const core::com::signals::key_t CHESSBOARD_FOUND_SIG    = "chessboardFound";

// ----------------------------------------------------------------------------

chess_board_detector::chess_board_detector() noexcept :
    m_sig_chessboard_detected(new_signal<chessboard_detected_signal_t>(CHESSBOARD_DETECTED_SIG)),
    m_sig_chessboard_found(new_signal<chessboard_found_signal_t>(CHESSBOARD_FOUND_SIG))
{
    new_slot(RECORD_POINTS_SLOT, &chess_board_detector::record_points, this);
    new_slot(UPDATE_CHESSBOARD_SIZE_SLOT, &chess_board_detector::update_chessboard_size, this);
}

// ----------------------------------------------------------------------------

chess_board_detector::~chess_board_detector() noexcept =
    default;

// ----------------------------------------------------------------------------

void chess_board_detector::configuring()
{
    SIGHT_ASSERT(
        "This service must have the same number of 'image' keys and 'calInfo' keys",
        m_image.size() == m_cal_info.size()
    );

    const config_t config       = this->get_config();
    const config_t board_config = config.get_child("board");

    m_width_key = board_config.get<std::string>("<xmlattr>.width");
    SIGHT_ASSERT("Missing board width preference key.", !m_width_key.empty());
    m_height_key = board_config.get<std::string>("<xmlattr>.height");
    SIGHT_ASSERT("Missing board height preference key.", !m_height_key.empty());
    m_scale_key = board_config.get<std::string>("<xmlattr>.scale");
}

// ----------------------------------------------------------------------------

void chess_board_detector::starting()
{
    this->update_chessboard_size();

    const std::size_t image_group_size = m_image.size();

    m_images.resize(image_group_size);
    m_point_lists.resize(image_group_size);
}

// ----------------------------------------------------------------------------

void chess_board_detector::updating()
{
    const std::size_t image_group_size = m_image.size();

    // Run parallel detections in separate threads.
    std::vector<std::thread> detection_jobs;
    for(std::size_t i = 1 ; i < image_group_size ; ++i)
    {
        detection_jobs.emplace_back(&chess_board_detector::do_detection, this, i);
    }

    // Detection in the first image is done on the service's worker.
    this->do_detection(0);

    for(auto& detection_job : detection_jobs)
    {
        detection_job.join();
    }

    const bool all_detected = (std::count(m_images.begin(), m_images.end(), nullptr) == 0);

    m_sig_chessboard_detected->async_emit(all_detected);

    if(all_detected)
    {
        m_sig_chessboard_found->async_emit();
    }
}

// ----------------------------------------------------------------------------

void chess_board_detector::stopping()
{
    m_images.clear();
    m_point_lists.clear();
}

// ----------------------------------------------------------------------------

service::connections_t chess_board_detector::auto_connections() const
{
    connections_t connections;
    connections.push(IMAGE_INPUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_INPUT, data::image::MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

// ----------------------------------------------------------------------------

void chess_board_detector::record_points()
{
    const std::size_t calib_group_size = m_cal_info.size();

    const bool all_detected = (std::count(m_images.begin(), m_images.end(), nullptr) == 0);

    if(all_detected)
    {
        for(std::size_t i = 0 ; i < calib_group_size ; ++i)
        {
            auto cal_info = m_cal_info[i].lock();
            SIGHT_ASSERT("Missing 'calibInfo' in-out.", cal_info);

            if(m_point_lists[i])
            {
                cal_info->add_record(m_images[i], m_point_lists[i]);

                // Notify
                auto sig = cal_info->signal<data::calibration_info::added_record_signal_t>(
                    data::calibration_info::ADDED_RECORD_SIG
                );

                sig->async_emit();
            }
            else
            {
                cal_info->add_record(m_images[i], std::make_shared<data::point_list>());
            }
        }
    }
}

// ----------------------------------------------------------------------------

void chess_board_detector::update_chessboard_size()
{
    try
    {
        ui::preferences preferences;
        m_width  = preferences.get(m_width_key, m_width);
        m_height = preferences.get(m_height_key, m_height);
        m_scale  = preferences.get(m_scale_key, m_scale);

        if(m_scale > 1.F)
        {
            m_scale = 1.F;
            SIGHT_ERROR("It is pointless to upscale the image for chessboard detection.");
        }
    }
    catch(const ui::preferences_disabled&)
    {
        // Nothing to do..
    }
}

// ----------------------------------------------------------------------------

void chess_board_detector::do_detection(std::size_t _image_index)
{
    const auto img = m_image[_image_index].lock();
    SIGHT_ASSERT("Missing 'image' input.", img);

    const bool is_valid = data::helper::medical_image::check_image_validity(img.get_shared());

    if(is_valid)
    {
        const cv::Mat cv_img = io::opencv::image::move_to_cv(img.get_shared());

        m_point_lists[_image_index] =
            sight::geometry::vision::helper::detect_chessboard(cv_img, m_width, m_height, m_scale);

        if(m_point_lists[_image_index] != nullptr)
        {
            m_images[_image_index] = std::make_shared<data::image>();
            m_images[_image_index]->deep_copy(img.get_shared());
        }
        else
        {
            m_images[_image_index] = nullptr;
        }

        const bool output_detection = (m_detection.size() == m_image.size());
        if(output_detection)
        {
            auto out_pl = m_detection[_image_index].lock();

            if(m_point_lists[_image_index] != nullptr)
            {
                out_pl->deep_copy(m_point_lists[_image_index]);
            }
            else
            {
                out_pl->get_points().clear();
            }

            auto sig = out_pl->signal<data::point_list::modified_signal_t>(data::point_list::MODIFIED_SIG);
            sig->async_emit();
        }
    }
}

// ----------------------------------------------------------------------------

} //namespace sight::module::geometry::vision
