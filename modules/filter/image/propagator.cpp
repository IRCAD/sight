/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "modules/filter/image/propagator.hpp"

#include <core/com/signal.hxx>
#include <core/com/slots.hxx>
#include <core/jobs/job.hpp>

#include <data/helper/medical_image.hpp>

#include <filter/image/image_diff.hpp>

namespace sight::module::filter::image
{

//-----------------------------------------------------------------------------

propagator::propagator() :
    filter(m_signals),
    has_parameters(m_slots)
{
    new_signal<signals::job_created_t>(signals::JOB_CREATED);

    new_slot(slots::CLEAR, &propagator::clear, this);
    new_slot(slots::PROPAGATE, &propagator::propagate, this);
}

//-----------------------------------------------------------------------------

void propagator::configuring(const config_t& _config)
{
    service::config_t config = _config.get_child("config.<xmlattr>");

    m_value     = config.get<int>("value", 1);
    m_overwrite = config.get<bool>("overwrite", true);
    m_radius    = config.get<double>("radius", m_radius);

    const std::string mode = config.get<std::string>("mode", "min");

    if(mode == "min")
    {
        m_mode = sight::filter::image::min_max_propagation::min;
    }
    else if(mode == "max")
    {
        m_mode = sight::filter::image::min_max_propagation::max;
    }
    else if(mode == "minmax")
    {
        m_mode = sight::filter::image::min_max_propagation::minmax;
    }
    else if(mode == "stddev")
    {
        m_mode = sight::filter::image::min_max_propagation::stddev;
    }
    else
    {
        SIGHT_FATAL("Unknown mode '" + mode + "'. Accepted values are 'min', 'max', 'minmax', or 'stddev'.");
    }
}

//-----------------------------------------------------------------------------

void propagator::starting()
{
    this->updating();
}

//-----------------------------------------------------------------------------

void propagator::updating()
{
    const auto image_in = m_image_in.lock();
    SIGHT_ASSERT("No " << std::quoted(IMAGE_IN) << " found.", image_in);

    if(data::helper::medical_image::check_image_validity(image_in.get_shared()))
    {
        // Initializes the mask if empty
        {
            const auto image_out = m_image_out.lock();
            SIGHT_ASSERT("The image must be in 3 dimensions", image_in->num_dimensions() == 3);

            image_out->resize(image_in->size(), sight::core::type::UINT8, sight::data::image::gray_scale);
            image_out->set_spacing(image_in->spacing());
            image_out->set_origin(image_in->origin());
            const auto lock = image_out->dump_lock();
            std::fill(image_out->begin(), image_out->end(), std::uint8_t(0));
        }

        propagate();
    }
}

//-----------------------------------------------------------------------------

void propagator::propagate()
{
    const auto job = std::make_shared<sight::core::jobs::job>(
        "Propagation",
        [&](sight::core::jobs::job& _running_job)
        {
            // Convert point list into seeds
            sight::filter::image::min_max_propagation::seeds_t seeds;

            {
                const auto point_list = m_seeds_in.lock();

                // Early return that can help to avoid deadlocks when starting the service
                if(point_list->get_points().empty())
                {
                    _running_job.done_work(10);
                    return;
                }

                _running_job.done_work(1);
                const auto image_in = m_image_in.lock();
                SIGHT_ASSERT("No " << std::quoted(IMAGE_IN) << " found.", image_in);
                SIGHT_ASSERT("Invalid image", data::helper::medical_image::check_image_validity(image_in.get_shared()));
                const auto& bbox = sight::data::helper::medical_image::compute_bounding_box(*image_in);

                std::ranges::for_each(
                    point_list->get_points(),
                    [&](const auto& _x)
                {
                    const auto& pt = _x->get_coord();
                    if(pt[0] >= bbox.first[0] && pt[1] >= bbox.first[1] && pt[2] >= bbox.first[2]
                       && pt[0] <= bbox.second[0] && pt[1] <= bbox.second[1] && pt[2] <= bbox.second[2])
                    {
                        const auto indices = sight::data::helper::medical_image::compute_voxel_indices(*image_in, pt);
                        sight::filter::image::bresenham_line::coordinates_t indices_rounded;
                        std::ranges::transform(
                            indices,
                            indices_rounded.begin(),
                            [](const auto& _x){return static_cast<std::size_t>(_x);});
                        seeds.insert(indices_rounded);
                    }
                });
            }
            _running_job.done_work(1);

            {
                const auto image_in    = m_image_in.lock();
                const auto image_out   = m_image_out.lock();
                const auto propag_diff = sight::filter::image::min_max_propagation::process(
                    image_in.get_shared(),
                    image_out.get_shared(),
                    nullptr,
                    seeds,
                    static_cast<std::uint8_t>(m_value),
                    m_radius,
                    m_overwrite,
                    m_mode
                );
                _running_job.done_work(6);

                if(propag_diff.num_elements() > 0)
                {
                    image_out->signal<data::image::buffer_modified_signal_t>(
                        data::image::BUFFER_MODIFIED_SIG
                    )->async_emit();
                    this->signal<filter::signals::computed_t>(filter::signals::COMPUTED)->async_emit();

                    const auto samples_out = m_samples_out.lock();
                    if(samples_out)
                    {
                        sight::data::image::size_t voxels_size {propag_diff.num_elements(), 1, 1};

                        samples_out->resize(voxels_size, image_in->type(), image_in->pixel_format());

                        const auto lock = samples_out->dump_lock();
                        for(std::size_t i = 0 ; i < propag_diff.num_elements() ; ++i)
                        {
                            samples_out->set_pixel(i, propag_diff.get_element(i).m_old_value);
                        }

                        samples_out->signal<data::image::modified_signal_t>(data::image::MODIFIED_SIG)->async_emit();
                    }
                }
            }

            _running_job.done_work(2);
        });

    job->set_cancelable(false);
    job->set_total_work_units(10);

    this->signal<signals::job_created_t>(signals::JOB_CREATED)->emit(job);

    job->run().get();
}

//------------------------------------------------------------------------------

void propagator::clear()
{
    {
        const auto image_out = m_image_out.lock();
        const auto lock      = image_out->dump_lock();

        std::fill(image_out->begin(), image_out->end(), std::uint8_t(0));

        image_out->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG)->async_emit();
    }
    {
        const auto image_in = m_image_in.lock();
        SIGHT_ASSERT("No " << std::quoted(IMAGE_IN) << " found.", image_in);

        const auto samples_out = m_samples_out.lock();
        const auto lock        = samples_out->dump_lock();

        sight::data::image::size_t voxels_size {0, 1, 1};
        samples_out->resize(voxels_size, image_in->type(), image_in->pixel_format());

        samples_out->signal<data::image::buffer_modified_signal_t>(data::image::MODIFIED_SIG)->async_emit();
    }
}

