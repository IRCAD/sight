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

#include <QTechnique>
#include <QVector3D>

namespace Qt3DRender
{
class QParameter;
class QRasterMode;
}

namespace fwRenderQt3D
{

namespace techniques
{

class FWRENDERQT3D_CLASS_API PhongLighting : public Qt3DRender::QTechnique
{

Q_OBJECT

Q_PROPERTY(QVector3D lightPosition READ getLightPosition WRITE setLightPosition NOTIFY lightPositionChanged)
Q_PROPERTY(QVector3D lightIntensity READ getLightIntensity WRITE setLightIntensity NOTIFY lightIntensityChanged)

public:

    // Constructor / Destructor
    FWRENDERQT3D_API PhongLighting();

    FWRENDERQT3D_API virtual ~PhongLighting();

    FWRENDERQT3D_API QVector3D getLightPosition();

    FWRENDERQT3D_API QVector3D getLightIntensity();

    FWRENDERQT3D_API void setLightPosition(QVector3D _lightPosition);

    FWRENDERQT3D_API void setLightIntensity(QVector3D _lightIntensity);

    FWRENDERQT3D_API Q_INVOKABLE void showNormals(bool _isEnable);

    FWRENDERQT3D_API Q_INVOKABLE void updateRasterMode(int _rasterMode);

Q_SIGNALS:

    void lightPositionChanged();

    void lightIntensityChanged();

private:

    Qt3DRender::QParameter* m_lightPosition;

    Qt3DRender::QParameter* m_lightIntensity;

    Qt3DRender::QRenderPass* m_renderPass;

    Qt3DRender::QRenderPass* m_normalPass;

    Qt3DRender::QRasterMode* m_rasterModeRenderState;

    // This function needs to be called once before using a shader including lighting.inc.glsl, due to incompatibilities
    // between sheldon and qt3d rules
    void fixShaderSyntaxe();

};

} // namespace techniques.

} // namespace fwRenderQt3D.
