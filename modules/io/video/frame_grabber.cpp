/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2020 IHU Strasbourg
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

#include "frame_grabber.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/camera.hpp>
#include <data/frame_tl.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/preferences.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <chrono>
#include <cmath>
#include <filesystem>
#include <regex>

namespace sight::module::io::video
{

// -----------------------------------------------------------------------------

frame_grabber::frame_grabber() noexcept
{
    new_slot(SET_STEP_SLOT, &frame_grabber::set_step, this);
}

// -----------------------------------------------------------------------------

frame_grabber::~frame_grabber() noexcept =
    default;

// -----------------------------------------------------------------------------

void frame_grabber::starting()
{
    m_worker = core::thread::worker::make();
}

// -----------------------------------------------------------------------------

void frame_grabber::stopping()
{
    this->stop_camera();

    m_worker->stop();
    m_worker.reset();
}

// -----------------------------------------------------------------------------

void frame_grabber::configuring()
{
    service::config_t config = this->get_config();

    m_fps = config.get<unsigned int>("fps", 30);

    m_one_shot = config.get<bool>("oneShot", false);

    m_create_new_ts = config.get<bool>("createTimestamp", false);

    m_use_timelapse = config.get<bool>("useTimelapse", true);

    m_default_duration = config.get<double>("defaultDuration", m_default_duration);

    SIGHT_FATAL_IF("Fps setting is set to " << m_fps << " but should be in ]0;60].", m_fps == 0 || m_fps > 60);

    m_step = config.get<std::uint64_t>("step", m_step);
    SIGHT_ASSERT("Step value is set to " << m_step << " but should be > 0.", m_step > 0);
    m_step_changed = m_step;
}

// -----------------------------------------------------------------------------

void frame_grabber::updating()
{
}

// -----------------------------------------------------------------------------

void frame_grabber::start_camera()
{
    // Make sure the video is not paused
    if(m_timer && m_is_paused)
    {
        m_is_paused = false;
        m_timer->start();
    }

    if(this->started())
    {
        // Do not reset if we are already started
        return;
    }

    const auto camera = m_camera.lock();

    if(camera->get_camera_source() == data::camera::file)
    {
        std::filesystem::path file = camera->get_video_file();

        // For compatibility with old calibration with absolute path
        if(!file.is_absolute())
        {
            const std::filesystem::path video_dir(ui::preferences().get("VIDEO_DIR_PREF", std::string()));
            file = video_dir / file;
        }

        file = file.lexically_normal();

        m_is_paused = false;

        const std::filesystem::path ext = file.extension();

        if(ext.string() == ".png" || ext.string() == ".jpg" || ext.string() == ".tiff" || ext.string() == ".bmp")
        {
            this->read_images(file.parent_path(), ext.string());
        }
        else
        {
            this->read_video(file);
        }
    }
    else if(camera->get_camera_source() == data::camera::device)
    {
        this->read_device(*camera);
    }
    else if(camera->get_camera_source() == data::camera::stream)
    {
        this->read_stream(*camera);
    }
    else
    {
        this->set_start_state(false);
        sight::ui::dialog::message::show(
            "Grabber",
            "This video source is not managed by this grabber."
        );
    }
}

// -----------------------------------------------------------------------------

void frame_grabber::pause_camera()
{
    m_is_paused = !m_is_paused;
    if(m_timer)
    {
        m_is_paused ? m_timer->stop() : m_timer->start();
    }
}

// -----------------------------------------------------------------------------

void frame_grabber::stop_camera()
{
    core::mt::scoped_lock lock(m_mutex);

    if(m_timer)
    {
        if(m_timer->is_running())
        {
            m_timer->stop();
        }

        m_timer.reset();
    }

    if(m_video_capture.isOpened())
    {
        m_video_capture.release();
    }

    m_image_to_read.clear();
    m_image_timestamps.clear();
    m_image_count = 0;

    if(m_is_initialized)
    {
        // Clear the timeline: send a black frame
        const auto sig_position = this->signal<position_modified_signal_t>(POSITION_MODIFIED_SIG);
        sig_position->async_emit(static_cast<std::int64_t>(-1));

        const auto sig_duration = this->signal<duration_modified_signal_t>(DURATION_MODIFIED_SIG);
        sig_duration->async_emit(static_cast<std::int64_t>(-1));

        const auto frame_tl = m_frame.lock();
        sight::module::io::video::frame_grabber::clear_timeline(*frame_tl);

        const auto sig = this->signal<grabber::camera_stopped_signal_t>(grabber::CAMERA_STOPPED_SIG);
        sig->async_emit();

        this->set_start_state(false);
    }

    m_is_initialized = false;
}

// -----------------------------------------------------------------------------

void frame_grabber::read_video(const std::filesystem::path& _file)
{
    core::mt::scoped_lock lock(m_mutex);

    m_video_capture.open(_file.string());

    if(m_video_capture.isOpened())
    {
        m_timer = m_worker->create_timer();

        const auto fps = static_cast<std::size_t>(m_video_capture.get(cv::CAP_PROP_FPS));
        m_video_frames_nb = static_cast<std::size_t>(m_video_capture.get(cv::CAP_PROP_FRAME_COUNT));

        if(fps == 0)
        {
            sight::ui::dialog::message::show(
                "Video error",
                "Cannot read FPS from video file. Please check the video format.",
                sight::ui::dialog::message::critical
            );
            return;
        }

        const auto sig_duration = this->signal<duration_modified_signal_t>(DURATION_MODIFIED_SIG);
        sig_duration->async_emit(static_cast<std::int64_t>((m_video_frames_nb / fps) * 1000));

        const auto sig_position = this->signal<position_modified_signal_t>(POSITION_MODIFIED_SIG);
        sig_position->async_emit(0);

        core::thread::timer::time_duration_t duration = std::chrono::milliseconds(1000 / fps);

        m_timer->set_function([this](auto&& ...){grab_video();});
        m_timer->set_duration(duration);
        m_timer->start();

        this->set_start_state(true);
        auto sig = this->signal<grabber::camera_started_signal_t>(grabber::CAMERA_STARTED_SIG);
        sig->async_emit();
    }
    else
    {
        sight::ui::dialog::message::show(
            "Grabber",
            "This file cannot be opened: " + _file.string() + "."
        );

        this->set_start_state(false);
    }
}

// -----------------------------------------------------------------------------

void frame_grabber::read_device(const data::camera& _camera)
{
    core::mt::scoped_lock lock(m_mutex);

    const std::string device = _camera.get_camera_id();
    const int index          = _camera.get_index();

#ifdef __linux__
    // On linux the V4L backend can read from device id (/dev/video...)
    m_video_capture.open(device);
    if(!m_video_capture.isOpened())
    {
        //try with index
        if(index >= 0)
        {
            m_video_capture.open(index);
        }
        else
        {
            sight::ui::dialog::message::show(
                "Grabber",
                "This device cannot be opened: " + device + " at index: " + std::to_string(index)
            );
        }
    }

    // Only way to capture 1080p with v4l.
    m_video_capture.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
#else
    //On other platforms (at least on MacOS, we should use the index given by Qt)
    if(index >= 0)
    {
        m_video_capture.open(index);
    }
    else
    {
        sight::ui::dialog::message::show(
            "Grabber",
            "This device cannot be opened: " + device + " at index: " + std::to_string(index)
        );
    }
#endif

    if(m_video_capture.isOpened())
    {
        m_timer = m_worker->create_timer();
        float fps = _camera.get_maximum_frame_rate();
        fps = fps <= 0.F ? 30.F : fps;
        const std::size_t height = _camera.get_height();
        const std::size_t width  = _camera.get_width();

        m_video_capture.set(cv::CAP_PROP_FPS, static_cast<int>(fps));
        m_video_capture.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(width));
        m_video_capture.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(height));

