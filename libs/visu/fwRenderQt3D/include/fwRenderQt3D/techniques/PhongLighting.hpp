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

#include <QPointer>
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

/**
 * @brief Manages a phong lighting technique.
 */
class FWRENDERQT3D_CLASS_API PhongLighting : public Qt3DRender::QTechnique
{

Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(QVector3D lightPosition READ getLightPosition WRITE setLightPosition NOTIFY lightPositionChanged)
Q_PROPERTY(QVector3D lightIntensity READ getLightIntensity WRITE setLightIntensity NOTIFY lightIntensityChanged)

public:

    /// Constructs a lighting rendering technique.
    FWRENDERQT3D_API PhongLighting();

    /// Destroys the technique.
    FWRENDERQT3D_API virtual ~PhongLighting();

    /// @returns light position value.
    FWRENDERQT3D_API QVector3D getLightPosition();

    /// @returns light intensity value.
    FWRENDERQT3D_API QVector3D getLightIntensity();

    /// Updates light position.
    FWRENDERQT3D_API void setLightPosition(QVector3D _lightPosition);

    /// Updates light intensity.
    FWRENDERQT3D_API void setLightIntensity(QVector3D _lightIntensity);

    /// Enables/Disables cells normals visualization.
    FWRENDERQT3D_API Q_INVOKABLE void enableCellsNormals(bool _isEnable);

    /// Enables/Disables normals visualization.
    FWRENDERQT3D_API Q_INVOKABLE void showNormals(bool _isEnable);

    /// Modifies raster mode (SURFACE, POINT, LINE, EDGE).
    FWRENDERQT3D_API Q_INVOKABLE void updateRasterMode(int _rasterMode);

Q_SIGNALS:

    /// Signal emitted when light position is modified.
    void lightPositionChanged();

    /// Signal emitted when light intensity is modified.
    void lightIntensityChanged();

private:

    /// Contains light position value.
    QPointer< Qt3DRender::QParameter > m_lightPosition;

    /// Contains light intensity value.
    QPointer< Qt3DRender::QParameter > m_lightIntensity;

    /// Contains default render pass.
    QPointer< Qt3DRender::QRenderPass > m_renderPass;

    /// Contains a second render pass used for EDGE rendering.
    QPointer< Qt3DRender::QRenderPass > m_edgeRenderPass;

    /// Contains a render pass used for point normals vizualisation.
    QPointer< Qt3DRender::QRenderPass > m_normalPass;

    /// Contains a render pass used for cell normals vizualisation.
    QPointer< Qt3DRender::QRenderPass > m_cellNormalPass;

    /// Contains a render state used to control raster mode.
    QPointer< Qt3DRender::QRasterMode > m_rasterModeRenderState;

    /// Specifies if normal visualization is set to point or cell.
    bool m_isCellsNormalsEnabled { false };

    /// This function needs to be called once before using a shader including lighting.inc.glsl, due to
    // incompatibilities
    /// between sheldon and qt3d rules.
    void fixShaderSyntaxe();

};

} // namespace techniques.

} // namespace fwRenderQt3D.
