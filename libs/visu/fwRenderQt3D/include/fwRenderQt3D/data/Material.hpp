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

#pragma once

#include "fwRenderQt3D/config.hpp"

#include <fwData/Material.hpp>

#include <QColor>
#include <QMaterial>
#include <QVector3D>

namespace Qt3DRender
{
class QParameter;
class QTechnique;
}

namespace fwRenderQt3D
{

namespace data
{

class FWRENDERQT3D_CLASS_QT_API Material : public Qt3DRender::QMaterial
{

Q_OBJECT

Q_PROPERTY(QColor ambient READ getAmbient WRITE setAmbient NOTIFY ambientChanged)
Q_PROPERTY(QColor diffuse READ getDiffuse WRITE setDiffuse NOTIFY diffuseChanged)
Q_PROPERTY(QVector3D specular READ getSpecular WRITE setSpecular NOTIFY specularChanged)
Q_PROPERTY(float shininess READ getShininess WRITE setShininess NOTIFY shininessChanged)

public:
    // Constructor / Destructor
    FWRENDERQT3D_QT_API Material(Qt3DCore::QNode* _parent = 0);
    FWRENDERQT3D_QT_API virtual ~Material();

    FWRENDERQT3D_QT_API QColor getAmbient();
    FWRENDERQT3D_QT_API QColor getDiffuse();
    FWRENDERQT3D_QT_API QVector3D getSpecular();
    FWRENDERQT3D_QT_API float getShininess();

    FWRENDERQT3D_QT_API void setAmbient(QColor _ambient);
    FWRENDERQT3D_QT_API void setDiffuse(QColor _diffuse);
    FWRENDERQT3D_QT_API void setSpecular(QVector3D _specular);
    FWRENDERQT3D_QT_API void setShininess(float _shininess);

    FWRENDERQT3D_QT_API void addTechnique(Qt3DRender::QTechnique* _technique);
    FWRENDERQT3D_QT_API void removeTechnique(Qt3DRender::QTechnique* _technique);

    FWRENDERQT3D_QT_API void addParameter(Qt3DRender::QParameter* _parameter);
    FWRENDERQT3D_QT_API void removeParameter(Qt3DRender::QParameter* _parameter);

    /// Updates material options mode (standard or point normals)
    /// TODO: add "cells normals" option
    FWRENDERQT3D_QT_API Q_INVOKABLE void updateOptionsMode(int _optionsMode);

    /// Updates material polygon mode (surface, point or wireframe)
    /// TODO: add "edge" mode (surface + wireframe)
    FWRENDERQT3D_QT_API Q_INVOKABLE void updatePolygonMode(int _polygonMode);

    /// TODO: Update material shading mode (flat/gouraud/phong). Only phong lighting supported for now.
    FWRENDERQT3D_QT_API Q_INVOKABLE void updateShadingMode(int _shadingMode);

    /// TODO: Update material color
    FWRENDERQT3D_QT_API Q_INVOKABLE void updateRGBAMode(::fwData::Material::sptr _sightMaterial);

Q_SIGNALS:

    void ambientChanged();

    void diffuseChanged();

    void specularChanged();

    void shininessChanged();

private:

    Qt3DRender::QParameter* m_ambientColor;

    Qt3DRender::QParameter* m_diffuseColor;

    Qt3DRender::QParameter* m_specularColor;

    Qt3DRender::QParameter* m_shininess;

};

} // namespace data.

} // namespace fwRenderQt3D.