        core::thread::timer::time_duration_t duration =
            std::chrono::milliseconds(1000 / static_cast<std::size_t>(fps));

        m_timer->set_function([this](auto&& ...){grab_video();});
        m_timer->set_duration(duration);
        m_timer->start();

        this->set_start_state(true);
        auto sig = this->signal<grabber::camera_started_signal_t>(grabber::CAMERA_STARTED_SIG);
        sig->async_emit();
    }
    else
    {
        sight::ui::dialog::message::show(
            "Grabber",
            "This device:" + device + " at index: " + std::to_string(index) + "cannot be openned."
        );

        this->set_start_state(false);
    }
}

// -----------------------------------------------------------------------------

void frame_grabber::read_stream(const data::camera& _camera)
{
    core::mt::scoped_lock lock(m_mutex);

    m_video_capture.open(_camera.get_stream_url());

    if(m_video_capture.isOpened())
    {
        m_timer = m_worker->create_timer();
        float fps = _camera.get_maximum_frame_rate();
        fps = fps <= 0.F ? 30.F : fps;
        const std::size_t height = _camera.get_height();
        const std::size_t width  = _camera.get_width();

        m_video_capture.set(cv::CAP_PROP_FPS, static_cast<int>(fps));
        m_video_capture.set(cv::CAP_PROP_FRAME_WIDTH, static_cast<double>(width));
        m_video_capture.set(cv::CAP_PROP_FRAME_HEIGHT, static_cast<double>(height));

        const core::thread::timer::time_duration_t duration =
            std::chrono::milliseconds(1000 / static_cast<std::size_t>(fps));

        m_timer->set_function([this](auto&& ...){grab_video();});
        m_timer->set_duration(duration);
        m_timer->start();

        this->set_start_state(true);
    }
    else
    {
        sight::ui::dialog::message::show(
            "Grabber",
            "This stream:" + _camera.get_stream_url() + " cannot be opened."
        );

        this->set_start_state(false);
    }
}

