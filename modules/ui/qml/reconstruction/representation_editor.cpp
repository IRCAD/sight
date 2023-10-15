/************************************************************************
 *
 * Copyright (C) 2018-2023 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "representation_editor.hpp"

#include <core/com/signal.hxx>

#include <data/material.hpp>
#include <data/mesh.hpp>

namespace sight::module::ui::qml::reconstruction
{

//------------------------------------------------------------------------------

representation_editor::representation_editor() noexcept =
    default;

//------------------------------------------------------------------------------

representation_editor::~representation_editor() noexcept =
    default;

//------------------------------------------------------------------------------

void representation_editor::configuring()
{
}

//------------------------------------------------------------------------------

void representation_editor::starting()
{
    this->editor::starting();
}

//------------------------------------------------------------------------------

void representation_editor::stopping()
{
    this->editor::stopping();
}

//------------------------------------------------------------------------------

void representation_editor::updating()
{
    {
        auto reconstruction = m_rec.lock();
        SIGHT_ASSERT("'" << s_RECONSTRUCTION_INOUT << "' must be set as 'inout'", reconstruction);
        m_material = reconstruction->getMaterial();
    }

    int representationMode = m_material->getRepresentationMode();
    int shadingMode        = m_material->getShadingMode();
    int normal             = m_material->getOptionsMode();
    Q_EMIT materialChanged(representationMode, shadingMode, normal);
}

//------------------------------------------------------------------------------

void representation_editor::onChangeRepresentation(int id)
{
    data::material::RepresentationType selectedMode = data::material::SURFACE;

    switch(id)
    {
        case 1:
            selectedMode = data::material::SURFACE;
            break;

        case 2:
            selectedMode = data::material::POINT;
            break;

        case 3:
            selectedMode = data::material::WIREFRAME;
            break;

        case 4:
            selectedMode = data::material::EDGE;
            break;

        default:
            selectedMode = data::material::SURFACE;
    }

    m_material->setRepresentationMode(selectedMode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void representation_editor::onChangeShading(int id)
{
    data::material::ShadingType selectedMode = data::material::PHONG;

    switch(id)
    {
        case 0:
            selectedMode = data::material::AMBIENT;
            break;

        case 1:
            selectedMode = data::material::FLAT;
            break;

        case 2:
            selectedMode = data::material::PHONG;
            break;

        default:
            selectedMode = data::material::PHONG;
    }

    m_material->setShadingMode(selectedMode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void representation_editor::onShowNormals(int state)
{
    switch(state)
    {
        case 1:
            m_material->setOptionsMode(data::material::STANDARD);
            break;

        case 2:
            m_material->setOptionsMode(data::material::NORMALS);
            break;

        case 3:
            m_material->setOptionsMode(data::material::CELLS_NORMALS);
            break;

        default:
            m_material->setOptionsMode(data::material::STANDARD);
    }

    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void representation_editor::notifyMaterial()
{
    auto reconstruction = m_rec.lock();
    SIGHT_ASSERT("No Reconstruction!", reconstruction);

    data::object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal<data::object::ModifiedSignalType>(
        data::object::MODIFIED_SIG
    );
    sig->async_emit();
}

//------------------------------------------------------------------------------

service::connections_t representation_editor::auto_connections() const
{
    connections_t connections;
    connections.push(s_RECONSTRUCTION_INOUT, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::reconstruction
