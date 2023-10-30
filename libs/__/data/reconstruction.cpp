/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/reconstruction.hpp"

#include "data/exception.hpp"
#include "data/image.hpp"
#include "data/material.hpp"
#include "data/mesh.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

SIGHT_REGISTER_DATA(sight::data::reconstruction);
namespace sight::data
{

const double reconstruction::NO_COMPUTED_MASK_VOLUME = -1.;

const core::com::signals::key_t reconstruction::MESH_CHANGED_SIG        = "meshModified";
const core::com::signals::key_t reconstruction::VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

reconstruction::reconstruction() :

    m_material(data::factory::make<data::material>()),
    m_computed_mask_volume(reconstruction::NO_COMPUTED_MASK_VOLUME),
    m_sig_mesh_changed(std::make_shared<mesh_changed_signal_t>()),
    m_sig_visibility_modified(std::make_shared<visibility_modified_signal_t>())
{
    m_signals(MESH_CHANGED_SIG, m_sig_mesh_changed)
        (VISIBILITY_MODIFIED_SIG, m_sig_visibility_modified);
}

//------------------------------------------------------------------------------

void reconstruction::shallow_copy(const object::csptr& _source)
{
    const auto& other = std::dynamic_pointer_cast<const reconstruction>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_b_is_visible     = other->m_b_is_visible;
    m_s_organ_name     = other->m_s_organ_name;
    m_s_structure_type = other->m_s_structure_type;

    m_material = other->m_material;
    m_image    = other->m_image;
    m_mesh     = other->m_mesh;

    m_computed_mask_volume = other->m_computed_mask_volume;

    base_class_t::shallow_copy(other);
}

//------------------------------------------------------------------------------

void reconstruction::deep_copy(const object::csptr& _source, const std::unique_ptr<deep_copy_cache_t>& _cache)
{
    const auto& other = std::dynamic_pointer_cast<const reconstruction>(_source);

    SIGHT_THROW_EXCEPTION_IF(
        exception(
            "Unable to copy " + (_source ? _source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_b_is_visible     = other->m_b_is_visible;
    m_s_organ_name     = other->m_s_organ_name;
    m_s_structure_type = other->m_s_structure_type;

    m_material = data::object::copy(other->m_material, _cache);
    m_image    = data::object::copy(other->m_image, _cache);
    m_mesh     = data::object::copy(other->m_mesh, _cache);

    m_computed_mask_volume = other->m_computed_mask_volume;

    base_class_t::deep_copy(other, _cache);
}

//------------------------------------------------------------------------------

bool reconstruction::operator==(const reconstruction& _other) const noexcept
{
    if(m_b_is_visible != _other.m_b_is_visible
       || m_s_organ_name != _other.m_s_organ_name
       || m_s_structure_type != _other.m_s_structure_type
       || m_computed_mask_volume != _other.m_computed_mask_volume
       || !core::tools::is_equal(m_material, _other.m_material)
       || !core::tools::is_equal(m_image, _other.m_image)
       || !core::tools::is_equal(m_mesh, _other.m_mesh))
    {
        return false;
    }

    // Super class last
    return base_class_t::operator==(_other);
}

//------------------------------------------------------------------------------

bool reconstruction::operator!=(const reconstruction& _other) const noexcept
{
    return !(*this == _other);
}

} // end namespace sight::data
