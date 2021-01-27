/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

fwDataRegisterMacro( ::sight::data::Reconstruction );
namespace sight::data
{

const double Reconstruction::s_NO_COMPUTED_MASK_VOLUME = -1.;

const core::com::Signals::SignalKeyType Reconstruction::s_MESH_CHANGED_SIG        = "meshModified";
const core::com::Signals::SignalKeyType Reconstruction::s_VISIBILITY_MODIFIED_SIG = "visibilityModified";

//------------------------------------------------------------------------------

Reconstruction::Reconstruction(data::Object::Key) :
    m_bIsVisible(false),
    m_sOrganName(""),
    m_sStructureType(""),
    m_material( data::factory::New< data::Material>() ),
    m_computedMaskVolume(Reconstruction::s_NO_COMPUTED_MASK_VOLUME)
{
    m_sigMeshChanged        = MeshChangedSignalType::New();
    m_sigVisibilityModified = VisibilityModifiedSignalType::New();
    m_signals(s_MESH_CHANGED_SIG, m_sigMeshChanged)
        (s_VISIBILITY_MODIFIED_SIG, m_sigVisibilityModified);
}

//------------------------------------------------------------------------------

Reconstruction::~Reconstruction()
{
}

//------------------------------------------------------------------------------

void Reconstruction::shallowCopy(const Object::csptr& _source )
{
    Reconstruction::csptr other = Reconstruction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldShallowCopy( _source );

    m_bIsVisible     = other->m_bIsVisible;
    m_sOrganName     = other->m_sOrganName;
    m_sStructureType = other->m_sStructureType;

    m_material = other->m_material;
    m_image    = other->m_image;
    m_mesh     = other->m_mesh;

    m_computedMaskVolume = other->m_computedMaskVolume;
}

//------------------------------------------------------------------------------

void Reconstruction::cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache)
{
    Reconstruction::csptr other = Reconstruction::dynamicConstCast(_source);
    FW_RAISE_EXCEPTION_IF( data::Exception(
                               "Unable to copy" + (_source ? _source->getClassname() : std::string("<NULL>"))
                               + " to " + this->getClassname()), !bool(other) );
    this->fieldDeepCopy( _source, cache );

    m_bIsVisible     = other->m_bIsVisible;
    m_sOrganName     = other->m_sOrganName;
    m_sStructureType = other->m_sStructureType;

    m_material = data::Object::copy(other->m_material, cache);
    m_image    = data::Object::copy(other->m_image, cache);
    m_mesh     = data::Object::copy(other->m_mesh, cache);

    m_computedMaskVolume = other->m_computedMaskVolume;
}

//------------------------------------------------------------------------------

} // end namespace sight::data
