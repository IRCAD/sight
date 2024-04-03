/************************************************************************
 *
 * Copyright (C) 2021-2024 IRCAD France
 * Copyright (C) 2021 IHU Strasbourg
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

#include <QPointer>
#include <QTechnique>
#include <QVector3D>

namespace Qt3DRender
{

class QParameter;
class QRasterMode;

} // namespace Qt3DRender

namespace sight::viz::qt3d::techniques
{

/**
 * @brief Manages a lighting technique.
 */
class SIGHT_VIZ_QT3D_CLASS_API lighting : public Qt3DRender::QTechnique
{
Q_OBJECT

/// Q_PROPERTY macros associate scene objects with QML properties.
Q_PROPERTY(QVector3D light_position READ get_light_position WRITE set_light_position NOTIFY light_position_changed)
Q_PROPERTY(QVector3D light_intensity READ get_light_intensity WRITE set_light_intensity NOTIFY light_intensity_changed)

public:

    /**
     * @brief Lighting mode
     */
    enum lighting_mode
    {
        ambient = 0,
        flat    = 1,
        phong   = 2
    };

    /// Constructs a lighting rendering technique.
    SIGHT_VIZ_QT3D_API lighting();

    /// Destroys the technique.
    SIGHT_VIZ_QT3D_API ~lighting() override;

    /// @returns light position value.
    SIGHT_VIZ_QT3D_API QVector3D get_light_position();

    /// @returns light intensity value.
    SIGHT_VIZ_QT3D_API QVector3D get_light_intensity();

    /// @returns light mode.
    SIGHT_VIZ_QT3D_API lighting_mode get_lighting_mode();

    /// Updates light position.
    SIGHT_VIZ_QT3D_API void set_light_position(QVector3D _light_position);

    /// Updates light intensity.
    SIGHT_VIZ_QT3D_API void set_light_intensity(QVector3D _light_intensity);

    /// Updates light intensity.
    SIGHT_VIZ_QT3D_API void set_lighting_mode(lighting_mode _lighting_mode);

    /// Enables/Disables cells normals visualization.
    SIGHT_VIZ_QT3D_API Q_INVOKABLE void enable_cells_normals(bool _is_enable);

    /// Enables/Disables normals visualization.
    SIGHT_VIZ_QT3D_API Q_INVOKABLE void show_normals(bool _is_enable);

    /// Modifies raster mode (SURFACE, POINT, LINE, EDGE).
    SIGHT_VIZ_QT3D_API Q_INVOKABLE void update_raster_mode(int _raster_mode);

Q_SIGNALS:

    /// Signal emitted when light position is modified.
    void light_position_changed();

    /// Signal emitted when light intensity is modified.
    void light_intensity_changed();

private:

    /// Contains light position parameter.
    QPointer<Qt3DRender::QParameter> m_light_position;

    /// Contains light intensity parameter.
    QPointer<Qt3DRender::QParameter> m_light_intensity;

    /// Contains light mode parameter.
    QPointer<Qt3DRender::QParameter> m_lighting_mode;

    /// Contains default render pass.
    QPointer<Qt3DRender::QRenderPass> m_render_pass;

    /// Contains a second render pass used for EDGE rendering.
    QPointer<Qt3DRender::QRenderPass> m_edge_render_pass;

    /// Contains a render pass used for point normals visualization.
    QPointer<Qt3DRender::QRenderPass> m_normal_pass;

    /// Contains a render pass used for cell normals visualization.
    QPointer<Qt3DRender::QRenderPass> m_cell_normal_pass;

    /// Contains a render state used to control raster mode.
    QPointer<Qt3DRender::QRasterMode> m_raster_mode_render_state;

    /// Specifies if normal visualization is set to point or cell.
    bool m_is_cells_normals_enabled {false};
};

} // namespace sight::viz::qt3d::techniques
