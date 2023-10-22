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

    int representation_mode = m_material->getRepresentationMode();
    int shading_mode        = m_material->getShadingMode();
    int normal              = m_material->getOptionsMode();
    Q_EMIT materialChanged(representation_mode, shading_mode, normal);
}

//------------------------------------------------------------------------------

void representation_editor::onChangeRepresentation(int _id)
{
    data::material::representation_t selected_mode = data::material::SURFACE;

    switch(_id)
    {
        case 1:
            selected_mode = data::material::SURFACE;
            break;

        case 2:
            selected_mode = data::material::POINT;
            break;

        case 3:
            selected_mode = data::material::WIREFRAME;
            break;

        case 4:
            selected_mode = data::material::EDGE;
            break;

        default:
            selected_mode = data::material::SURFACE;
    }

    m_material->setRepresentationMode(selected_mode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void representation_editor::onChangeShading(int _id)
{
    data::material::shading_t selected_mode = data::material::PHONG;

    switch(_id)
    {
        case 0:
            selected_mode = data::material::AMBIENT;
            break;

        case 1:
            selected_mode = data::material::FLAT;
            break;

        case 2:
            selected_mode = data::material::PHONG;
            break;

        default:
            selected_mode = data::material::PHONG;
    }

    m_material->setShadingMode(selected_mode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void representation_editor::onShowNormals(int _state)
{
    switch(_state)
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

    data::object::modified_signal_t::sptr sig;
    sig = reconstruction->getMaterial()->signal<data::object::modified_signal_t>(
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