// -----------------------------------------------------------------------------

void frame_grabber::read_images(const std::filesystem::path& _folder, const std::string& _extension)
{
    core::mt::scoped_lock lock(m_mutex);

    std::filesystem::directory_iterator current_entry(_folder);
    std::filesystem::directory_iterator end_entry;
    for( ; current_entry != end_entry ; ++current_entry)
    {
        std::filesystem::path entry_path = *current_entry;

        if(entry_path.extension() == _extension)
        {
            m_image_to_read.push_back(entry_path);
        }
    }

    // Sort in alphabetical order (ex: img_001, img_002...)
    std::sort(m_image_to_read.begin(), m_image_to_read.end());

    if(!m_image_to_read.empty())
    {
        // Find the timestamps of all the images
        double stub_timestamp = 0.;
        for(const std::filesystem::path& image_path : m_image_to_read)
        {
            const std::string image_name = image_path.filename().string();
            static const std::regex s_TIMESTAMP("[^0-9]*([0-9]{5,})[^0-9]*");
            std::smatch match;
            if(std::regex_match(image_name, match, s_TIMESTAMP))
            {
                const std::string timestamp_str = match[1].str();
                m_image_timestamps.push_back(std::stod(timestamp_str));
            }
            else
            {
                SIGHT_WARN(
                    "Could not find a timestamp in file name: " + image_name
                    + ". Generating a timestamp duration of: " + std::to_string(m_default_duration)
                    + "ms."
                );

                m_image_timestamps.push_back(stub_timestamp);

                stub_timestamp += m_default_duration;
            }
        }

        const std::string file = m_image_to_read.front().string();
        const cv::Mat image    = cv::imread(file, cv::IMREAD_UNCHANGED);

        const int width  = image.size().width;
        const int height = image.size().height;
        const int type   = image.type();

        if(width > 0 && height > 0)
        {
            const auto w = static_cast<std::size_t>(width);
            const auto h = static_cast<std::size_t>(height);

            auto frame_tl = m_frame.lock();

            switch(type)
            {
                case CV_8UC1:
                    frame_tl->init_pool_size(
                        w,
                        h,
                        core::type::UINT8,
                        sight::data::frame_tl::pixel_format::gray_scale
                    );
                    break;

                case CV_8UC3:
                    frame_tl->init_pool_size(
                        w,
                        h,
                        core::type::UINT8,
                        sight::data::frame_tl::pixel_format::rgb
                    );
                    break;

                case CV_8UC4:
                    frame_tl->init_pool_size(
                        w,
                        h,
                        core::type::UINT8,
                        sight::data::frame_tl::pixel_format::rgba
                    );
                    break;

                case CV_16UC1:
                    frame_tl->init_pool_size(
                        w,
                        h,
                        core::type::UINT16,
                        sight::data::frame_tl::pixel_format::gray_scale
                    );
                    break;

                default:
                    sight::ui::dialog::message::show(
                        "Grabber",
                        "This file cannot be opened: " + file + "."
                    );
                    return;
            }
        }

        m_is_initialized = true;
        this->set_start_state(true);

        const auto sig_duration = this->signal<duration_modified_signal_t>(DURATION_MODIFIED_SIG);

        std::int64_t video_duration = 0;
        if(!m_use_timelapse)
        {
            video_duration = static_cast<std::int64_t>(m_image_to_read.size() * m_fps);
        }
        else
        {
            video_duration = static_cast<std::int64_t>(m_image_timestamps.back())
                             - static_cast<std::int64_t>(m_image_timestamps.front());
        }

        sig_duration->async_emit(video_duration);

        const auto sig_position = this->signal<position_modified_signal_t>(POSITION_MODIFIED_SIG);
        sig_position->async_emit(0);

        if(m_one_shot)
        {
            m_timer = m_worker->create_timer();
            m_timer->set_one_shot(true);
            m_timer->set_function([this](auto&& ...){grab_image();});
            m_timer->set_duration(std::chrono::milliseconds(0));
            m_timer->start();
        }
        else
        {
            m_timer = m_worker->create_timer();

            core::thread::timer::time_duration_t duration;
            if(!m_use_timelapse)
            {
                duration = std::chrono::milliseconds(1000 / m_fps);
            }
            else if(m_image_to_read.size() >= 2)
            {
                duration = std::chrono::milliseconds(
                    static_cast<std::int64_t>(m_image_timestamps[1] - m_image_timestamps[0])
                );
                m_timer->set_one_shot(true);
            }
            else
            {
                SIGHT_ERROR("Only one image to read, set 'oneShot' mode to true.");
                return;
            }

            m_timer->set_function([this](auto&& ...){grab_image();});
            m_timer->set_duration(duration);
            m_timer->start();
        }

        auto sig = this->signal<grabber::camera_started_signal_t>(grabber::CAMERA_STARTED_SIG);
        sig->async_emit();
    }
}

