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
        SIGHT_ASSERT("'" << RECONSTRUCTION_INOUT << "' must be set as 'inout'", reconstruction);
        m_material = reconstruction->get_material();
    }

    int representation_mode = m_material->get_representation_mode();
    int shading_mode        = static_cast<int>(m_material->get_shading_mode());
    int normal              = m_material->get_options_mode();
    Q_EMIT material_changed(representation_mode, shading_mode, normal);
}

//------------------------------------------------------------------------------

void representation_editor::on_change_representation(int _id)
{
    data::material::representation_t selected_mode = data::material::surface;

    switch(_id)
    {
        case 1:
            selected_mode = data::material::surface;
            break;

        case 2:
            selected_mode = data::material::point;
            break;

        case 3:
            selected_mode = data::material::wireframe;
            break;

        case 4:
            selected_mode = data::material::edge;
            break;

        default:
            selected_mode = data::material::surface;
    }

    m_material->set_representation_mode(selected_mode);
    this->notify_material();
}

//------------------------------------------------------------------------------

void representation_editor::on_change_shading(int _id)
{
    data::material::shading_t selected_mode = data::material::shading_t::phong;

    switch(_id)
    {
        case 0:
            selected_mode = data::material::shading_t::ambient;
            break;

        case 1:
            selected_mode = data::material::shading_t::flat;
            break;

        case 2:
            selected_mode = data::material::shading_t::phong;
            break;

        default:
            selected_mode = data::material::shading_t::phong;
    }

    m_material->set_shading_mode(selected_mode);
    this->notify_material();
}

//------------------------------------------------------------------------------

void representation_editor::on_show_normals(int _state)
{
    switch(_state)
    {
        case 1:
            m_material->set_options_mode(data::material::standard);
            break;

        case 2:
            m_material->set_options_mode(data::material::normals);
            break;

        case 3:
            m_material->set_options_mode(data::material::cells_normals);
            break;

        default:
            m_material->set_options_mode(data::material::standard);
    }

    this->notify_material();
}

//------------------------------------------------------------------------------

void representation_editor::notify_material()
{
    auto reconstruction = m_rec.lock();
    SIGHT_ASSERT("No Reconstruction!", reconstruction);

    data::object::modified_signal_t::sptr sig;
    sig = reconstruction->get_material()->signal<data::object::modified_signal_t>(
        data::object::MODIFIED_SIG
    );
    sig->async_emit();
}

//------------------------------------------------------------------------------

service::connections_t representation_editor::auto_connections() const
{
    connections_t connections;
    connections.push(RECONSTRUCTION_INOUT, data::object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::reconstruction
