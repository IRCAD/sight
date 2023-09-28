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

#include "SRepresentationEditor.hpp"

#include <core/com/signal.hxx>

#include <data/Material.hpp>
#include <data/Mesh.hpp>

namespace sight::module::ui::qml::reconstruction
{

//------------------------------------------------------------------------------

SRepresentationEditor::SRepresentationEditor() noexcept =
    default;

//------------------------------------------------------------------------------

SRepresentationEditor::~SRepresentationEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void SRepresentationEditor::configuring()
{
}

//------------------------------------------------------------------------------

void SRepresentationEditor::starting()
{
    this->editor::starting();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::stopping()
{
    this->editor::stopping();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::updating()
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

void SRepresentationEditor::onChangeRepresentation(int id)
{
    data::Material::RepresentationType selectedMode = data::Material::SURFACE;

    switch(id)
    {
        case 1:
            selectedMode = data::Material::SURFACE;
            break;

        case 2:
            selectedMode = data::Material::POINT;
            break;

        case 3:
            selectedMode = data::Material::WIREFRAME;
            break;

        case 4:
            selectedMode = data::Material::EDGE;
            break;

        default:
            selectedMode = data::Material::SURFACE;
    }

    m_material->setRepresentationMode(selectedMode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::onChangeShading(int id)
{
    data::Material::ShadingType selectedMode = data::Material::PHONG;

    switch(id)
    {
        case 0:
            selectedMode = data::Material::AMBIENT;
            break;

        case 1:
            selectedMode = data::Material::FLAT;
            break;

        case 2:
            selectedMode = data::Material::PHONG;
            break;

        default:
            selectedMode = data::Material::PHONG;
    }

    m_material->setShadingMode(selectedMode);
    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::onShowNormals(int state)
{
    switch(state)
    {
        case 1:
            m_material->setOptionsMode(data::Material::STANDARD);
            break;

        case 2:
            m_material->setOptionsMode(data::Material::NORMALS);
            break;

        case 3:
            m_material->setOptionsMode(data::Material::CELLS_NORMALS);
            break;

        default:
            m_material->setOptionsMode(data::Material::STANDARD);
    }

    this->notifyMaterial();
}

//------------------------------------------------------------------------------

void SRepresentationEditor::notifyMaterial()
{
    auto reconstruction = m_rec.lock();
    SIGHT_ASSERT("No Reconstruction!", reconstruction);

    data::Object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal<data::Object::ModifiedSignalType>(
        data::Object::MODIFIED_SIG
    );
    sig->async_emit();
}

//------------------------------------------------------------------------------

service::connections_t SRepresentationEditor::getAutoConnections() const
{
    connections_t connections;
    connections.push(s_RECONSTRUCTION_INOUT, data::Object::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::reconstruction
