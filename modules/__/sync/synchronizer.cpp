/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>

#include <data/image_series.hpp>

namespace sight::module::sync
{

// ----------------------------------------------------------------------------

synchronizer::synchronizer()
{
    new_slot(slots::RESET_TIMELINE_SLOT, &synchronizer::reset_timeline, this);
    new_slot(slots::SYNCHRONIZE_SLOT, &synchronizer::synchronize, this);
    new_slot(slots::SET_FRAME_BINDING_SLOT, &synchronizer::set_frame_binding, this);
    new_slot(slots::SET_MATRIX_BINDING_SLOT, &synchronizer::set_matrix_binding, this);
    new_slot(slots::SET_DELAY_SLOT, &synchronizer::set_delay, this);

    new_signal<signals::timestamp_signal_t>(signals::SYNCHRONIZATION_DONE_SIG);
    new_signal<signals::void_signal_t>(signals::SYNCHRONIZATION_SKIPPED_SIG);
    new_signal<signals::int_signal_t>(signals::FRAME_SYNCHRONIZED_SIG);
    new_signal<signals::int_signal_t>(signals::FRAME_UNSYNCHRONIZED_SIG);
    new_signal<signals::int_signal_t>(signals::MATRIX_SYNCHRONIZED_SIG);
    new_signal<signals::int_signal_t>(signals::MATRIX_UNSYNCHRONIZED_SIG);
}

//-----------------------------------------------------------------------------

service::connections_t synchronizer::auto_connections() const
{
    return {
        {config_key::FRAMETL_INPUT, data::timeline::signals::CLEARED, slots::RESET_TIMELINE_SLOT},
        {config_key::MATRIXTL_INPUT, data::timeline::signals::CLEARED, slots::RESET_TIMELINE_SLOT},
        {config_key::FRAMETL_INPUT, data::timeline::signals::PUSHED, service::slots::UPDATE},
        {config_key::MATRIXTL_INPUT, data::timeline::signals::PUSHED, service::slots::UPDATE}
    };
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
    const config_t configuration = this->get_config();

    const auto in_config = configuration.equal_range("in");
    for(auto it_config = in_config.first ; it_config != in_config.second ; ++it_config)
    {
        const std::string group = it_config->second.get<std::string>("<xmlattr>.group", "");
        if(group == config_key::FRAMETL_INPUT)
        {
            const auto key_config = it_config->second.equal_range(config_key::KEY);
            for(auto frame_tl_config = key_config.first ;
                frame_tl_config != key_config.second ;
                ++frame_tl_config)
            {
                int delay = frame_tl_config->second.get<int>(
                    config_key::TL_DELAY,
                    0
                );
                if(delay < 0)
                {
                    SIGHT_WARN("Synchronization delay should be positive");
                    delay = 0;
                }

                m_frame_tl_delay.push_back(delay);
            }
        }
        else if(group == config_key::MATRIXTL_INPUT)
        {
            const auto key_config = it_config->second.equal_range(config_key::KEY);
            for(auto matrix_tl_config = key_config.first ;
                matrix_tl_config != key_config.second ;
                ++matrix_tl_config)
            {
                int delay = matrix_tl_config->second.get<int>(
                    config_key::TL_DELAY,
                    0
                );
                if(delay < 0)
                {
                    SIGHT_WARN("Synchronization delay should be positive");
                    delay = 0;
                }

                m_matrix_tl_delay.push_back(delay);
            }
        }
    }

    const auto inouts_config = configuration.equal_range("inout");
    for(auto it_config = inouts_config.first ; it_config != inouts_config.second ; ++it_config)
    {
        const std::string group = it_config->second.get<std::string>("<xmlattr>.group", "");

        if(group == config_key::FRAME_INOUT)
        {
            std::size_t frame_index = 0;
            const auto key_config   = it_config->second.equal_range(config_key::KEY);
            for(auto frame_out_var_config = key_config.first ;
                frame_out_var_config != key_config.second ;
                ++frame_out_var_config, ++frame_index)
            {
                const std::size_t tl_index = frame_out_var_config->second.get<std::size_t>(
                    config_key::OUTVAR_TL_INDEX,
                    0
                );
                const unsigned int element_index = frame_out_var_config->second.get<unsigned int>(
                    config_key::OUTVAR_ELEMENT_INDEX,
                    0
                );
                const bool send_status = frame_out_var_config->second.get<bool>(
                    config_key::OUTVAR_SEND_STATUS,
                    false
                );

                m_frame_out_var_parameters.emplace_back(
                    out_var_parameter(
                    {
                        frame_index,
                        tl_index,
                        element_index,
                        false,
                        send_status,
                        0
                    })
                );
            }
        }
        else if(group == config_key::MATRIX_INOUT)
        {
            std::size_t matrix_index = 0;
            const auto key_config    = it_config->second.equal_range(config_key::KEY);
            for(auto matrix_out_var_config = key_config.first ;
                matrix_out_var_config != key_config.second ;
                ++matrix_out_var_config, ++matrix_index)
            {
                const std::size_t tl_index = matrix_out_var_config->second.get<std::size_t>(
                    config_key::OUTVAR_TL_INDEX,
                    0
                );
                const unsigned int element_index = matrix_out_var_config->second.get<unsigned int>(
                    config_key::OUTVAR_ELEMENT_INDEX,
                    0
                );
                const bool send_status = matrix_out_var_config->second.get<bool>(
                    config_key::OUTVAR_SEND_STATUS,
                    false
                );

                m_matrix_out_var_parameters.emplace_back(
                    out_var_parameter(
                    {
                        matrix_index,
                        tl_index,
                        element_index,
                        false,
                        send_status,
                        0
                    })
                );
            }
        }
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
    m_update_mask |= object_received;

    if(m_legacy_auto_sync)
    {
        return;
    }

    if((m_update_mask & sync_requested) != 0)
    {
        this->synchronize();
    }
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
    m_update_mask |= sync_requested;
    if((m_update_mask & object_received) == 0)
    {
        return;
    }

    // do the synchronisation
    // step 1: get the TL implicated in the synchronization
    std::vector<std::size_t> frame_tl_populated_index;
    std::vector<core::clock::type> frame_tl_populated_timestamp;

    for(std::size_t i = 0 ; i != m_frame_t_ls.size() ; ++i)
    {
        const auto tl = m_frame_t_ls[i].lock();
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

    for(std::size_t i = 0 ; i != m_matrix_t_ls.size() ; ++i)
    {
        const auto tl = m_matrix_t_ls[i].lock();
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
    const auto frame_tl_max_timestamp = frame_tl_populated_timestamp.empty() ? 0 : *(std::max_element(
                                                                                         frame_tl_populated_timestamp.
                                                                                         begin(),
                                                                                         frame_tl_populated_timestamp.
                                                                                         end()
    ));

    const auto matrix_tl_max_timestamp = matrix_tl_populated_timestamp.empty() ? 0 : *(std::max_element(
                                                                                           matrix_tl_populated_timestamp
                                                                                           .begin(),
                                                                                           matrix_tl_populated_timestamp
                                                                                           .end()
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
        // there is nothing to synchronize
        SIGHT_INFO("skip synch, because there is nothing to synch");
        this->signal<signals::void_signal_t>(signals::SYNCHRONIZATION_SKIPPED_SIG)->async_emit();
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
            copy_frame_from_t_lto_output(tl_index, synchronization_timestamp);
        }

        for(const std::size_t tl_index : matrix_tl_to_synch_index)
        {
            copy_matrix_from_t_lto_output(tl_index, synchronization_timestamp);
        }

        this->signal<signals::timestamp_signal_t>(signals::SYNCHRONIZATION_DONE_SIG)->async_emit(
            synchronization_timestamp
        );
        send_frame_var_status(frame_tl_to_synch_index);
        send_matrix_var_status(matrix_tl_to_synch_index);

        // synchronisation has been done with success, reset the mask
        m_update_mask = m_legacy_auto_sync ? object_received : 0;
    }
    else
    {
        SIGHT_INFO("skip synch, because m_lastTimeStamp == synchronizationTimestamp");
        this->signal<signals::void_signal_t>(signals::SYNCHRONIZATION_SKIPPED_SIG)->async_emit();
    }
}

// ----------------------------------------------------------------------------

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

void synchronizer::copy_frame_from_t_lto_output(
    std::size_t _frame_tl_index,
    core::clock::type _synchronization_timestamp
)
{
    const auto frame_tl = m_frame_t_ls[_frame_tl_index].lock();
    CSPTR(data::frame_tl::buffer_t) buffer =
        frame_tl->get_closest_buffer(_synchronization_timestamp - m_frame_tl_delay[_frame_tl_index]);

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
                enum data::image::pixel_format format {data::image::undefined};
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
                image_series->set_frame_acquisition_time_point(_synchronization_timestamp);
            }

            const std::uint8_t* frame_buff = &buffer->get_element(frame_tl_element_index);
            auto iter                      = frame->begin<std::uint8_t>();
            std::memcpy(&*iter, frame_buff, buffer->size());

            // Notify
            auto sig = frame->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG);
            sig->async_emit();
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

void synchronizer::copy_matrix_from_t_lto_output(
    std::size_t _matrix_tl_index,
    core::clock::type _synchronization_timestamp
)
{
    const auto matrix_tl = m_matrix_t_ls[_matrix_tl_index].lock();
    CSPTR(data::matrix_tl::buffer_t) buffer =
        matrix_tl->get_closest_buffer(_synchronization_timestamp - m_matrix_tl_delay[_matrix_tl_index]);

    if(buffer)
    {
        for(const auto output_var_param : get_matrix_tl_output_var_index(_matrix_tl_index))
        {
            const std::size_t matrix_out_index         = output_var_param.out_var_index;
            const unsigned int matrix_tl_element_index = output_var_param.tl_element_index;

            if(buffer->is_present(matrix_tl_element_index))
            {
                auto matrix = m_matrix[matrix_out_index].lock();
                SIGHT_ASSERT("Matrix with indices '" << matrix_out_index << "' does not exist", matrix);
                const auto& values = buffer->get_element(matrix_tl_element_index);
                for(std::uint8_t i = 0 ; i < 4 ; ++i)
                {
                    for(std::uint8_t j = 0 ; j < 4 ; ++j)
                    {
                        (*matrix)(i, j) = static_cast<double>(values[i * 4 + j]);
                    }
                }

                auto sig = matrix->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
                sig->async_emit();
            }
        }
    }
    else
    {
        SIGHT_ERROR(
            "Buffer not found for timestamp " << _synchronization_timestamp << " in timeline 'matrix" << _matrix_tl_index
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
                std::find(
                    _synch_frame_tl_index.begin(),
                    _synch_frame_tl_index.end(),
                    output_var_param.tl_index
                ) != _synch_frame_tl_index.end();
            if(output_var_param.is_synchronized != is_synch)
            {
                output_var_param.is_synchronized = is_synch;
                const auto signal_key =
                    is_synch ? signals::FRAME_SYNCHRONIZED_SIG : signals::FRAME_UNSYNCHRONIZED_SIG;
                this->signal<signals::int_signal_t>(signal_key)->async_emit(
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
                std::find(
                    _synch_matrix_tl_index.begin(),
                    _synch_matrix_tl_index.end(),
                    output_var_param.tl_index
                ) != _synch_matrix_tl_index.end();
            if(output_var_param.is_synchronized != is_synch)
            {
                output_var_param.is_synchronized = is_synch;
                const auto signal_key =
                    is_synch ? signals::MATRIX_SYNCHRONIZED_SIG : signals::MATRIX_UNSYNCHRONIZED_SIG;
                this->signal<signals::int_signal_t>(signal_key)->async_emit(
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
    if(_key.rfind(slots::FRAME_DELAY_PREFIX, 0) == 0)
    {
        try
        {
            static constexpr size_t s_FRAME_DELAY_KEY_SIZE = std::size(slots::FRAME_DELAY_PREFIX);
            const size_t frame_tl_index                    =
                static_cast<size_t>(std::stoul(_key.substr(s_FRAME_DELAY_KEY_SIZE)));
            if(frame_tl_index < m_frame_tl_delay.size())
            {
                m_frame_tl_delay[frame_tl_index] = _val;
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
    else if(_key.rfind(slots::MATRIX_DELAY_PREFIX, 0) == 0)
    {
        try
        {
            static constexpr size_t s_MATRIX_DELAY_KEY_SIZE = std::size(slots::MATRIX_DELAY_PREFIX);
            const size_t matrix_tl_index                    =
                static_cast<size_t>(std::stoul(_key.substr(s_MATRIX_DELAY_KEY_SIZE)));
            if(matrix_tl_index < m_matrix_tl_delay.size())
            {
                m_matrix_tl_delay[matrix_tl_index] = _val;
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
