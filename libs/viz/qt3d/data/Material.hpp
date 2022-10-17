/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "viz/qt3d/config.hpp"

#include <data/Material.hpp>

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
class VIZ_QT3D_CLASS_QT_API Material : public Qt3DRender::QMaterial
{
Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(QColor ambient READ getAmbient WRITE setAmbient NOTIFY ambientChanged)
Q_PROPERTY(QColor diffuse READ getDiffuse WRITE setDiffuse NOTIFY diffuseChanged)
Q_PROPERTY(QVector3D specular READ getSpecular WRITE setSpecular NOTIFY specularChanged)
Q_PROPERTY(float shininess READ getShininess WRITE setShininess NOTIFY shininessChanged)

public:

    /// Constructs a Qt3D Material.
    VIZ_QT3D_QT_API Material(Qt3DCore::QNode* _parent = nullptr);

    /// Destroys the material.
    VIZ_QT3D_QT_API ~Material() override;

    /// @returns ambient color.
    VIZ_QT3D_QT_API QColor getAmbient();

    /// @returns diffuse color.
    VIZ_QT3D_QT_API QColor getDiffuse();

    /// @returns specular color.
    VIZ_QT3D_QT_API QVector3D getSpecular();

    /// @returns shininess.
    VIZ_QT3D_QT_API float getShininess();

    /// Updates ambient color.
    VIZ_QT3D_QT_API void setAmbient(QColor _ambient);

    /// Updates diffuse color.
    VIZ_QT3D_QT_API void setDiffuse(QColor _diffuse);

    /// Updates specular color.
    VIZ_QT3D_QT_API void setSpecular(QVector3D _specular);

    /// Updates shininess.
    VIZ_QT3D_QT_API void setShininess(float _shininess);

    /// Adds a new technique to the material.
    VIZ_QT3D_QT_API void addTechnique(Qt3DRender::QTechnique* _technique);

    /// Removes a technique from the material.
    VIZ_QT3D_QT_API void removeTechnique(Qt3DRender::QTechnique* _technique);

    /// Adds a new parameter to the material.
    VIZ_QT3D_QT_API void addParameter(Qt3DRender::QParameter* _parameter);

    /// Removes a parameter from the material.
    VIZ_QT3D_QT_API void removeParameter(Qt3DRender::QParameter* _parameter);

    /// Updates material options mode (standard or point normals).
    VIZ_QT3D_QT_API Q_INVOKABLE void updateOptionsMode(int _optionsMode);

    /// Updates material polygon mode (surface, point, wireframe or edge).
    VIZ_QT3D_QT_API Q_INVOKABLE void updatePolygonMode(int _polygonMode);

    /// Updates material shading mode (ambient/flat/phong).
    VIZ_QT3D_QT_API Q_INVOKABLE void updateShadingMode(int _shadingMode);

    /// Updates material color.
    VIZ_QT3D_QT_API Q_INVOKABLE void updateRGBAMode(sight::data::Material::sptr _sightMaterial);

Q_SIGNALS:

    /// Signal emitted when ambient color is modified.
    void ambientChanged();

    /// Signal emitted when diffuse color is modified.
    void diffuseChanged();

    /// Signal emitted when specular color is modified.
    void specularChanged();

    /// Signal emitted when shininess is modified.
    void shininessChanged();

private:

    /// Contains material ambient color.
    QPointer<Qt3DRender::QParameter> m_ambientColor;

    /// Contains material ambient color. Used in default fragment shader. Uses of another copy
    /// of the ambient color is needed due to incompatibilities between sheldon and qt3d rules.
    QPointer<Qt3DRender::QParameter> m_ambientColorCopy;

    /// Contains material diffuse color.
    QPointer<Qt3DRender::QParameter> m_diffuseColor;

    /// Contains material specular color.
    QPointer<Qt3DRender::QParameter> m_specularColor;

    /// Contains material shininess.
    QPointer<Qt3DRender::QParameter> m_shininess;
};

} // namespace sight::viz::qt3d::data
