/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
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

#include "uiReconstructionQml/SOrganMaterialEditor.hpp"

#include <fwData/Material.hpp>
#include <fwData/Reconstruction.hpp>

namespace uiReconstructionQml
{

static const ::fwServices::IService::KeyType s_RECONSTRUCTION_INOUT = "reconstruction";

//------------------------------------------------------------------------------

SOrganMaterialEditor::SOrganMaterialEditor() noexcept
{
    this->registerObject(s_RECONSTRUCTION_INOUT, AccessType::INOUT, true);
}

//------------------------------------------------------------------------------

SOrganMaterialEditor::~SOrganMaterialEditor() noexcept
{
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::configuring()
{

}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::starting()
{
    ::fwQml::IQmlEditor::starting();
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::stopping()
{
    ::fwQml::IQmlEditor::stopping();
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::updating()
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("'" + s_RECONSTRUCTION_INOUT + "' must be set as 'inout'", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial();

    QColor color;
    color.setRgbF(material->diffuse()->red(), material->diffuse()->green(), material->diffuse()->blue());

    int alpha = static_cast<int>(material->diffuse()->alpha()*100);
    Q_EMIT materialChanged(color, alpha);
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::onColor(QColor color)
{
    ::fwData::Reconstruction::sptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("'" + s_RECONSTRUCTION_INOUT + "' must be set as 'inout'", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial();
    material->diffuse()->red()        = static_cast<float>(color.redF());
    material->diffuse()->green()      = static_cast<float>(color.greenF());
    material->diffuse()->blue()       = static_cast<float>(color.blueF());
    this->materialNotification();
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::onOpacitySlider(int value )
{

    ::fwData::Reconstruction::csptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("'" + s_RECONSTRUCTION_INOUT + "' must be set as 'inout'", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial();
    material->diffuse()->alpha()      = value/100.0f;
    this->materialNotification();
}

//------------------------------------------------------------------------------

void SOrganMaterialEditor::materialNotification()
{
    ::fwData::Reconstruction::csptr reconstruction = this->getInOut< ::fwData::Reconstruction >(s_RECONSTRUCTION_INOUT);
    SLM_ASSERT("'" + s_RECONSTRUCTION_INOUT + "' must be set as 'inout'", reconstruction);

    ::fwData::Object::ModifiedSignalType::sptr sig;
    sig = reconstruction->getMaterial()->signal< ::fwData::Object::ModifiedSignalType >(
        ::fwData::Object::s_MODIFIED_SIG);
    sig->asyncEmit();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SOrganMaterialEditor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_RECONSTRUCTION_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//------------------------------------------------------------------------------

}
