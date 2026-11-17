/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include "synchronizer.hpp"

#include <algorithm>

#include <data/image_series.hpp>

namespace sight::module::sync
{

// ----------------------------------------------------------------------------

synchronizer::synchronizer()
{
    new_slot(slots::RESET_TIMELINE, &synchronizer::reset_timeline, this);
    new_slot(slots::TRY_SYNC, &synchronizer::try_sync, this);
    new_slot(slots::REQUEST_SYNC, &synchronizer::request_sync, this);
    new_slot(slots::SET_FRAME_BINDING, &synchronizer::set_frame_binding, this);
    new_slot(slots::SET_MATRIX_BINDING, &synchronizer::set_matrix_binding, this);
    new_slot(slots::SET_DELAY, &synchronizer::set_delay, this);

    new_signal<signals::timestamp_t>(signals::SYNCHRONIZATION_DONE);
    new_signal<signals::int_t>(signals::FRAME_SYNCHRONIZED);
    new_signal<signals::int_t>(signals::FRAME_UNSYNCHRONIZED);
    new_signal<signals::int_t>(signals::MATRIX_SYNCHRONIZED);
    new_signal<signals::int_t>(signals::MATRIX_UNSYNCHRONIZED);
}

//-----------------------------------------------------------------------------

service::connections_t synchronizer::auto_connections() const
{
    return {
        {config_key::FRAME_TIMELINES, data::timeline::signals::CLEARED, slots::RESET_TIMELINE},
        {config_key::MATRIX_TIMELINES, data::timeline::signals::CLEARED, slots::RESET_TIMELINE},
        {config_key::FRAME_TIMELINES, data::timeline::signals::PUSHED, slots::TRY_SYNC},
        {config_key::MATRIX_TIMELINES, data::timeline::signals::PUSHED, slots::TRY_SYNC}
    };
}

// ----------------------------------------------------------------------------

std::optional<std::string> synchronizer::resolve_object_type(
    std::string_view _key,
    std::optional<std::size_t> _index
) const
{
    if(_index.has_value()
       && (_key == config_key::FRAME_TIMELINES || _key == config_key::MATRIX_TIMELINES
           || _key == config_key::FRAMES || _key == config_key::MATRICES))
    {
        return std::nullopt;
    }

    if(_index.has_value()
       && (_key == config_key::FRAME_DELAYS || _key == config_key::MATRIX_DELAYS
           || _key == config_key::FRAME_TIMELINE || _key == config_key::FRAME_INDEX
           || _key == config_key::MATRIX_TIMELINE || _key == config_key::MATRIX_INDEX))
    {
        return data::integer::classname();
    }

    if(_index.has_value() && (_key == config_key::FRAME_SEND_STATUS || _key == config_key::MATRIX_SEND_STATUS))
    {
        return data::boolean::classname();
    }

    return service::synchronizer::resolve_object_type(_key, _index);
}

// ----------------------------------------------------------------------------

void synchronizer::configuring()
{
    const auto cfg = this->get_config();

    m_legacy_auto_sync = cfg.get<bool>(config_key::LEGACY_AUTO_SYNCH, m_legacy_auto_sync);

    m_tolerance = cfg.get<core::clock::type>(config_key::TOLERANCE, m_tolerance);
}

//-----------------------------------------------------------------------------

void synchronizer::starting()
{
    const auto make_out_var_parameter = [](std::size_t _out_var_index, const auto& _timeline, const auto& _index,
                                           const auto& _send_status)
                                        {
                                            const auto timeline    = _timeline.lock();
                                            const auto index       = _index.lock();
                                            const auto send_status = _send_status.lock();

                                            return out_var_parameter {
                                                .out_var_index = _out_var_index,
                                                .tl_index      =
                                                    timeline ? static_cast<std::size_t>(timeline->value()) : 0,
                                                .tl_element_index =
                                                    index ? static_cast<unsigned int>(index->value()) : 0,
                                                .is_synchronized        = false,
                                                .signal_synchronization = send_status && send_status->value()
                                            };
                                        };

    m_frame_out_var_parameters.clear();
    m_frame_out_var_parameters.reserve(m_frames.size());
    for(std::size_t index = 0 ; index < m_frames.size() ; ++index)
    {
        m_frame_out_var_parameters.push_back(
            make_out_var_parameter(index, m_frame_timelines[index], m_frame_indices[index], m_frame_send_status[index])
        );
    }

    m_matrix_out_var_parameters.clear();
    m_matrix_out_var_parameters.reserve(m_matrices.size());
    for(std::size_t index = 0 ; index < m_matrices.size() ; ++index)
    {
        m_matrix_out_var_parameters.push_back(
            make_out_var_parameter(
                index,
                m_matrix_timelines[index],
                m_matrix_indices[index],
                m_matrix_send_status[index]
            )
        );
    }

    SIGHT_ASSERT("No valid worker for timer.", this->worker());
    if(m_legacy_auto_sync)
    {
        m_worker = sight::core::thread::worker::make();
        m_timer  = m_worker->create_timer();
        const auto duration = std::chrono::milliseconds(m_time_step);
        m_timer->set_function([this](auto&& ...){synchronize();});
        m_timer->set_duration(duration);
        m_timer->start();
    }
}

// ----------------------------------------------------------------------------

void synchronizer::updating()
{
    this->synchronize();
}

// ----------------------------------------------------------------------------

void synchronizer::stopping()
{
    if(m_legacy_auto_sync)
    {
        m_timer->stop();
        m_timer.reset();
        m_worker->stop();
        m_worker.reset();
    }
}

// ----------------------------------------------------------------------------

void synchronizer::synchronize()
{
    // do the synchronisation
    // step 1: get the TL implicated in the synchronization
    std::vector<std::size_t> frame_tl_populated_index;
    std::vector<core::clock::type> frame_tl_populated_timestamp;

    for(std::size_t i = 0 ; i != m_frame_tls.size() ; ++i)
    {
        const auto tl = m_frame_tls[i].lock();
        // if the tl is null, ignore it
        if(tl)
        {
            // get the tl new timestamp
            const auto tl_newest_timestamp = tl->get_newer_timestamp();

            //treat only the tl with some data inside.
            if(tl_newest_timestamp > 0)
            {
                frame_tl_populated_timestamp.push_back(tl_newest_timestamp);
                frame_tl_populated_index.push_back(i);
            }
        }
    }

    std::vector<std::size_t> matrix_tl_populated_index;
    std::vector<core::clock::type> matrix_tl_populated_timestamp;

    for(std::size_t i = 0 ; i != m_matrix_tls.size() ; ++i)
    {
        const auto tl = m_matrix_tls[i].lock();
        if(tl)
        {
            // get the tl new timestamp
            const auto tl_newest_timestamp = tl->get_newer_timestamp();

            //treat only the tl with some data inside.
            if(tl_newest_timestamp > 0)
            {
                matrix_tl_populated_timestamp.push_back(tl_newest_timestamp);
                matrix_tl_populated_index.push_back(i);
            }
        }
    }

    // step 2: find the synchronization timestamp

    // Timestamp reference for the synchronization
    const auto frame_tl_max_timestamp = frame_tl_populated_timestamp.empty() ? 0 : *(std::ranges::max_element(
                                                                                         frame_tl_populated_timestamp

    ));

    const auto matrix_tl_max_timestamp = matrix_tl_populated_timestamp.empty() ? 0 : *(std::ranges::max_element(
                                                                                           matrix_tl_populated_timestamp

    ));

    core::clock::type max_synchronization_timestamp = std::max(
        frame_tl_max_timestamp,
        matrix_tl_max_timestamp
    );

    // This gives the most recent timestamp provided in a TL.
    // However the max is not enough, as some timelines have there own maxTimestamp < global maxTimestamp.
    // Using the global max, will imply that only the most recent TL can be properly synchronized.
    // To cover this, the algorithm will find every TL which are in the tolerance range from the max, and will take the
    // minimum.
    // This should allow a synchronization around a reference timestamp which is up to date, while being consensual
    // over the populated TL.

    if(max_synchronization_timestamp == 0)
    {
        // Nothing to synchronize, print a debug message
        SIGHT_DEBUG("skip sync, because there is nothing to sync");
        this->async_emit(signals::SYNCHRONIZATION_DONE, max_synchronization_timestamp);
        return;
    }

    core::clock::type synchronization_timestamp = max_synchronization_timestamp;
    std::vector<std::size_t> frame_tl_to_synch_index;
    for(std::size_t i = 0 ; i < frame_tl_populated_timestamp.size() ; i++)
    {
        if(max_synchronization_timestamp - frame_tl_populated_timestamp[i] < m_tolerance)
        {
            synchronization_timestamp = std::min(synchronization_timestamp, frame_tl_populated_timestamp[i]);
            frame_tl_to_synch_index.push_back(frame_tl_populated_index[i]);
        }
    }

    std::vector<std::size_t> matrix_tl_to_synch_index;
    for(std::size_t i = 0 ; i < matrix_tl_populated_timestamp.size() ; i++)
    {
        if(max_synchronization_timestamp - matrix_tl_populated_timestamp[i] < m_tolerance)
        {
            synchronization_timestamp = std::min(synchronization_timestamp, matrix_tl_populated_timestamp[i]);
            matrix_tl_to_synch_index.push_back(matrix_tl_populated_index[i]);
        }
    }

    //step 3: get the matrix + frame and populate the output

    if(m_last_time_stamp != synchronization_timestamp)
    {
        m_last_time_stamp = synchronization_timestamp;

        for(const std::size_t tl_index : frame_tl_to_synch_index)
        {
            copy_frame_from_tl_to_output(tl_index, synchronization_timestamp);
        }

        for(const std::size_t tl_index : matrix_tl_to_synch_index)
        {
            copy_matrix_from_tl_to_output(tl_index, synchronization_timestamp);
        }

        this->async_emit(signals::SYNCHRONIZATION_DONE, synchronization_timestamp);
        send_frame_var_status(frame_tl_to_synch_index);
        send_matrix_var_status(matrix_tl_to_synch_index);
    }
    else
    {
        this->async_emit(signals::SYNCHRONIZATION_DONE, synchronization_timestamp);
    }
}

// ----------------------------------------------------------------------------

void synchronizer::try_sync()
{
    if(m_locked)
    {
        return;
    }

    m_locked = true;
    this->synchronize();
}

//------------------------------------------------------------------------------

void synchronizer::request_sync()
{
    m_locked = false;
}

//------------------------------------------------------------------------------

std::vector<synchronizer::out_var_parameter> synchronizer::get_frame_tl_output_var_index(std::size_t _frame_tl_index)
{
    std::vector<out_var_parameter> result;
    for(auto outvar_param : m_frame_out_var_parameters)
    {
        if(outvar_param.tl_index == _frame_tl_index)
        {
            result.push_back(outvar_param);
        }
    }

    return result;
}

// ----------------------------------------------------------------------------

void synchronizer::copy_frame_from_tl_to_output(
    std::size_t _frame_tl_index,
    core::clock::type _synchronization_timestamp
)
{
    const auto frame_tl = m_frame_tls[_frame_tl_index].lock();
    const auto delay    = static_cast<core::clock::type>(*m_frame_tl_delays[_frame_tl_index]);
    const auto buffer   = frame_tl->get_closest_buffer(_synchronization_timestamp - delay);

    data::image::size_t frame_tl_size = {frame_tl->get_width(), frame_tl->get_height(), 0};

    std::size_t frame_tl_num_components                     = frame_tl->num_components();
    core::type frame_tl_type                                = frame_tl->type();
    enum data::frame_tl::pixel_format frame_tl_pixel_format = frame_tl->pixel_format();

    if(buffer)
    {
        for(const out_var_parameter output_var_param : get_frame_tl_output_var_index(_frame_tl_index))
        {
            const std::size_t frame_out_index         = output_var_param.out_var_index;
            const unsigned int frame_tl_element_index = output_var_param.tl_element_index;

            const auto frame = m_frames[frame_out_index].lock();
            SIGHT_ASSERT("image with index '" << frame_out_index << "' does not exist", frame);

            // Check if frame dimensions have changed
            if(frame_tl_size != frame->size() || frame_tl_num_components != frame->num_components())
            {
                enum data::image::pixel_format_t format
                {
                    data::image::undefined
                };
                switch(frame_tl_pixel_format)
                {
                    case data::frame_tl::pixel_format::gray_scale:
                        format = data::image::gray_scale;
                        break;

                    case data::frame_tl::pixel_format::rgb:
                        format = data::image::rgb;
                        break;

                    case data::frame_tl::pixel_format::bgr:
                        format = data::image::bgr;
                        break;

                    case data::frame_tl::pixel_format::rgba:
                        format = data::image::rgba;
                        break;

                    case data::frame_tl::pixel_format::bgra:
                        format = data::image::bgra;
                        break;

                    default:
                        SIGHT_ERROR("FrameTL pixel format undefined");
                        return;
                }

                frame->resize(frame_tl_size, frame_tl_type, format);
                const data::image::origin_t origin = {0., 0., 0.};
                frame->set_origin(origin);
                const data::image::spacing_t spacing = {1., 1., 1.};
                frame->set_spacing(spacing);
                frame->set_window_width({1.0});
                frame->set_window_center({0.0});
            }

            // Set the time stamp on the image, if we set dicom image as output.
            // The value must be set after the previous `if`, in order to prevent the data
            // from being erased in case of resize (frameTLSize has a 0 value as 3 dimension)
            // and thus prevent the timestamp to be lost
            if(auto image_series =
                   std::dynamic_pointer_cast<sight::data::image_series>(frame.get_shared()); image_series)
            {
                image_series->set_frame_acquisition_time_point(_synchronization_timestamp, 0);
            }

            const std::uint8_t* frame_buff = &buffer->get_element(frame_tl_element_index);
            auto iter                      = frame->begin<std::uint8_t>();
            std::memcpy(&*iter, frame_buff, buffer->size());

            // Notify

            frame->async_emit(data::image::signals::BUFFER_MODIFIED);
        }
    }
    else
    {
        SIGHT_ERROR(
            "Buffer not found for timestamp " << _synchronization_timestamp << " in timeline 'frame" << _frame_tl_index
            << "'."
        );
    }
}

// ----------------------------------------------------------------------------

std::vector<synchronizer::out_var_parameter> synchronizer::get_matrix_tl_output_var_index(
    std::size_t _matrix_tl_index
)
{
    std::vector<out_var_parameter> result;
    for(auto outvar_param : m_matrix_out_var_parameters)
    {
        if(outvar_param.tl_index == _matrix_tl_index)
        {
            result.push_back(outvar_param);
        }
    }

    return result;
}

//------------------------------------------------------------------------------

void synchronizer::copy_matrix_from_tl_to_output(
    std::size_t _matrix_tl_index,
    core::clock::type _synchronization_timestamp
)
{
    const auto matrix_tl = m_matrix_tls[_matrix_tl_index].lock();
    const auto delay     = static_cast<core::clock::type>(*m_matrix_tl_delays[_matrix_tl_index]);
    const auto buffer    = matrix_tl->get_closest_buffer(_synchronization_timestamp - delay);

    if(buffer)
    {
        for(const auto output_var_param : get_matrix_tl_output_var_index(_matrix_tl_index))
        {
            const std::size_t matrix_out_index         = output_var_param.out_var_index;
            const unsigned int matrix_tl_element_index = output_var_param.tl_element_index;

            if(buffer->is_present(matrix_tl_element_index))
            {
                auto matrix = m_matrices[matrix_out_index].lock();
                SIGHT_ASSERT("Matrix with indices '" << matrix_out_index << "' does not exist", matrix);
                const auto& values = buffer->get_element(matrix_tl_element_index);
                for(std::uint8_t i = 0 ; i < 4 ; ++i)
                {
                    for(std::uint8_t j = 0 ; j < 4 ; ++j)
                    {
                        (*matrix)(i, j) = static_cast<double>(values[i * 4 + j]);
                    }
                }

                matrix->async_emit(data::signals::MODIFIED);
            }
        }
    }
    else
    {
        SIGHT_ERROR(
            "Buffer not found for timestamp " << _synchronization_timestamp << " in timeline 'matrix"
            << _matrix_tl_index
            << "'."
        );
    }
}

//-----------------------------------------------------------------------------

void synchronizer::send_frame_var_status(const std::vector<std::size_t>& _synch_frame_tl_index)
{
    for(auto& output_var_param : m_frame_out_var_parameters)
    {
        if(output_var_param.signal_synchronization)
        {
            //a signal should be send when synchronized/un-synchronized
            bool is_synch =
                std::ranges::find(
                    _synch_frame_tl_index,

                    output_var_param.tl_index
                ) != _synch_frame_tl_index.end();
            if(output_var_param.is_synchronized != is_synch)
            {
                output_var_param.is_synchronized = is_synch;
                const auto signal_key =
                    is_synch ? signals::FRAME_SYNCHRONIZED : signals::FRAME_UNSYNCHRONIZED;
                this->async_emit(
                    signal_key,
                    static_cast<int>(output_var_param.
                                     out_var_index)
                );
            }
        }
    }
}

//-----------------------------------------------------------------------------

void synchronizer::send_matrix_var_status(const std::vector<std::size_t>& _synch_matrix_tl_index)
{
    for(auto& output_var_param : m_matrix_out_var_parameters)
    {
        if(output_var_param.signal_synchronization)
        {
            //a signal should be send when synchronized/un-synchronized
            bool is_synch =
                std::ranges::find(
                    _synch_matrix_tl_index,

                    output_var_param.tl_index
                ) != _synch_matrix_tl_index.end();
            if(output_var_param.is_synchronized != is_synch)
            {
                output_var_param.is_synchronized = is_synch;
                const auto signal_key =
                    is_synch ? signals::MATRIX_SYNCHRONIZED : signals::MATRIX_UNSYNCHRONIZED;
                this->async_emit(
                    signal_key,
                    static_cast<int>(output_var_param.
                                     out_var_index)
                );
            }
        }
    }
}

// ----------------------------------------------------------------------------

void synchronizer::reset_timeline()
{
    m_last_time_stamp = 0.;
}

//-----------------------------------------------------------------------------

void synchronizer::set_frame_binding(
    std::size_t _tl_index,
    unsigned int _element_index,
    std::size_t _output_var_index
)
{
    for(auto& output_var_param : m_frame_out_var_parameters)
    {
        if(output_var_param.out_var_index == _output_var_index)
        {
            output_var_param.tl_index         = _tl_index;
            output_var_param.tl_element_index = _element_index;
            return;
        }
    }

    SIGHT_WARN(
        "The outputVar Index " << _output_var_index
        << " provided in the slot setFrameBinding has not been found."
    );
}

//-----------------------------------------------------------------------------

void synchronizer::set_matrix_binding(
    std::size_t _tl_index,
    unsigned int _element_index,
    std::size_t _output_var_index
)
{
    for(auto& output_var_param : m_matrix_out_var_parameters)
    {
        if(output_var_param.out_var_index == _output_var_index)
        {
            output_var_param.tl_index         = _tl_index;
            output_var_param.tl_element_index = _element_index;
            return;
        }
    }

    SIGHT_WARN(
        "The outputVar Index " << _output_var_index
        << " provided in the slot setMatrixBinding has not been found."
    );
}

//-----------------------------------------------------------------------------

void synchronizer::set_delay(int _val, std::string _key)
{
    if(_val < 0)
    {
        SIGHT_ERROR("The delay set for " << _key << " is negative. A positive value is expected");
        return;
    }

    /**
     * if the key received is of format frameDelay_i where i is a number,
     * it means that the value is a delay set for the i th frameTL
     * This works respectively for matrixDelay_i and matrixTL
     */
    if(_key.starts_with(slots::FRAME_DELAY_PREFIX))
    {
        try
        {
            static constexpr size_t s_FRAME_DELAY_KEY_SIZE = std::size(slots::FRAME_DELAY_PREFIX);
            const size_t frame_tl_index                    =
                static_cast<size_t>(std::stoul(_key.substr(s_FRAME_DELAY_KEY_SIZE)));
            if(frame_tl_index < m_frame_tl_delays.size())
            {
                auto delay = m_frame_tl_delays[frame_tl_index].lock();
                delay->set_value(_val);
            }
            else
            {
                SIGHT_ERROR(
                    "The frameTL index " << frame_tl_index
                    << " provided in the update delay slot is out of bound"
                );
            }
        }
        catch(...)
        {
            SIGHT_ERROR("The frameTL index provided in the update delay slot is not a proper number: " << _key);
        }
    }
    else if(_key.starts_with(slots::MATRIX_DELAY_PREFIX))
    {
        try
        {
            static constexpr size_t s_MATRIX_DELAY_KEY_SIZE = std::size(slots::MATRIX_DELAY_PREFIX);
            const size_t matrix_tl_index                    =
                static_cast<size_t>(std::stoul(_key.substr(s_MATRIX_DELAY_KEY_SIZE)));
            if(matrix_tl_index < m_matrix_tl_delays.size())
            {
                auto delay = m_matrix_tl_delays[matrix_tl_index].lock();
                delay->set_value(_val);
            }
            else
            {
                SIGHT_ERROR(
                    "The matrixTL index " << matrix_tl_index << " provided in the update delay slot is out of bound"
                );
            }
        }
        catch(...)
        {
            SIGHT_ERROR("The matrixTL index provided in the update delay slot is not a proper number: " << _key);
        }
    }
    else
    {
        SIGHT_WARN("Unknown key");
    }
}

} // namespace sight::module::sync
