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
    m_ambient_color = new Qt3DRender::QParameter(QStringLiteral("u_f3AmbientCol"), QVector3D(0.05F, 0.05F, 0.05F));
    this->effect()->addParameter(m_ambient_color);
    m_ambient_color_copy = new Qt3DRender::QParameter(QStringLiteral("u_f3AmbientCol"), QVector3D(0.05F, 0.05F, 0.05F));
    this->effect()->addParameter(m_ambient_color_copy);

    m_diffuse_color = new Qt3DRender::QParameter(QStringLiteral("u_f3DiffuseCol"), QColor("white"));
    this->effect()->addParameter(m_diffuse_color);

    m_specular_color = new Qt3DRender::QParameter(QStringLiteral("u_f3SpecularCol"), QVector3D(0.2F, 0.2F, 0.2F));
    this->effect()->addParameter(m_specular_color);

    m_shininess = new Qt3DRender::QParameter(QStringLiteral("u_fShininess"), 25.0F);
    this->effect()->addParameter(m_shininess);

    /// Adds lighting technique and sets default light position/intensity. Default shading mode is phong.
    auto* const phong_technique = new viz::qt3d::techniques::lighting();
    phong_technique->set_light_position(QVector3D(-100.0F, -100.0F, -100.0F));
    phong_technique->set_light_intensity(QVector3D(1.0F, 1.0F, 1.0F));
    this->add_technique(phong_technique);
}

//------------------------------------------------------------------------------

material::~material()
= default;

//------------------------------------------------------------------------------

QColor material::get_ambient()
{
    return qvariant_cast<QColor>(m_ambient_color->value());
}

//------------------------------------------------------------------------------

QColor material::get_diffuse()
{
    return qvariant_cast<QColor>(m_diffuse_color->value());
}

//------------------------------------------------------------------------------

QVector3D material::get_specular()
{
    return qvariant_cast<QVector3D>(m_specular_color->value());
}

//------------------------------------------------------------------------------

float material::get_shininess()
{
    return qvariant_cast<float>(m_shininess->value());
}

//------------------------------------------------------------------------------

void material::set_ambient(QColor _ambient)
{
    m_ambient_color->setValue(_ambient);
    m_ambient_color_copy->setValue(_ambient);
}

//------------------------------------------------------------------------------

void material::set_diffuse(QColor _diffuse)
{
    m_diffuse_color->setValue(_diffuse);
}

//------------------------------------------------------------------------------

void material::set_specular(QVector3D _specular)
{
    m_specular_color->setValue(_specular);
}

//------------------------------------------------------------------------------

void material::set_shininess(float _shininess)
{
    m_shininess->setValue(_shininess);
}

//------------------------------------------------------------------------------

void material::add_technique(Qt3DRender::QTechnique* _technique)
{
    this->effect()->addTechnique(_technique);
}

//------------------------------------------------------------------------------

void material::remove_technique(Qt3DRender::QTechnique* _technique)
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

void material::update_options_mode(int _options_mode)
{
    viz::qt3d::techniques::lighting* const tech =
        dynamic_cast<viz::qt3d::techniques::lighting*>(this->effect()->techniques()[0]);

    if(_options_mode == 1)
    {
        tech->show_normals(false);
    }
    else if(_options_mode == 2)
    {
        tech->enable_cells_normals(false);
        tech->show_normals(true);
    }
    else if(_options_mode == 3)
    {
        tech->enable_cells_normals(true);
        tech->show_normals(true);
    }
}

//------------------------------------------------------------------------------

void material::update_polygon_mode(int _polygon_mode)
{
    viz::qt3d::techniques::lighting* const tech =
        dynamic_cast<viz::qt3d::techniques::lighting*>(this->effect()->techniques()[0]);
    tech->update_raster_mode(_polygon_mode);
}

//------------------------------------------------------------------------------

void material::update_shading_mode(int _shading_mode)
{
    viz::qt3d::techniques::lighting* const tech =
        dynamic_cast<viz::qt3d::techniques::lighting*>(this->effect()->techniques()[0]);
    tech->set_lighting_mode(static_cast<viz::qt3d::techniques::lighting::lighting_mode>(_shading_mode));
}

//------------------------------------------------------------------------------

void material::update_rgba_mode(sight::data::material::sptr _sight_material)
{
    //Sets up material colors.
    sight::data::color::csptr sight_ambient = _sight_material->ambient();
    sight::data::color::csptr sight_diffuse = _sight_material->diffuse();

    const QColor ambient(static_cast<int>(sight_ambient->red() * 255), static_cast<int>(sight_ambient->green() * 255),
                         static_cast<int>(sight_ambient->blue() * 255), static_cast<int>(sight_ambient->alpha() * 255));
    this->set_ambient(ambient);

    const QColor diffuse(static_cast<int>(sight_diffuse->red() * 255), static_cast<int>(sight_diffuse->green() * 255),
                         static_cast<int>(sight_diffuse->blue() * 255), static_cast<int>(sight_diffuse->alpha() * 255));
    this->set_diffuse(diffuse);

    const QVector3D specular(.2F, .2F, .2F);
    this->set_specular(specular);

    this->set_shininess(25);
}

} // namespace sight::viz::qt3d::data
