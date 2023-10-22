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

#include "mesh_modifier.hpp"

#include <core/com/signal.hxx>

#include <geometry/data/mesh.hpp>

#include <ui/__/dialog/message.hpp>

namespace tuto13_mesh_generator_cpp
{

static const std::string s_FUNCTOR_CONFIG = "functor";

//-----------------------------------------------------------------------------

void mesh_modifier::configuring(const config_t& _config)
{
    this->initialize();

    const config_t config = _config.get_child("config.<xmlattr>");

    m_functor = config.get<std::string>(s_FUNCTOR_CONFIG);
    SIGHT_ASSERT(
        "Wrong functor name",
        m_functor == "ShakeMeshPoint"
        || m_functor == "ColorizeMeshPoints"
        || m_functor == "ColorizeMeshCells"
        || m_functor == "ComputePointNormals"
        || m_functor == "ComputeCellNormals"
        || m_functor == "ShakePointNormals"
        || m_functor == "ShakeCellNormals"
        || m_functor == "MeshDeformation"
    );
}

//-----------------------------------------------------------------------------

void mesh_modifier::starting()
{
    this->actionServiceStarting();
}

//------------------------------------------------------------------------------

void mesh_modifier::updating()
{
    namespace data     = sight::data;
    namespace geometry = sight::geometry;
    namespace ui       = sight::ui;

    const auto mesh = m_mesh.lock();

    try
    {
        if(m_functor == "ShakeMeshPoint")
        {
            geometry::data::mesh::shakePoint(mesh.get_shared());

            data::mesh::signal_t::sptr sig;
            sig = mesh->signal<data::mesh::signal_t>(data::mesh::VERTEX_MODIFIED_SIG);
            sig->async_emit();
        }
        else if(m_functor == "ColorizeMeshCells")
        {
            geometry::data::mesh::colorizeMeshCells(mesh.get_shared());

            data::mesh::signal_t::sptr sig;
            sig = mesh->signal<data::mesh::signal_t>(data::mesh::CELL_COLORS_MODIFIED_SIG);
            sig->async_emit();
        }
        else if(m_functor == "ColorizeMeshPoints")
        {
            geometry::data::mesh::colorizeMeshPoints(mesh.get_shared());

            data::mesh::signal_t::sptr sig;
            sig = mesh->signal<data::mesh::signal_t>(data::mesh::POINT_COLORS_MODIFIED_SIG);
            sig->async_emit();
        }
        else if(m_functor == "ComputeCellNormals")
        {
            geometry::data::mesh::generateCellNormals(mesh.get_shared());

            data::mesh::signal_t::sptr sig;
            sig = mesh->signal<data::mesh::signal_t>(
                data::mesh::CELL_NORMALS_MODIFIED_SIG
            );
            sig->async_emit();
        }
        else if(m_functor == "ComputePointNormals")
        {
            geometry::data::mesh::generatePointNormals(mesh.get_shared());

            data::mesh::signal_t::sptr sig;
            sig = mesh->signal<data::mesh::signal_t>(
                data::mesh::POINT_NORMALS_MODIFIED_SIG
            );
            sig->async_emit();
        }
        else if(m_functor == "ShakeCellNormals")
        {
            geometry::data::mesh::shakeCellNormals(mesh.get_shared());

            data::mesh::signal_t::sptr sig;
            sig = mesh->signal<data::mesh::signal_t>(data::mesh::CELL_NORMALS_MODIFIED_SIG);
            sig->async_emit();
        }
        else if(m_functor == "ShakePointNormals")
        {
            geometry::data::mesh::shakePointNormals(mesh.get_shared());

            data::mesh::signal_t::sptr sig;
            sig = mesh->signal<data::mesh::signal_t>(
                data::mesh::POINT_NORMALS_MODIFIED_SIG
            );
            sig->async_emit();
        }
        else if(m_functor == "MeshDeformation")
        {
            m_animator.computeDeformation(mesh.get_shared(), 100, 50);
            const auto sig = mesh->signal<data::object::modified_signal_t>(data::object::MODIFIED_SIG);
            {
                sight::core::com::connection::blocker block(sig->get_connection(slot(sight::service::slots::UPDATE)));
                sig->async_emit();
            }
        }
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during generating : " << e.what();

        ui::dialog::message::show(
            "Warning",
            ss.str(),
            ui::dialog::message::WARNING
        );
    }
}

//-----------------------------------------------------------------------------

void mesh_modifier::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

} // namespace tuto13_mesh_generator_cpp.
