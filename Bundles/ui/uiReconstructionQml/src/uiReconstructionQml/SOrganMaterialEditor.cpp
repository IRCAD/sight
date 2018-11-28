/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