// -----------------------------------------------------------------------------

void frame_grabber::grab_video()
{
    core::mt::scoped_lock lock(m_mutex);

    if(m_video_capture.isOpened())
    {
        const double timestamp = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>
                                                         (std::chrono::system_clock::now().time_since_epoch()).count());

        const bool is_grabbed = m_video_capture.grab();

        if(is_grabbed)
        {
            auto frame_tl = m_frame.lock();

            cv::Mat image;
            m_video_capture.retrieve(image);

            this->update_zoom(image);

            if(!m_is_initialized)
            {
                const auto width  = static_cast<std::size_t>(m_video_capture.get(cv::CAP_PROP_FRAME_WIDTH));
                const auto height = static_cast<std::size_t>(m_video_capture.get(cv::CAP_PROP_FRAME_HEIGHT));

                const std::size_t w = static_cast<std::size_t>(image.size().width);
                const std::size_t h = static_cast<std::size_t>(image.size().height);

                if(width != w || height != h)
                {
                    SIGHT_ERROR(
                        "This video cannot be read, the frame size is not expected. expected: "
                        << width << " x " << height << ", actual: " << w << " x " << h
                    );
                    return;
                }

                switch(image.type())
                {
                    case CV_8UC1:
                        frame_tl->init_pool_size(
                            width,
                            height,
                            core::type::UINT8,
                            sight::data::frame_tl::pixel_format::gray_scale
                        );
                        break;

                    case CV_8UC3:
                        frame_tl->init_pool_size(
                            width,
                            height,
                            core::type::UINT8,
                            sight::data::frame_tl::pixel_format::rgb
                        );
                        break;

                    case CV_8UC4:
                        frame_tl->init_pool_size(
                            width,
                            height,
                            core::type::UINT8,
                            sight::data::frame_tl::pixel_format::rgba
                        );
                        break;

                    case CV_16UC1:
                        frame_tl->init_pool_size(
                            width,
                            height,
                            core::type::UINT16,
                            sight::data::frame_tl::pixel_format::gray_scale
                        );
                        break;

                    default:
                        sight::ui::dialog::message::show(
                            "Grabber",
                            "This video cannot be read, the video type is not managed."
                        );
                        return;
                }

                m_is_initialized = true;
            }

            // Get time slider position.
            const auto ms           = static_cast<std::size_t>(m_video_capture.get(cv::CAP_PROP_POS_MSEC));
            const auto sig_position = this->signal<position_modified_signal_t>(POSITION_MODIFIED_SIG);
            sig_position->async_emit(static_cast<std::int64_t>(ms));

            // Get the buffer of the timeline to fill
            SPTR(data::frame_tl::buffer_t) buffer_out = frame_tl->create_buffer(timestamp);
            std::uint8_t* frame_buff_out = buffer_out->add_element(0);

            // Create an OpenCV mat that aliases the buffer created from the output timeline.
            cv::Mat img_out(image.size(), image.type(), (void*) frame_buff_out, cv::Mat::AUTO_STEP);

            if(image.type() == CV_8UC3)
            {
                // Convert the read image from BGR to RGB.
                cv::cvtColor(image, img_out, cv::COLOR_BGR2RGB);
            }
            else if(image.type() == CV_8UC4)
            {
                // Convert the read image from BGRA to RGBA.
                cv::cvtColor(image, img_out, cv::COLOR_BGRA2RGBA);
            }
            else
            {
                image.copyTo(img_out);
            }

            frame_tl->push_object(buffer_out);

            const auto sig =
                frame_tl->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
            sig->async_emit(timestamp);
        }

        if(m_loop_video)
        {
            // Loop the video.
            const auto current_f = static_cast<std::size_t>(m_video_capture.get(cv::CAP_PROP_POS_FRAMES));

            if(current_f == m_video_frames_nb)
            {
                m_video_capture.set(cv::CAP_PROP_POS_MSEC, 0.);
            }
        }
    }
}

