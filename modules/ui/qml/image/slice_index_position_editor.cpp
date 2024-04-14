/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "slice_index_position_editor.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/com/slot.hxx>
#include <core/com/slots.hxx>

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>

#include <array>
#include <functional>

namespace sight::module::ui::qml::image
{

namespace imHelper = data::helper::medical_image;

static const core::com::slots::key_t UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const core::com::slots::key_t UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

//------------------------------------------------------------------------------

slice_index_position_editor::slice_index_position_editor() noexcept
{
    new_slot(UPDATE_SLICE_INDEX_SLOT, &slice_index_position_editor::update_slice_index, this);
    new_slot(UPDATE_SLICE_TYPE_SLOT, &slice_index_position_editor::update_slice_type, this);
}

//------------------------------------------------------------------------------

slice_index_position_editor::~slice_index_position_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void slice_index_position_editor::starting()
{
    {
        auto image = m_image.lock();
        m_axial_index = imHelper::get_slice_index(*image, orientation_t::axial).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::axial, m_axial_index);

        m_frontal_index = imHelper::get_slice_index(*image, orientation_t::frontal).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::frontal, m_frontal_index);

        m_sagittal_index = imHelper::get_slice_index(*image, orientation_t::sagittal).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::sagittal, m_sagittal_index);
    }
    this->update_slice_type_from_img(m_orientation);

    this->updating();
}

//------------------------------------------------------------------------------

void slice_index_position_editor::stopping()
{
}

//------------------------------------------------------------------------------

void slice_index_position_editor::configuring()
{
}

//------------------------------------------------------------------------------

void slice_index_position_editor::updating()
{
    {
        namespace imHelper = data::helper::medical_image;
        auto image = m_image.lock();
        m_axial_index = imHelper::get_slice_index(*image, orientation_t::axial).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::axial, m_axial_index);

        m_frontal_index = imHelper::get_slice_index(*image, orientation_t::frontal).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::frontal, m_frontal_index);

        m_sagittal_index = imHelper::get_slice_index(*image, orientation_t::sagittal).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::sagittal, m_sagittal_index);
    }
    this->update_slice_index_from_img();
}

//-----------------------------------------------------------------------------

void slice_index_position_editor::update_slice_index(int _axial, int _frontal, int _sagittal)
{
    m_axial_index    = static_cast<std::int64_t>(_axial);
    m_frontal_index  = static_cast<std::int64_t>(_frontal);
    m_sagittal_index = static_cast<std::int64_t>(_sagittal);
    {
        auto image = m_image.lock();
        imHelper::set_slice_index(*image, orientation_t::axial, m_axial_index);
        imHelper::set_slice_index(*image, orientation_t::frontal, m_frontal_index);
        imHelper::set_slice_index(*image, orientation_t::sagittal, m_sagittal_index);
    }

    this->update_slice_index_from_img();
}

//-----------------------------------------------------------------------------

void slice_index_position_editor::set_slice_index(int _slice_index)
{
    m_orientation = static_cast<orientation_t>(_slice_index);
    this->update_slice_type_from_img(m_orientation);
}

//-----------------------------------------------------------------------------

int slice_index_position_editor::get_slice_index() const
{
    return static_cast<int>(m_orientation);
}

//-----------------------------------------------------------------------------

void slice_index_position_editor::update_slice_type(int _from, int _to)
{
    if(_to == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(_from);
    }
    else if(_from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(_to);
    }

    this->update_slice_type_from_img(m_orientation);
}

//------------------------------------------------------------------------------

void slice_index_position_editor::update_slice_index_from_img()
{
    auto image = m_image.lock();

    if(imHelper::check_image_validity(image.get_shared()))
    {
        // Get Index
        const auto index = imHelper::get_slice_index(*image, m_orientation);

        // Update QSlider
        int max = 0;
        if(image->num_dimensions() > m_orientation)
        {
            max = static_cast<int>(image->size()[m_orientation] - 1);
        }

        this->set_slice_range(0, max);
        this->set_slice_value(static_cast<int>(index.value_or(0)));
    }
}

//------------------------------------------------------------------------------

void slice_index_position_editor::update_slice_type_from_img(const orientation_t& _type)
{
    // Update Type Choice
    this->set_slice_type(static_cast<int>(_type));
    this->update_slice_index_from_img();
}

//------------------------------------------------------------------------------

void slice_index_position_editor::on_slice_index(int _index)
{
    auto image = m_image.lock();

    imHelper::set_slice_index(*image, m_orientation, _index);

    auto sig = image->signal<data::image::slice_index_modified_signal_t>(
        data::image::SLICE_INDEX_MODIFIED_SIG
    );
    core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_SLICE_INDEX_SLOT)));
    sig->async_emit(
        static_cast<int>(m_axial_index),
        static_cast<int>(m_frontal_index),
        static_cast<int>(m_sagittal_index)
    );
}

//------------------------------------------------------------------------------

void slice_index_position_editor::on_slice_type(int _type)
{
    auto type = static_cast<orientation_t>(_type);
    SIGHT_ASSERT(
        "Bad slice type " << type,
        type == orientation_t::x_axis
        || type == orientation_t::y_axis
        || type == orientation_t::z_axis
    );

    const int old_type = static_cast<int>(m_orientation);
    // Change slice type
    m_orientation = type;

    // Fire the signal
    auto image = m_image.lock();
    auto sig   = image->signal<data::image::slice_type_modified_signal_t>(
        data::image::SLICE_TYPE_MODIFIED_SIG
    );
    {
        core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_SLICE_TYPE_SLOT)));
        sig->async_emit(old_type, _type);
    }
    this->update_slice_index_from_img();
}

//------------------------------------------------------------------------------

service::connections_t slice_index_position_editor::auto_connections() const
{
    connections_t connections;

    connections.push(IMAGE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(IMAGE_INOUT, data::image::SLICE_INDEX_MODIFIED_SIG, UPDATE_SLICE_INDEX_SLOT);
    connections.push(IMAGE_INOUT, data::image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT);
    connections.push(IMAGE_INOUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::image
