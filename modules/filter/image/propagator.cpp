/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2021 IHU Strasbourg
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

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hpp>
#include <core/com/slot.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>

#include <filter/image/image_diff.hpp>

#include <service/macros.hpp>

#include <ui/history/ImageDiffCommand.hpp>

namespace sight::module::filter::image
{

static const core::com::signals::key_t DRAWN_SIG = "drawn";

static const core::com::slots::key_t DRAW_SLOT            = "draw";
static const core::com::slots::key_t SET_ORIENTATION_SLOT = "setOrientation";
static const core::com::slots::key_t RESET_DRAWING        = "resetDrawing";

//-----------------------------------------------------------------------------

propagator::propagator() :
    has_parameters(m_slots)
{
    new_slot(DRAW_SLOT, &propagator::draw, this);
    new_slot(SET_ORIENTATION_SLOT, &propagator::set_orientation, this);
    new_slot(RESET_DRAWING, &propagator::reset_drawing, this);

    m_sig_drawn = new_signal<drawn_signal_t>(DRAWN_SIG);
}

//-----------------------------------------------------------------------------

propagator::~propagator()
= default;

//-----------------------------------------------------------------------------

void propagator::configuring()
{
    service::config_t config = this->get_config();

    m_value     = config.get<int>("value", 1);
    m_overwrite = config.get<bool>("overwrite", true);
    m_radius    = config.get<double>("radius", std::numeric_limits<double>::infinity());

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
    else
    {
        SIGHT_FATAL("Unknown mode '" + mode + "'. Accepted values are 'min', 'max' or 'minmax'.");
    }

    const std::string orientation = config.get<std::string>("orientation", "axial");

    if(orientation == "sagital")
    {
        m_orientation = data::helper::medical_image::x_axis;
    }
    else if(orientation == "frontal")
    {
        m_orientation = data::helper::medical_image::y_axis;
    }
    else if(orientation == "axial")
    {
        m_orientation = data::helper::medical_image::z_axis;
    }
    else
    {
        SIGHT_FATAL("Unknown orientation '" + orientation + "'");
    }
}

//-----------------------------------------------------------------------------

void propagator::starting()
{
    const auto img_in_lock = m_image_in.lock();
    SIGHT_ASSERT("No " << IMAGE_IN << " found.", img_in_lock);
    const auto img_out_lock = m_image_out.lock();
    SIGHT_ASSERT("No " << IMAGE_INOUT << " found.", img_out_lock);

    bool is_valid = data::helper::medical_image::check_image_validity(img_in_lock.get_shared())
                    && data::helper::medical_image::check_image_validity(img_out_lock.get_shared());

    SIGHT_FATAL_IF("Input and output image must have the same size.", img_in_lock->size() != img_out_lock->size());
    SIGHT_WARN_IF(
        "Input and output image must have the same spacing.",
        img_in_lock->spacing() != img_out_lock->spacing()
    );

    if(is_valid)
    {
        m_propagator = std::make_unique<sight::filter::image::min_max_propagation>(
            img_in_lock.get_shared(),
            img_out_lock.get_shared(),
            nullptr
        );
        m_line_drawer = std::make_unique<sight::filter::image::line_drawer>(img_out_lock.get_shared(), nullptr);
    }
}

//-----------------------------------------------------------------------------

void propagator::updating()
{
    this->stopping();
    this->starting();
}

//-----------------------------------------------------------------------------

void propagator::stopping()
{
    m_propagator.reset();
    m_drawing = false;
}

//-----------------------------------------------------------------------------

void propagator::set_orientation(int _from, int _to)
{
    if(_to == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(_from);
    }
    else if(_from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(_to);
    }
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
        else
        {
            SIGHT_WARN("Unknown mode '" + _val + "'. Accepted values are 'min', 'max' or 'minmax'.");
        }
    }
}

//-----------------------------------------------------------------------------

void propagator::reset_drawing()
{
    m_drawing = false;
    m_diff.clear();
}

//-----------------------------------------------------------------------------