// -----------------------------------------------------------------------------

void frame_grabber::grab_image()
{
    const double t0 = core::hires_clock::get_time_in_milli_sec();

    core::mt::scoped_lock lock(m_mutex);

    // When using time lapse, the timer is set to "one shot": it is stopped when this method is called and re-started
    // at the end of it. So we need to add a boolean to check if the grabber is paused when the method is called.
    if(!m_is_paused && m_image_count < m_image_to_read.size())
    {
        const auto frame_tl = m_frame.lock();

        const std::filesystem::path image_path = m_image_to_read[m_image_count];

        const cv::Mat image               = cv::imread(image_path.string(), cv::IMREAD_UNCHANGED);
        core::hires_clock::type timestamp = NAN;

        //create a new timestamp
        if(m_create_new_ts)
        {
            timestamp = core::hires_clock::get_time_in_milli_sec();
        }
        //use the image timestamp
        else
        {
            timestamp = m_image_timestamps[m_image_count];
        }

        this->update_zoom(image);

        SIGHT_DEBUG("Reading image index " << m_image_count << " with timestamp " << timestamp);

        const std::size_t width  = static_cast<std::size_t>(image.size().width);
        const std::size_t height = static_cast<std::size_t>(image.size().height);

        if(width == frame_tl->get_width() && height == frame_tl->get_height())
        {
            const auto sig_position = this->signal<position_modified_signal_t>(POSITION_MODIFIED_SIG);
            sig_position->async_emit(static_cast<std::int64_t>(m_image_count) * 30);

            // Get the buffer of the timeline to fill
            SPTR(data::frame_tl::buffer_t) buffer_out = frame_tl->create_buffer(timestamp);
            std::uint8_t* frame_buff_out = buffer_out->add_element(0);

            // Create an openCV mat that aliases the buffer created from the output timeline
            cv::Mat img_out(image.size(), image.type(), (void*) frame_buff_out, cv::Mat::AUTO_STEP);

            if(image.type() == CV_8UC3)
            {
                // convert the readded image from BGR to RGB
                cv::cvtColor(image, img_out, cv::COLOR_BGR2RGB);
            }
            else if(image.type() == CV_8UC4)
            {
                // convert the readded image from BGRA to RGBA
                cv::cvtColor(image, img_out, cv::COLOR_BGRA2RGBA);
            }
            else
            {
                image.copyTo(img_out);
            }

            frame_tl->push_object(buffer_out);

            const auto sig =
                frame_tl->signal<data::timeline::signals::pushed_t>(data::timeline::signals::PUSHED);
            sig->async_emit(timestamp);

            const double t1           = core::hires_clock::get_time_in_milli_sec();
            const double elapsed_time = t1 - t0;

            if(m_use_timelapse)
            {
                double next_duration = 0.;

                const std::size_t current_image = m_image_count;
                const double current_time       = m_image_timestamps[current_image] + elapsed_time;

                // If the next image delay is already passed, drop the image and check the next one.
                while(next_duration < elapsed_time && m_image_count + m_step < m_image_timestamps.size())
                {
                    next_duration  = m_image_timestamps[m_image_count + m_step] - current_time;
                    m_image_count += m_step;
                }

                // If it is the last image: stop the timer or loop
                if(m_image_count + m_step == m_image_to_read.size())
                {
                    m_timer->stop();
                    if(m_loop_video)
                    {
                        frame_tl->clear_timeline();
                        m_image_count = 0;
                        core::thread::timer::time_duration_t duration = std::chrono::milliseconds(1000 / m_fps);
                        m_timer->set_duration(duration);
                        m_timer->start();
                    }
                }
                else
                {
                    core::thread::timer::time_duration_t duration =
                        std::chrono::milliseconds(static_cast<std::int64_t>(next_duration));
                    m_timer->stop();
                    m_timer->set_duration(duration);
                    m_timer->start();
                }
            }
            else
            {
                m_image_count += m_step;
            }
        }
        else
        {
            SIGHT_ERROR("Images doesn't have the same size.");
        }
    }
    else if(!m_is_paused && m_loop_video)
    {
        const auto frame_tl = m_frame.lock();
        frame_tl->clear_timeline();
        m_image_count = 0;
    }
}

