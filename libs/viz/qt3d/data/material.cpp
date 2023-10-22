/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "viz/qt3d/data/material.hpp"

#include "viz/qt3d/techniques/Lighting.hpp"

#include <data/color.hpp>

#include <QEffect>
#include <QParameter>

namespace sight::viz::qt3d::data
{

material::material(Qt3DCore::QNode* _parent) :
    QMaterial(_parent)
{
    auto* const effect = new Qt3DRender::QEffect;
    this->setEffect(effect);

    /// Initializes parameters with default values.
    m_ambientColor = new Qt3DRender::QParameter(QStringLiteral("u_f3AmbientCol"), QVector3D(0.05F, 0.05F, 0.05F));
    this->effect()->addParameter(m_ambientColor);
    m_ambientColorCopy = new Qt3DRender::QParameter(QStringLiteral("u_f3AmbientCol"), QVector3D(0.05F, 0.05F, 0.05F));
    this->effect()->addParameter(m_ambientColorCopy);

    m_diffuseColor = new Qt3DRender::QParameter(QStringLiteral("u_f3DiffuseCol"), QColor("white"));
    this->effect()->addParameter(m_diffuseColor);

    m_specularColor = new Qt3DRender::QParameter(QStringLiteral("u_f3SpecularCol"), QVector3D(0.2F, 0.2F, 0.2F));
    this->effect()->addParameter(m_specularColor);

    m_shininess = new Qt3DRender::QParameter(QStringLiteral("u_fShininess"), 25.0F);
    this->effect()->addParameter(m_shininess);

    /// Adds lighting technique and sets default light position/intensity. Default shading mode is phong.
    auto* const phong_technique = new viz::qt3d::techniques::Lighting();
    phong_technique->setLightPosition(QVector3D(-100.0F, -100.0F, -100.0F));
    phong_technique->setLightIntensity(QVector3D(1.0F, 1.0F, 1.0F));
    this->addTechnique(phong_technique);
}

//------------------------------------------------------------------------------

material::~material()
= default;

//------------------------------------------------------------------------------

QColor material::getAmbient()
{
    return qvariant_cast<QColor>(m_ambientColor->value());
}

//------------------------------------------------------------------------------

QColor material::getDiffuse()
{
    return qvariant_cast<QColor>(m_diffuseColor->value());
}

//------------------------------------------------------------------------------

QVector3D material::getSpecular()
{
    return qvariant_cast<QVector3D>(m_specularColor->value());
}

//------------------------------------------------------------------------------

float material::getShininess()
{
    return qvariant_cast<float>(m_shininess->value());
}

//------------------------------------------------------------------------------

void material::setAmbient(QColor _ambient)
{
    m_ambientColor->setValue(_ambient);
    m_ambientColorCopy->setValue(_ambient);
}

//------------------------------------------------------------------------------

void material::setDiffuse(QColor _diffuse)
{
    m_diffuseColor->setValue(_diffuse);
}

//------------------------------------------------------------------------------

void material::setSpecular(QVector3D _specular)
{
    m_specularColor->setValue(_specular);
}

//------------------------------------------------------------------------------

void material::setShininess(float _shininess)
{
    m_shininess->setValue(_shininess);
}

//------------------------------------------------------------------------------

void material::addTechnique(Qt3DRender::QTechnique* _technique)
{
    this->effect()->addTechnique(_technique);
}

//------------------------------------------------------------------------------

void material::removeTechnique(Qt3DRender::QTechnique* _technique)
{
    this->effect()->removeTechnique(_technique);
}

//------------------------------------------------------------------------------

void material::addParameter(Qt3DRender::QParameter* _parameter)
{
    this->effect()->addParameter(_parameter);
}

//------------------------------------------------------------------------------

void material::removeParameter(Qt3DRender::QParameter* _parameter)
{
    this->effect()->removeParameter(_parameter);
}

//------------------------------------------------------------------------------

void material::updateOptionsMode(int _options_mode)
{
    viz::qt3d::techniques::Lighting* const tech =
        dynamic_cast<viz::qt3d::techniques::Lighting*>(this->effect()->techniques()[0]);

    if(_options_mode == 1)
    {
        tech->showNormals(false);
    }
    else if(_options_mode == 2)
    {
        tech->enableCellsNormals(false);
        tech->showNormals(true);
    }
    else if(_options_mode == 3)
    {
        tech->enableCellsNormals(true);
        tech->showNormals(true);
    }
}

//------------------------------------------------------------------------------

void material::updatePolygonMode(int _polygon_mode)
{
    viz::qt3d::techniques::Lighting* const tech =
        dynamic_cast<viz::qt3d::techniques::Lighting*>(this->effect()->techniques()[0]);
    tech->updateRasterMode(_polygon_mode);
}

//------------------------------------------------------------------------------

void material::updateShadingMode(int _shading_mode)
{
    viz::qt3d::techniques::Lighting* const tech =
        dynamic_cast<viz::qt3d::techniques::Lighting*>(this->effect()->techniques()[0]);
    tech->setLightingMode(static_cast<viz::qt3d::techniques::Lighting::LightingMode>(_shading_mode));
}

//------------------------------------------------------------------------------

void material::updateRGBAMode(sight::data::material::sptr _sight_material)
{
    //Sets up material colors.
    sight::data::color::csptr sight_ambient = _sight_material->ambient();
    sight::data::color::csptr sight_diffuse = _sight_material->diffuse();

    const QColor ambient(static_cast<int>(sight_ambient->red() * 255), static_cast<int>(sight_ambient->green() * 255),
                         static_cast<int>(sight_ambient->blue() * 255), static_cast<int>(sight_ambient->alpha() * 255));
    this->setAmbient(ambient);

    const QColor diffuse(static_cast<int>(sight_diffuse->red() * 255), static_cast<int>(sight_diffuse->green() * 255),
                         static_cast<int>(sight_diffuse->blue() * 255), static_cast<int>(sight_diffuse->alpha() * 255));
    this->setDiffuse(diffuse);

    const QVector3D specular(.2F, .2F, .2F);
    this->setSpecular(specular);

    this->setShininess(25);
}

} // namespace sight::viz::qt3d::data