//-----------------------------------------------------------------------------

void propagator::stopping()
{
}

//-----------------------------------------------------------------------------

void propagator::set_bool_parameter(bool _val, std::string _key)
{
    SIGHT_WARN_IF("Key must be 'overwrite' for this slot to have an effect.", _key != "overwrite");
    if(_key == "overwrite")
    {
        m_overwrite = _val;
    }
}

//-----------------------------------------------------------------------------

void propagator::set_int_parameter(int _val, std::string _key)
{
    SIGHT_WARN_IF("Key must be 'value' for this slot to have an effect.", _key != "value");
    if(_key == "value")
    {
        m_value = _val;
    }
}

//-----------------------------------------------------------------------------

void propagator::set_double_parameter(double _val, std::string _key)
{
    SIGHT_WARN_IF("Key must be 'radius' for this slot to have an effect.", _key != "radius");
    if(_key == "radius")
    {
        m_radius = _val;
    }
}

//-----------------------------------------------------------------------------

void propagator::set_enum_parameter(std::string _val, std::string _key)
{
    SIGHT_WARN_IF("Key must be 'mode' for this slot to have an effect.", _key != "mode");
    if(_key == "mode")
    {
        if(_val == "min")
        {
            m_mode = sight::filter::image::min_max_propagation::min;
        }
        else if(_val == "max")
        {
            m_mode = sight::filter::image::min_max_propagation::max;
        }
        else if(_val == "minmax")
        {
            m_mode = sight::filter::image::min_max_propagation::minmax;
        }
        else if(_val == "stddev")
        {
            m_mode = sight::filter::image::min_max_propagation::stddev;
        }
        else
        {
            SIGHT_WARN("Unknown mode '" + _val + "'. Accepted values are 'min', 'max', 'minmax' or 'stddev'.");
        }
    }
}

//-----------------------------------------------------------------------------

service::connections_t propagator::auto_connections() const
{
    return {
        {IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {SEEDS_IN, data::point_list::MODIFIED_SIG, slots::PROPAGATE}
    };
}

} // namespace sight::module::filter::image.