//------------------------------------------------------------------------------

void frame_grabber::update_zoom(cv::Mat _image)
{
    if(m_zoom_center.has_value())
    {
        // Compute zoom parameters
        const int width               = _image.size().width;
        const int height              = _image.size().height;
        const int cropped_area_width  = width / m_zoom_factor;
        const int cropped_area_height = height / m_zoom_factor;

        // Compute correct position for zoom center
        const int center_x_pix = (m_zoom_center.value()[0] + width / 2);
        const int center_y_pix = -(m_zoom_center.value()[1]) + height / 2;

        // Compute starting area of zoom rectangle in pixels
        const int x1 = center_x_pix - cropped_area_width / 2;
        const int y1 = center_y_pix - cropped_area_height / 2;

        // Compute a padded zoomed area image
        const auto image_rect   = cv::Rect({}, _image.size());
        const auto roi          = cv::Rect(x1, y1, cropped_area_width, cropped_area_height);
        const auto intersection = image_rect & roi;
        const auto inter_roi    = intersection - roi.tl();
        cv::Mat crop            = cv::Mat::zeros(roi.size(), _image.type());
        _image(intersection).copyTo(crop(inter_roi));

        // Rescale the zoomed image
        cv::resize(crop, _image, _image.size());
    }
}

// -----------------------------------------------------------------------------

