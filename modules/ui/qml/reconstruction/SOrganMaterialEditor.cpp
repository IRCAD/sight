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

#include "SOrganMaterialEditor.hpp"

#include <data/Material.hpp>

namespace sight::module::ui::qml::reconstruction
{

//------------------------------------------------------------------------------

SOrganMaterialEditor::SOrganMaterialEditor() noexcept =
    default;

//------------------------------------------------------------------------------

SOrganMaterialEditor::~SOrganMaterialEditor() noexcept =
    default;

//------------------------------------------------------------------------------

void SOrganMaterialEditor::configuring()
{
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::starting()
{
    sight::ui::qml::IQmlEditor::starting();
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::stopping()
{
    sight::ui::qml::IQmlEditor::stopping();
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::updating()
{
    auto reconstruction = m_rec.lock();
    SIGHT_ASSERT("'" << s_RECONSTRUCTION_INOUT << "' must be set as 'inout'", reconstruction);

    data::Material::sptr material = reconstruction->getMaterial();

    QColor color;
    color.setRgbF(material->diffuse()->red(), material->diffuse()->green(), material->diffuse()->blue());

    int alpha = static_cast<int>(material->diffuse()->alpha() * 100);
    Q_EMIT materialChanged(color, alpha);
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::onColor(QColor color)
{
    auto reconstruction = m_rec.lock();
    SIGHT_ASSERT("'" << s_RECONSTRUCTION_INOUT << "' must be set as 'inout'", reconstruction);

    data::Material::sptr material = reconstruction->getMaterial();
    material->diffuse()->red()   = static_cast<float>(color.redF());
    material->diffuse()->green() = static_cast<float>(color.greenF());
    material->diffuse()->blue()  = static_cast<float>(color.blueF());
    this->materialNotification();
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::onOpacitySlider(int value)
{
    auto reconstruction = m_rec.lock();
    SIGHT_ASSERT("'" << s_RECONSTRUCTION_INOUT << "' must be set as 'inout'", reconstruction);

    data::Material::sptr material = reconstruction->getMaterial();
    material->diffuse()->alpha() = static_cast<float>(value) / 100.0F;
    this->materialNotification();
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::materialNotification()
{
    auto reconstruction = m_rec.lock();
    SIGHT_ASSERT("'" << s_RECONSTRUCTION_INOUT << "' must be set as 'inout'", reconstruction);

    data::Object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal<data::Object::ModifiedSignalType>(
        data::Object::s_MODIFIED_SIG
    );
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SOrganMaterialEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_RECONSTRUCTION_INOUT, data::Object::s_MODIFIED_SIG, IService::slots::s_UPDATE);
    return connections;
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::qml::reconstruction
