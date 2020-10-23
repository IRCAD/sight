/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "Tuto14MeshGenerator/SMeshModifier.hpp"

#include <fwCom/Signal.hxx>

#include <fwData/Mesh.hpp>

#include <fwDataTools/Mesh.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include <fwServices/macros.hpp>

namespace Tuto14MeshGenerator
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::Tuto14MeshGenerator::SMeshModifier, ::fwData::Mesh )

static const ::fwServices::IService::KeyType s_MESH_INOUT = "mesh";

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
    ::fwDataTools::Mesh::initRand();
}

//------------------------------------------------------------------------------

void SMeshModifier::updating()
{
    const auto mesh = this->getLockedInOut< ::fwData::Mesh >(s_MESH_INOUT);

    try
    {
        if(m_functor == "ShakeMeshPoint")
        {
            ::fwDataTools::Mesh::shakePoint(mesh.get_shared());

            ::fwData::Mesh::VertexModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::VertexModifiedSignalType >(::fwData::Mesh::s_VERTEX_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ColorizeMeshCells")
        {
            ::fwDataTools::Mesh::colorizeMeshCells(mesh.get_shared());

            ::fwData::Mesh::CellColorsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::CellColorsModifiedSignalType >(
                ::fwData::Mesh::s_CELL_COLORS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ColorizeMeshPoints")
        {
            ::fwDataTools::Mesh::colorizeMeshPoints(mesh.get_shared());

            ::fwData::Mesh::PointColorsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::PointColorsModifiedSignalType >(
                ::fwData::Mesh::s_POINT_COLORS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ComputeCellNormals")
        {
            ::fwDataTools::Mesh::generateCellNormals(mesh.get_shared());

            ::fwData::Mesh::CellNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::CellNormalsModifiedSignalType >(
                ::fwData::Mesh::s_CELL_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ComputePointNormals")
        {
            ::fwDataTools::Mesh::generatePointNormals(mesh.get_shared());

            ::fwData::Mesh::PointNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::PointNormalsModifiedSignalType >(
                ::fwData::Mesh::s_POINT_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ShakeCellNormals")
        {
            ::fwDataTools::Mesh::shakeCellNormals(mesh.get_shared());

            ::fwData::Mesh::CellNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::CellNormalsModifiedSignalType >(
                ::fwData::Mesh::s_CELL_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "ShakePointNormals")
        {
            ::fwDataTools::Mesh::shakePointNormals(mesh.get_shared());

            ::fwData::Mesh::PointNormalsModifiedSignalType::sptr sig;
            sig = mesh->signal< ::fwData::Mesh::PointNormalsModifiedSignalType >(
                ::fwData::Mesh::s_POINT_NORMALS_MODIFIED_SIG);
            sig->asyncEmit();
        }
        else if(m_functor == "MeshDeformation")
        {
            m_animator.computeDeformation(mesh.get_shared(), 100, 50);
            const auto sig = mesh->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
            {
                ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
                sig->asyncEmit();
            }
        }
    }
    catch (const std::exception& _e)
    {
        std::stringstream ss;
        ss << "Warning during generating : " << _e.what();

        ::fwGui::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            ::fwGui::dialog::IMessageDialog::WARNING);
    }
}

//-----------------------------------------------------------------------------

void SMeshModifier::stopping()
{
    this->actionServiceStopping();
}

//-----------------------------------------------------------------------------

} // namespace Tuto14MeshGenerator.