void frame_grabber::toggle_loop_mode()
{
    m_loop_video = !m_loop_video;
}

// -----------------------------------------------------------------------------

void frame_grabber::set_position(int64_t _position)
{
    core::mt::scoped_lock lock(m_mutex);

    if(m_video_capture.isOpened())
    {
        m_video_capture.set(cv::CAP_PROP_POS_MSEC, static_cast<double>(_position));
    }
    else if(!m_image_to_read.empty())
    {
        const auto new_pos = static_cast<std::size_t>(_position / 30);
        if(new_pos < m_image_to_read.size())
        {
            m_image_count = new_pos;
        }
    }
}

//-----------------------------------------------------------------------------

void frame_grabber::next_image()
{
    if(m_one_shot)
    {
        // Compute difference between a possible step change in set_step() slot and the current step value
        const std::int64_t shift = static_cast<std::int64_t>(m_step_changed)
                                   - static_cast<std::int64_t>(m_step);
        const std::int64_t shifted_image_count = static_cast<std::int64_t>(m_image_count) + shift;

        if(shifted_image_count < static_cast<std::int64_t>(m_image_to_read.size()))
        {
            // Update image position index
            m_image_count = static_cast<std::size_t>(shifted_image_count);
            m_step        = m_step_changed;

            m_timer->stop();
            m_timer->start();
        }
        else
        {
            sight::ui::dialog::message::show(
                "Grabber",
                "No more image to read."
            );
        }
    }
}

//-----------------------------------------------------------------------------

void frame_grabber::previous_image()
{
    if(m_one_shot)
    {
        if(m_image_count - m_step >= m_step_changed)
        {
            // Compute difference between a possible step change in set_step() slot and the current step value
            const std::int64_t shift = static_cast<std::int64_t>(m_step_changed)
                                       - static_cast<std::int64_t>(m_step);
            const std::int64_t shiftedimage_count = static_cast<std::int64_t>(m_image_count) - shift;

            // Update image position index
            // m_imageCount is pointing to next image, so -1 = present image
            m_image_count = static_cast<std::size_t>(shiftedimage_count) - (2 * m_step);
            m_step        = m_step_changed;

            m_timer->stop();
            m_timer->start();
        }
        else
        {
            sight::ui::dialog::message::show(
                "Grabber",
                "No previous image."
            );
        }
    }
}

//-----------------------------------------------------------------------------

void frame_grabber::set_parameter(ui::parameter_t _value, std::string _key)
{
    if(_key == "step")
    {
        const int step = std::get<int>(_value);
        SIGHT_ASSERT("Needed step value (" << step << ") should be > 0.", step > 0);
        // Save the changed step value
        m_step_changed = static_cast<std::uint64_t>(step);
    }
    else
    {
        SIGHT_WARN("Only 'step' key is supported (current key value is : '" << _key << "').");
    }
}

//------------------------------------------------------------------------------

void frame_grabber::set_step(int _step, std::string _key)
{
    this->set_parameter(_step, _key);
}

//------------------------------------------------------------------------------

void frame_grabber::add_roi_center(sight::data::point::sptr _p)
{
    const auto& coord = _p->get_coord();

    m_zoom_center = {{static_cast<int>(std::nearbyint(coord[0])), static_cast<int>(std::nearbyint(coord[1]))}};
}

//------------------------------------------------------------------------------

void frame_grabber::remove_roi_center(sight::data::point::sptr /*p*/)
{
    m_zoom_center.reset();
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::video
