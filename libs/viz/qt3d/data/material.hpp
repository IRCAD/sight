/************************************************************************
 *
 * Copyright (C) 2020-2024 IRCAD France
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

#pragma once

#include <sight/viz/qt3d/config.hpp>

#include <data/material.hpp>

#include <QColor>
#include <QMaterial>
#include <QPointer>
#include <QVector3D>

namespace Qt3DRender
{

class QParameter;
class QTechnique;

} // namespace Qt3DRender

namespace sight::viz::qt3d::data
{

/**
 * @brief Manages a qt3d material.
 */
class SIGHT_VIZ_QT3D_CLASS_API_QT material : public Qt3DRender::QMaterial
{
Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(QColor ambient READ get_ambient WRITE set_ambient NOTIFY ambient_changed)
Q_PROPERTY(QColor diffuse READ get_diffuse WRITE set_diffuse NOTIFY diffuse_changed)
Q_PROPERTY(QVector3D specular READ get_specular WRITE set_specular NOTIFY specular_changed)
Q_PROPERTY(float shininess READ get_shininess WRITE set_shininess NOTIFY shininess_changed)

public:

    /// Constructs a Qt3D material.
    SIGHT_VIZ_QT3D_API_QT material(Qt3DCore::QNode* _parent = nullptr);

    /// Destroys the material.
    SIGHT_VIZ_QT3D_API_QT ~material() override;

    /// @returns ambient color.
    SIGHT_VIZ_QT3D_API_QT QColor get_ambient();

    /// @returns diffuse color.
    SIGHT_VIZ_QT3D_API_QT QColor get_diffuse();

    /// @returns specular color.
    SIGHT_VIZ_QT3D_API_QT QVector3D get_specular();

    /// @returns shininess.
    SIGHT_VIZ_QT3D_API_QT float get_shininess();

    /// Updates ambient color.
    SIGHT_VIZ_QT3D_API_QT void set_ambient(QColor _ambient);

    /// Updates diffuse color.
    SIGHT_VIZ_QT3D_API_QT void set_diffuse(QColor _diffuse);

    /// Updates specular color.
    SIGHT_VIZ_QT3D_API_QT void set_specular(QVector3D _specular);

    /// Updates shininess.
    SIGHT_VIZ_QT3D_API_QT void set_shininess(float _shininess);

    /// Adds a new technique to the material.
    SIGHT_VIZ_QT3D_API_QT void add_technique(Qt3DRender::QTechnique* _technique);

    /// Removes a technique from the material.
    SIGHT_VIZ_QT3D_API_QT void remove_technique(Qt3DRender::QTechnique* _technique);

    /// Adds a new parameter to the material.
    SIGHT_VIZ_QT3D_API_QT void addParameter(Qt3DRender::QParameter* _parameter);

    /// Removes a parameter from the material.
    SIGHT_VIZ_QT3D_API_QT void removeParameter(Qt3DRender::QParameter* _parameter);

    /// Updates material options mode (standard or point normals).
    SIGHT_VIZ_QT3D_API_QT Q_INVOKABLE void update_options_mode(int _options_mode);

    /// Updates material polygon mode (surface, point, wireframe or edge).
    SIGHT_VIZ_QT3D_API_QT Q_INVOKABLE void update_polygon_mode(int _polygon_mode);

    /// Updates material shading mode (ambient/flat/phong).
    SIGHT_VIZ_QT3D_API_QT Q_INVOKABLE void update_shading_mode(int _shading_mode);

    /// Updates material color.
    SIGHT_VIZ_QT3D_API_QT Q_INVOKABLE void update_rgba_mode(sight::data::material::sptr _sight_material);

Q_SIGNALS:

    /// Signal emitted when ambient color is modified.
    void ambient_changed();

    /// Signal emitted when diffuse color is modified.
    void diffuse_changed();

    /// Signal emitted when specular color is modified.
    void specular_changed();

    /// Signal emitted when shininess is modified.
    void shininess_changed();

private:

    /// Contains material ambient color.
    QPointer<Qt3DRender::QParameter> m_ambient_color;

    /// Contains material ambient color. Used in default fragment shader. Uses of another copy
    /// of the ambient color is needed due to incompatibilities between sheldon and qt3d rules.
    QPointer<Qt3DRender::QParameter> m_ambient_color_copy;

    /// Contains material diffuse color.
    QPointer<Qt3DRender::QParameter> m_diffuse_color;

    /// Contains material specular color.
    QPointer<Qt3DRender::QParameter> m_specular_color;

    /// Contains material shininess.
    QPointer<Qt3DRender::QParameter> m_shininess;
};

} // namespace sight::viz::qt3d::data
