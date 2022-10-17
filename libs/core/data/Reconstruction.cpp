/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <core/com/Signal.hpp>
#include <core/com/Signal.hxx>
#include <core/com/Signals.hpp>

SIGHT_REGISTER_DATA(sight::data::Reconstruction);
namespace sight::data
{

const double Reconstruction::s_NO_COMPUTED_MASK_VOLUME = -1.;

const core::com::Signals::SignalKeyType Reconstruction::s_MESH_CHANGED_SIG        = "meshModified";
const core::com::Signals::SignalKeyType Reconstruction::s_VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

Reconstruction::Reconstruction(data::Object::Key /*unused*/) :

    m_material(data::factory::New<data::Material>()),
    m_computedMaskVolume(Reconstruction::s_NO_COMPUTED_MASK_VOLUME),
    m_sigMeshChanged(MeshChangedSignalType::New()),
    m_sigVisibilityModified(VisibilityModifiedSignalType::New())
{
    m_signals(s_MESH_CHANGED_SIG, m_sigMeshChanged)
        (s_VISIBILITY_MODIFIED_SIG, m_sigVisibilityModified);
}

//------------------------------------------------------------------------------

void Reconstruction::shallowCopy(const Object::csptr& source)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
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

    BaseClass::shallowCopy(other);
}

//------------------------------------------------------------------------------

void Reconstruction::deepCopy(const Object::csptr& source, const std::unique_ptr<DeepCopyCacheType>& cache)
{
    const auto& other = dynamicConstCast(source);

    SIGHT_THROW_EXCEPTION_IF(
        Exception(
            "Unable to copy " + (source ? source->getClassname() : std::string("<NULL>"))
            + " to " + getClassname()
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

    BaseClass::deepCopy(other, cache);
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
    return BaseClass::operator==(other);
}

//------------------------------------------------------------------------------

bool Reconstruction::operator!=(const Reconstruction& other) const noexcept
{
    return !(*this == other);
}

} // end namespace sight::data