void propagator::draw(data::tools::picking_info _picking_info)
{
    SIGHT_ASSERT("Drawer not instantiated, have you started the service ?", m_line_drawer);

    const auto img_out_lock = m_image_out.lock();
    SIGHT_ASSERT(IMAGE_INOUT << " does not exist", img_out_lock);

    SPTR(data::image::buffer_t) val =
        data::helper::medical_image::get_pixel_in_image_space(img_out_lock.get_shared(), m_value);

    const data::image::spacing_t img_spacing = img_out_lock->spacing();
    // Draw lines as thick as a single voxel.
    const double thickness = *std::min_element(img_spacing.begin(), img_spacing.end());

    coordinates_t new_point = {{static_cast<coordinates_t::value_type>(_picking_info.m_world_pos[0]),
        static_cast<coordinates_t::value_type>(_picking_info.m_world_pos[1]),
        static_cast<coordinates_t::value_type>(_picking_info.m_world_pos[2])
    }
    };

    bool img_buffer_modified = false;
    if(_picking_info.m_event_id == data::tools::picking_info::event::mouse_left_down)
    {
        m_drawing   = true;
        m_old_point = new_point;

        m_diff = m_line_drawer->draw(m_orientation, m_old_point, m_old_point, val.get(), thickness, m_overwrite);

        img_buffer_modified = m_diff.num_elements() > 0;
    }
    else if(m_drawing && _picking_info.m_event_id == data::tools::picking_info::event::mouse_move)
    {
        const auto diff = m_line_drawer->draw(
            m_orientation,
            m_old_point,
            new_point,
            val.get(),
            thickness,
            m_overwrite
        );
        m_old_point = new_point;

        img_buffer_modified = this->append_diff(diff);
    }
    else if(m_drawing && _picking_info.m_event_id == data::tools::picking_info::event::mouse_left_up)
    {
        const auto diff = m_line_drawer->draw(
            m_orientation,
            m_old_point,
            new_point,
            val.get(),
            thickness,
            m_overwrite
        );

        img_buffer_modified = this->append_diff(diff);

        auto seeds = this->convert_diff_to_seeds();

        sight::filter::image::image_diff propag_diff;

        propag_diff = m_propagator->propagate(seeds, val.get(), m_radius, m_overwrite, m_mode);

        img_buffer_modified |= this->append_diff(propag_diff);

        if(m_diff.num_elements() > 0)
        {
            ui::history::image_diff_command::sptr diff_command(new ui::history::image_diff_command(
                                                                   img_out_lock.get_shared(),
                                                                   m_diff
            ));
            m_sig_drawn->async_emit(diff_command);
            m_diff.clear();
        }

        m_drawing = false;

        m_sig_computed->async_emit();
    }

    if(img_buffer_modified)
    {
        auto sig = img_out_lock->signal<data::image::buffer_modified_signal_t>(data::image::BUFFER_MODIFIED_SIG);
        core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
        sig->async_emit();
    }
}

//-----------------------------------------------------------------------------

bool propagator::append_diff(const sight::filter::image::image_diff& _diff)
{
    const bool append = (_diff.num_elements() > 0);

    if(append)
    {
        m_diff.add_diff(_diff);
    }

    return append;
}

//-----------------------------------------------------------------------------

sight::filter::image::min_max_propagation::seeds_t propagator::convert_diff_to_seeds() const
{
    const auto img_out = m_image_out.lock();
    SIGHT_ASSERT(IMAGE_INOUT << " does not exist", img_out);

    const data::image::size_t& img_size = img_out->size();

    sight::filter::image::min_max_propagation::seeds_t seeds;

    const std::size_t nb_elts = m_diff.num_elements();
    for(std::size_t i = 0 ; i < nb_elts ; ++i)
    {
        data::image::index_t index = m_diff.get_element_diff_index(i);
        sight::filter::image::min_max_propagation::coordinates_t coords;
        coords[0] = index % img_size[0];
        coords[1] = (index / img_size[0]) % img_size[1];
        coords[2] = (index / img_size[0]) / img_size[1];

        seeds.push_back(coords);
    }

    return seeds;
}

//-----------------------------------------------------------------------------

service::connections_t propagator::auto_connections() const
{
    return {
        {IMAGE_IN, data::image::MODIFIED_SIG, service::slots::UPDATE},
        {IMAGE_IN, data::image::SLICE_TYPE_MODIFIED_SIG, SET_ORIENTATION_SLOT},
        {IMAGE_IN, data::image::SLICE_INDEX_MODIFIED_SIG, RESET_DRAWING}
    };
}

} // namespace sight::module::filter::image.
