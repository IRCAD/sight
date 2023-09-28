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

#include "data/Reconstruction.hpp"

#include "data/Exception.hpp"
#include "data/Image.hpp"
#include "data/Material.hpp"
#include "data/Mesh.hpp"
#include "data/registry/macros.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>

SIGHT_REGISTER_DATA(sight::data::Reconstruction);
namespace sight::data
{

const double Reconstruction::s_NO_COMPUTED_MASK_VOLUME = -1.;

const core::com::signals::key_t Reconstruction::MESH_CHANGED_SIG        = "meshModified";
const core::com::signals::key_t Reconstruction::VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

Reconstruction::Reconstruction() :

    m_material(data::factory::make<data::Material>()),
    m_computedMaskVolume(Reconstruction::s_NO_COMPUTED_MASK_VOLUME),
    m_sigMeshChanged(std::make_shared<MeshChangedSignalType>()),
    m_sigVisibilityModified(std::make_shared<VisibilityModifiedSignalType>())
{
    m_signals(MESH_CHANGED_SIG, m_sigMeshChanged)
        (VISIBILITY_MODIFIED_SIG, m_sigVisibilityModified);
}

//------------------------------------------------------------------------------

void Reconstruction::shallow_copy(const Object::csptr& source)
{
    const auto& other = std::dynamic_pointer_cast<const Reconstruction>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_bIsVisible     = other->m_bIsVisible;
    m_sOrganName     = other->m_sOrganName;
    m_sStructureType = other->m_sStructureType;

    m_material = other->m_material;
    m_image    = other->m_image;
    m_mesh     = other->m_mesh;

    m_computedMaskVolume = other->m_computedMaskVolume;

    base_class::shallow_copy(other);
}

//------------------------------------------------------------------------------

void Reconstruction::deep_copy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = std::dynamic_pointer_cast<const Reconstruction>(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->get_classname() : std::string("<NULL>"))
            + " to " + get_classname()
        ),
        !bool(other)
    );

    m_bIsVisible     = other->m_bIsVisible;
    m_sOrganName     = other->m_sOrganName;
    m_sStructureType = other->m_sStructureType;

    m_material = data::Object::copy(other->m_material, cache);
    m_image    = data::Object::copy(other->m_image, cache);
    m_mesh     = data::Object::copy(other->m_mesh, cache);

    m_computedMaskVolume = other->m_computedMaskVolume;

    base_class::deep_copy(other, cache);
}

//------------------------------------------------------------------------------

bool Reconstruction::operator==(const Reconstruction& other) const noexcept
{
    if(m_bIsVisible != other.m_bIsVisible
       || m_sOrganName != other.m_sOrganName
       || m_sStructureType != other.m_sStructureType
       || m_computedMaskVolume != other.m_computedMaskVolume
       || !core::tools::is_equal(m_material, other.m_material)
       || !core::tools::is_equal(m_image, other.m_image)
       || !core::tools::is_equal(m_mesh, other.m_mesh))
    {
        return false;
    }

    // Super class last
    return base_class::operator==(other);
}

//------------------------------------------------------------------------------

bool Reconstruction::operator!=(const Reconstruction& other) const noexcept
{
    return !(*this == other);
}

} // end namespace sight::data
