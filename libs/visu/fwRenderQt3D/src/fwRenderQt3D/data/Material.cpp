/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwRenderQt3D/data/Material.hpp"

#include "fwRenderQt3D/techniques/Lighting.hpp"

#include <fwData/Color.hpp>

#include <QEffect>
#include <QParameter>

namespace fwRenderQt3D
{

namespace data
{

Material::Material(Qt3DCore::QNode* _parent) :
    QMaterial(_parent)
{
    Qt3DRender::QEffect* const effect = new Qt3DRender::QEffect;
    this->setEffect(effect);

    /// Initializes parameters with default values.
    m_ambientColor = new Qt3DRender::QParameter(QStringLiteral("u_f3AmbientCol"), QVector3D(0.05f, 0.05f, 0.05f));
    this->effect()->addParameter(m_ambientColor);
    m_ambientColorCopy = new Qt3DRender::QParameter(QStringLiteral("u_f3AmbientCol"), QVector3D(0.05f, 0.05f, 0.05f));
    this->effect()->addParameter(m_ambientColorCopy);

    m_diffuseColor = new Qt3DRender::QParameter(QStringLiteral("u_f3DiffuseCol"), QColor("white"));
    this->effect()->addParameter(m_diffuseColor);

    m_specularColor = new Qt3DRender::QParameter(QStringLiteral("u_f3SpecularCol"), QVector3D(0.2f, 0.2f, 0.2f));
    this->effect()->addParameter(m_specularColor);

    m_shininess = new Qt3DRender::QParameter(QStringLiteral("u_fShininess"), 25.0f);
    this->effect()->addParameter(m_shininess);

    /// Adds lighting technique and sets default light position/intensity. Default shading mode is phong.
    ::fwRenderQt3D::techniques::Lighting* const phongTechnique = new ::fwRenderQt3D::techniques::Lighting();
    phongTechnique->setLightPosition(QVector3D(-100.0f, -100.0f, -100.0f));
    phongTechnique->setLightIntensity(QVector3D(1.0f, 1.0f, 1.0f));
    this->addTechnique(phongTechnique);
}

//------------------------------------------------------------------------------

Material::~Material()
{
}

//------------------------------------------------------------------------------

QColor Material::getAmbient()
{
    return qvariant_cast<QColor>(m_ambientColor->value());
}

//------------------------------------------------------------------------------

QColor Material::getDiffuse()
{
    return qvariant_cast<QColor>(m_diffuseColor->value());
}

//------------------------------------------------------------------------------

QVector3D Material::getSpecular()
{
    return qvariant_cast<QVector3D>(m_specularColor->value());
}

//------------------------------------------------------------------------------

float Material::getShininess()
{
    return qvariant_cast<float>(m_shininess->value());
}

//------------------------------------------------------------------------------

void Material::setAmbient(QColor _ambient)
{
    m_ambientColor->setValue(_ambient);
    m_ambientColorCopy->setValue(_ambient);
}

//------------------------------------------------------------------------------

void Material::setDiffuse(QColor _diffuse)
{
    m_diffuseColor->setValue(_diffuse);
}

//------------------------------------------------------------------------------

void Material::setSpecular(QVector3D _specular)
{
    m_specularColor->setValue(_specular);
}

//------------------------------------------------------------------------------

void Material::setShininess(float _shininess)
{
    m_shininess->setValue(_shininess);
}

//------------------------------------------------------------------------------

void Material::addTechnique(Qt3DRender::QTechnique* _technique)
{
    this->effect()->addTechnique(_technique);
}

//------------------------------------------------------------------------------

void Material::removeTechnique(Qt3DRender::QTechnique* _technique)
{
    this->effect()->removeTechnique(_technique);
}

//------------------------------------------------------------------------------

void Material::addParameter(Qt3DRender::QParameter* _parameter)
{
    this->effect()->addParameter(_parameter);
}

//------------------------------------------------------------------------------

void Material::removeParameter(Qt3DRender::QParameter* _parameter)
{
    this->effect()->removeParameter(_parameter);
}

//------------------------------------------------------------------------------

void Material::updateOptionsMode(int _optionsMode)
{
    ::fwRenderQt3D::techniques::Lighting* const tech =
        (::fwRenderQt3D::techniques::Lighting*)this->effect()->techniques()[0];

    if(_optionsMode == 1)
    {
        tech->showNormals(false);
    }
    else if(_optionsMode == 2)
    {
        tech->enableCellsNormals(false);
        tech->showNormals(true);
    }
    else if(_optionsMode == 3)
    {
        tech->enableCellsNormals(true);
        tech->showNormals(true);
    }
}

//------------------------------------------------------------------------------

void Material::updatePolygonMode(int _polygonMode)
{
    ::fwRenderQt3D::techniques::Lighting* const tech =
        (::fwRenderQt3D::techniques::Lighting*)this->effect()->techniques()[0];
    tech->updateRasterMode(_polygonMode);
}

//------------------------------------------------------------------------------

void Material::updateShadingMode(int _shadingMode)
{
    ::fwRenderQt3D::techniques::Lighting* const tech =
        (::fwRenderQt3D::techniques::Lighting*)this->effect()->techniques()[0];
    tech->setLightingMode(static_cast< ::fwRenderQt3D::techniques::Lighting::LightingMode >(_shadingMode));
}

//------------------------------------------------------------------------------

void Material::updateRGBAMode(::fwData::Material::sptr _sightMaterial)
{
    //Sets up Material colors.
    ::fwData::Color::csptr sightAmbient = _sightMaterial->ambient();
    ::fwData::Color::csptr sightDiffuse = _sightMaterial->diffuse();

    const QColor ambient(static_cast<int>(sightAmbient->red()*255), static_cast<int>(sightAmbient->green()*255),
                         static_cast<int>(sightAmbient->blue()*255), static_cast<int>(sightAmbient->alpha()*255));
    this->setAmbient(ambient);

    const QColor diffuse(static_cast<int>(sightDiffuse->red()*255), static_cast<int>(sightDiffuse->green()*255),
                         static_cast<int>(sightDiffuse->blue()*255), static_cast<int>(sightDiffuse->alpha()*255));
    this->setDiffuse(diffuse);

    const QVector3D specular(.2f, .2f, .2f);
    this->setSpecular(specular);

    this->setShininess(25);
}

} // namespace data.

} //namespace fwRenderQt3D.
