/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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

#include "SRepresentationEditor.hpp"

#include <core/com/Signal.hxx>

#include <data/Material.hpp>
#include <data/Mesh.hpp>
#include <data/Reconstruction.hpp>

namespace sight::modules::ui::qml::reconstruction
{

static const service::IService::KeyType s_RECONSTRUCTION_INOUT = "reconstruction";

//------------------------------------------------------------------------------

SRepresentationEditor::SRepresentationEditor() noexcept
{
    this->registerObject(s_RECONSTRUCTION_INOUT, AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

SRepresentationEditor::~SRepresentationEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SRepresentationEditor::configuring()
{
}

//------------------------------------------------------------------------------

void SRepresentationEditor::starting()
{
    this->IQmlEditor::starting();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::stopping()
{
    this->IQmlEditor::stopping();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::updating()
{
    data::Reconstruction::sptr reconstruction = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("'" + s_RECONSTRUCTION_INOUT + "' must be set as 'inout'", reconstruction);
    m_material = reconstruction->getMaterial();

    int representationMode = m_material->getRepresentationMode();
    int shadingMode        = m_material->getShadingMode();
    int normal             = m_material->getOptionsMode();
    Q_EMIT materialChanged(representationMode, shadingMode, normal);
}

//------------------------------------------------------------------------------

void SRepresentationEditor::onChangeRepresentation( int id )
{

    data::Material::RepresentationType selectedMode = data::Material::SURFACE;

    switch(id)
    {
        case 1:
        {
            selectedMode = data::Material::SURFACE;
            break;
        }
        case 2:
        {
            selectedMode = data::Material::POINT;
            break;
        }
        case 3:
        {
            selectedMode = data::Material::WIREFRAME;
            break;
        }
        case 4:
        {
            selectedMode = data::Material::EDGE;
            break;
        }
        default:
            selectedMode = data::Material::SURFACE;
    }

    m_material->setRepresentationMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::onChangeShading(  int id )
{
    data::Material::ShadingType selectedMode = data::Material::PHONG;

    switch(id)
    {
        case 0:
        {
            selectedMode = data::Material::AMBIENT;
            break;
        }
        case 1:
        {
            selectedMode = data::Material::FLAT;
            break;
        }
        case 2:
        {
            selectedMode = data::Material::GOURAUD;
            break;
        }
        case 3:
        {
            selectedMode = data::Material::PHONG;
            break;
        }
        default:
            selectedMode = data::Material::PHONG;
    }

    m_material->setShadingMode( selectedMode );
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::onShowNormals(int state )
{
    data::Reconstruction::sptr reconstruction = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("No Reconstruction!", reconstruction);

    switch(state)
    {
        case 1:
            m_material->setOptionsMode( data::Material::STANDARD );
            break;
        case 2:
            m_material->setOptionsMode( data::Material::NORMALS );
            break;
        case 3:
            m_material->setOptionsMode( data::Material::CELLS_NORMALS );
            break;
        default:
            m_material->setOptionsMode( data::Material::STANDARD );
    }

    this->notifyMaterial();

    // In VTK backend the normals is handled by the mesh and not by the material
    auto sig = reconstruction->signal< data::Reconstruction::MeshChangedSignalType >(
        data::Reconstruction::s_MESH_CHANGED_SIG);
    sig->asyncEmit(reconstruction->getMesh());
}

//------------------------------------------------------------------------------

void SRepresentationEditor::notifyMaterial()
{
    data::Reconstruction::sptr reconstruction = this->getInOut< data::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("No Reconstruction!", reconstruction);

    data::Object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal< data::Object::ModifiedSignalType >(
        data::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SRepresentationEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;

    connections.push(s_RECONSTRUCTION_INOUT, data::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

}
