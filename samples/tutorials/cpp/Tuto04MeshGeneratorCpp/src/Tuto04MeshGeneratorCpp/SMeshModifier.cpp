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

#include "Tuto04MeshGeneratorCpp/SMeshModifier.hpp"

#include <core/com/Signal.hxx>

#include <data/Mesh.hpp>
#include <data/tools/Mesh.hpp>

#include <gui/dialog/MessageDialog.hpp>

#include <services/macros.hpp>

namespace Tuto04MeshGeneratorCpp
{

fwServicesRegisterMacro( ::sight::gui::IActionSrv, ::Tuto04MeshGeneratorCpp::SMeshModifier, ::sight::data::Mesh )

static const services::IService::KeyType s_MESH_INOUT = "mesh";

static const std::string s_FUNCTOR_CONFIG = "functor";

//-----------------------------------------------------------------------------

SMeshModifier::SMeshModifier() noexcept
{
}

//-----------------------------------------------------------------------------

SMeshModifier::~SMeshModifier() noexcept
{
}

//-----------------------------------------------------------------------------

void SMeshModifier::configuring()
{
    this->initialize();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    m_functor = config.get<std::string>(s_FUNCTOR_CONFIG);
    OSLM_ASSERT("Wrong functor name",
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

void SMeshModifier::starting()
{
    this->actionServiceStarting();
    data::tools::Mesh::initRand();
}

//------------------------------------------------------------------------------

void SMeshModifier::updating()
{
    const auto mesh = this->getLockedInOut< data::Mesh >(s_MESH_INOUT);

    try
    {
        if(m_functor == "ShakeMeshPoint")
        {
            data::tools::Mesh::shakePoint(mesh.get_shared());

            data::Mesh::VertexModifiedSignalType::sptr sig;
            sig = mesh->signal< data::Mesh::VertexModifiedSignalType >(data::Mesh::s_VERTEX_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ColorizeMeshCells")
        {
            data::tools::Mesh::colorizeMeshCells(mesh.get_shared());

            data::Mesh::CellColorsModifiedSignalType::sptr sig;
            sig = mesh->signal< data::Mesh::CellColorsModifiedSignalType >(
                data::Mesh::s_CELL_COLORS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ColorizeMeshPoints")
        {
            data::tools::Mesh::colorizeMeshPoints(mesh.get_shared());

            data::Mesh::PointColorsModifiedSignalType::sptr sig;
            sig = mesh->signal< data::Mesh::PointColorsModifiedSignalType >(
                data::Mesh::s_POINT_COLORS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ComputeCellNormals")
        {
            data::tools::Mesh::generateCellNormals(mesh.get_shared());

            data::Mesh::CellNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< data::Mesh::CellNormalsModifiedSignalType >(
                data::Mesh::s_CELL_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ComputePointNormals")
        {
            data::tools::Mesh::generatePointNormals(mesh.get_shared());

            data::Mesh::PointNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< data::Mesh::PointNormalsModifiedSignalType >(
                data::Mesh::s_POINT_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ShakeCellNormals")
        {
            data::tools::Mesh::shakeCellNormals(mesh.get_shared());

            data::Mesh::CellNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< data::Mesh::CellNormalsModifiedSignalType >(
                data::Mesh::s_CELL_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ShakePointNormals")
        {
            data::tools::Mesh::shakePointNormals(mesh.get_shared());

            data::Mesh::PointNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< data::Mesh::PointNormalsModifiedSignalType >(
                data::Mesh::s_POINT_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "MeshDeformation")
        {
            m_animator.computeDeformation(mesh.get_shared(), 100, 50);
            const auto sig = mesh->signal< data::Object::ModifiedSignalType >(data::Object::s_MODIFIED_SIG);
            {
                core::com::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
    catch (const std::exception& _e)
    {
        std::stringstream ss;
        ss << "Warning during generating : " << _e.what();

        gui::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            gui::dialog::IMessageDialog::WARNING);
    }
}

//-----------------------------------------------------------------------------

void SMeshModifier::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

} // namespace Tuto04MeshGeneratorCpp.
