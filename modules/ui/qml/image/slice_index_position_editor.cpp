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
    new_slot(UPDATE_SLICE_INDEX_SLOT, &slice_index_position_editor::updateSliceIndex, this);
    new_slot(UPDATE_SLICE_TYPE_SLOT, &slice_index_position_editor::updateSliceType, this);
}

//------------------------------------------------------------------------------

slice_index_position_editor::~slice_index_position_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void slice_index_position_editor::starting()
{
    {
        auto image = m_image.lock();
        m_axialIndex = imHelper::get_slice_index(*image, orientation_t::AXIAL).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::AXIAL, m_axialIndex);

        m_frontalIndex = imHelper::get_slice_index(*image, orientation_t::FRONTAL).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::FRONTAL, m_frontalIndex);

        m_sagittalIndex = imHelper::get_slice_index(*image, orientation_t::SAGITTAL).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::SAGITTAL, m_sagittalIndex);
    }
    this->updateSliceTypeFromImg(m_orientation);

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
        m_axialIndex = imHelper::get_slice_index(*image, orientation_t::AXIAL).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::AXIAL, m_axialIndex);

        m_frontalIndex = imHelper::get_slice_index(*image, orientation_t::FRONTAL).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::FRONTAL, m_frontalIndex);

        m_sagittalIndex = imHelper::get_slice_index(*image, orientation_t::SAGITTAL).value_or(0);
        imHelper::set_slice_index(*image, orientation_t::SAGITTAL, m_sagittalIndex);
    }
    this->updateSliceIndexFromImg();
}

//-----------------------------------------------------------------------------

void slice_index_position_editor::updateSliceIndex(int _axial, int _frontal, int _sagittal)
{
    m_axialIndex    = static_cast<std::int64_t>(_axial);
    m_frontalIndex  = static_cast<std::int64_t>(_frontal);
    m_sagittalIndex = static_cast<std::int64_t>(_sagittal);
    {
        auto image = m_image.lock();
        imHelper::set_slice_index(*image, orientation_t::AXIAL, m_axialIndex);
        imHelper::set_slice_index(*image, orientation_t::FRONTAL, m_frontalIndex);
        imHelper::set_slice_index(*image, orientation_t::SAGITTAL, m_sagittalIndex);
    }

    this->updateSliceIndexFromImg();
}

//-----------------------------------------------------------------------------

void slice_index_position_editor::set_slice_index(int _slice_index)
{
    m_orientation = static_cast<orientation_t>(_slice_index);
    this->updateSliceTypeFromImg(m_orientation);
}

//-----------------------------------------------------------------------------

int slice_index_position_editor::get_slice_index() const
{
    return static_cast<int>(m_orientation);
}

//-----------------------------------------------------------------------------

void slice_index_position_editor::updateSliceType(int _from, int _to)
{
    if(_to == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(_from);
    }
    else if(_from == static_cast<int>(m_orientation))
    {
        m_orientation = static_cast<orientation_t>(_to);
    }

    this->updateSliceTypeFromImg(m_orientation);
}

//------------------------------------------------------------------------------

void slice_index_position_editor::updateSliceIndexFromImg()
{
    auto image = m_image.lock();

    if(imHelper::check_image_validity(image.get_shared()))
    {
        // Get Index
        const auto index = imHelper::get_slice_index(*image, m_orientation);

        // Update QSlider
        int max = 0;
        if(image->numDimensions() > m_orientation)
        {
            max = static_cast<int>(image->size()[m_orientation] - 1);
        }

        this->setSliceRange(0, max);
        this->setSliceValue(static_cast<int>(index.value_or(0)));
    }
}

//------------------------------------------------------------------------------

void slice_index_position_editor::updateSliceTypeFromImg(const orientation_t& _type)
{
    // Update Type Choice
    this->set_slice_type(static_cast<int>(_type));
    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

void slice_index_position_editor::onSliceIndex(int _index)
{
    auto image = m_image.lock();

    imHelper::set_slice_index(*image, m_orientation, _index);

    auto sig = image->signal<data::image::SliceIndexModifiedSignalType>(
        data::image::SLICE_INDEX_MODIFIED_SIG
    );
    core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_SLICE_INDEX_SLOT)));
    sig->async_emit(
        static_cast<int>(m_axialIndex),
        static_cast<int>(m_frontalIndex),
        static_cast<int>(m_sagittalIndex)
    );
}

//------------------------------------------------------------------------------

void slice_index_position_editor::onSliceType(int _type)
{
    auto type = static_cast<orientation_t>(_type);
    SIGHT_ASSERT(
        "Bad slice type " << type,
        type == orientation_t::X_AXIS
        || type == orientation_t::Y_AXIS
        || type == orientation_t::Z_AXIS
    );

    const int old_type = static_cast<int>(m_orientation);
    // Change slice type
    m_orientation = type;

    // Fire the signal
    auto image = m_image.lock();
    auto sig   = image->signal<data::image::SliceTypeModifiedSignalType>(
        data::image::SLICE_TYPE_MODIFIED_SIG
    );
    {
        core::com::connection::blocker block(sig->get_connection(this->slot(UPDATE_SLICE_TYPE_SLOT)));
        sig->async_emit(old_type, _type);
    }
    this->updateSliceIndexFromImg();
}

//------------------------------------------------------------------------------

service::connections_t slice_index_position_editor::auto_connections() const
{
    connections_t connections;

    connections.push(s_IMAGE_INOUT, data::image::MODIFIED_SIG, service::slots::UPDATE);
    connections.push(s_IMAGE_INOUT, data::image::SLICE_INDEX_MODIFIED_SIG, UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, data::image::SLICE_TYPE_MODIFIED_SIG, UPDATE_SLICE_TYPE_SLOT);
    connections.push(s_IMAGE_INOUT, data::image::BUFFER_MODIFIED_SIG, service::slots::UPDATE);

    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::image
