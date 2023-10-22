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

#include "viz/qt3d/techniques/Lighting.hpp"

#include <core/runtime/path.hpp>

#include <QGraphicsApiFilter>
#include <QRasterMode>
#include <QRenderPass>
#include <QShaderProgram>

#include <Qt3DRender/QParameter>

#include <QUrl>

#include <fstream>

namespace sight::viz::qt3d::techniques
{

Lighting::Lighting() :
    m_renderPass(new Qt3DRender::QRenderPass()),
    m_edgeRenderPass(new Qt3DRender::QRenderPass()),
    m_normalPass(new Qt3DRender::QRenderPass()),
    m_cellNormalPass(new Qt3DRender::QRenderPass()),
    m_rasterModeRenderState(new Qt3DRender::QRasterMode())
{
    // Specifies graphics API to use with this technique.
    this->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    this->graphicsApiFilter()->setMajorVersion(3);
    this->graphicsApiFilter()->setMinorVersion(3);
    this->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    // Light parameters. May be reworked when adding light management to qt3d sight features.
    m_lightPosition = new Qt3DRender::QParameter(QStringLiteral("u_f3LightPosition"), QVector3D(0.0F, 0.0F, 0.0F));
    this->addParameter(m_lightPosition);

    m_lightIntensity = new Qt3DRender::QParameter(QStringLiteral("u_f3LightIntensity"), QVector3D(0.0F, 0.0F, 0.0F));
    this->addParameter(m_lightIntensity);

    // Lighting mode parameter.
    m_lightingMode = new Qt3DRender::QParameter(QStringLiteral("u_iLightingMode"), LightingMode::PHONG);
    this->addParameter(m_lightingMode);

    // Point normals visualisation shader program & render pass : render normals if specified.
    const auto vertex_shader_normal_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/normalVisu_VP.glsl"
    );
    const auto geometry_shader_normal_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/normalVisu_GP.glsl"
    );
    const auto fragment_shader_normal_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/normalVisu_FP.glsl"
    );
    auto* const normal_shader_program = new Qt3DRender::QShaderProgram();
    normal_shader_program->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    vertex_shader_normal_path
                    .string()
                )
            )
        )
    );
    normal_shader_program->setGeometryShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    geometry_shader_normal_path
                    .string()
                )
            )
        )
    );
    normal_shader_program->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    fragment_shader_normal_path
                    .string()
                )
            )
        )
    );

    m_normalPass->setShaderProgram(normal_shader_program);
    this->addRenderPass(m_normalPass);

    // Cell normals visualisation shader program & render pass : render normals if specified.
    const auto vertex_shader_cell_normal_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/normalVisu_VP.glsl"
    );
    const auto geometry_shader_cell_normal_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/cellsNormalVisu_GP.glsl"
    );
    const auto fragment_shader_cell_normal_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/normalVisu_FP.glsl"
    );
    auto* const cell_normal_shader_program = new Qt3DRender::QShaderProgram();
    cell_normal_shader_program->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    vertex_shader_cell_normal_path
                    .string()
                )
            )
        )
    );
    cell_normal_shader_program->setGeometryShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    geometry_shader_cell_normal_path
                    .
                    string()
                )
            )
        )
    );
    cell_normal_shader_program->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    fragment_shader_cell_normal_path
                    .
                    string()
                )
            )
        )
    );

    m_cellNormalPass->setShaderProgram(cell_normal_shader_program);
    this->addRenderPass(m_cellNormalPass);

    // Lighting shader program & render pass : renders the mesh using selected illumination algorithm.
    const auto vertex_shader_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/defaultRender_VP.glsl"
    );
    const auto fragment_shader_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/defaultRender_FP.glsl"
    );
    auto* const render_shader_program = new Qt3DRender::QShaderProgram();
    render_shader_program->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    vertex_shader_path
                    .string()
                )
            )
        )
    );
    render_shader_program->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    fragment_shader_path
                    .string()
                )
            )
        )
    );

    m_renderPass->setShaderProgram(render_shader_program);
    this->addRenderPass(m_renderPass);

    // Adds a render pass needed with "EDGE" polygon mode.
    const auto edge_vertex_shader_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/defaultRender_VP.glsl"
    );
    const auto edge_fragment_shader_path = core::runtime::get_library_resource_file_path(
        "viz_qt3d/glsl/edgeRender_FP.glsl"
    );
    auto* const edge_shader_program = new Qt3DRender::QShaderProgram();
    edge_shader_program->setVertexShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    edge_vertex_shader_path
                    .string()
                )
            )
        )
    );
    edge_shader_program->setFragmentShaderCode(
        Qt3DRender::QShaderProgram::loadSource(
            QUrl::fromLocalFile(
                QString::
                fromStdString(
                    edge_fragment_shader_path
                    .string()
                )
            )
        )
    );

    m_edgeRenderPass->setShaderProgram(edge_shader_program);
    auto* edge_raster_mode = new Qt3DRender::QRasterMode();
    edge_raster_mode->setRasterMode(Qt3DRender::QRasterMode::Lines);
    m_edgeRenderPass->addRenderState(edge_raster_mode);
    this->addRenderPass(m_edgeRenderPass);

    // Adds QRasterMode render state to m_renderPass to control polygon mode.
    // Sets polygon mode default value to "surface".
    m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Fill);
    m_renderPass->addRenderState(m_rasterModeRenderState);

    // By default, sets normal visualisation to false.
    m_normalPass->setEnabled(false);
    m_cellNormalPass->setEnabled(false);

    // By default, disables "EDGE" render pass.
    m_edgeRenderPass->setEnabled(false);
}

Lighting::~Lighting()
= default;

//------------------------------------------------------------------------------

QVector3D Lighting::getLightPosition()
{
    return qvariant_cast<QVector3D>(m_lightPosition->value());
}

//------------------------------------------------------------------------------

QVector3D Lighting::getLightIntensity()
{
    return qvariant_cast<QVector3D>(m_lightIntensity->value());
}

//------------------------------------------------------------------------------

Lighting::LightingMode Lighting::getLightingMode()
{
    int lighting_mode = qvariant_cast<int>(m_lightingMode->value());
    return static_cast<Lighting::LightingMode>(lighting_mode);
}

//------------------------------------------------------------------------------

void Lighting::setLightPosition(QVector3D _light_position)
{
    m_lightPosition->setValue(_light_position);
}

//------------------------------------------------------------------------------

void Lighting::setLightIntensity(QVector3D _light_intensity)
{
    m_lightIntensity->setValue(_light_intensity);
}

//------------------------------------------------------------------------------

void Lighting::setLightingMode(Lighting::LightingMode _lighting_mode)
{
    m_lightingMode->setValue(_lighting_mode);
}

//------------------------------------------------------------------------------

void Lighting::enableCellsNormals(bool _is_enabled)
{
    m_isCellsNormalsEnabled = _is_enabled;
}

//------------------------------------------------------------------------------

void Lighting::showNormals(bool _is_enabled)
{
    if(m_isCellsNormalsEnabled)
    {
        // If cell normal visu is enabled, disables point normals pass if necessary and enables cell normals pass.
        if(m_normalPass->isEnabled())
        {
            m_normalPass->setEnabled(false);
        }

        m_cellNormalPass->setEnabled(_is_enabled);
    }
    else
    {
        // If point normal visu is enabled, disables cell normals pass if necessary and enables point normals pass.
        if(m_cellNormalPass->isEnabled())
        {
            m_cellNormalPass->setEnabled(false);
        }

        m_normalPass->setEnabled(_is_enabled);
    }
}

//------------------------------------------------------------------------------

void Lighting::updateRasterMode(int _raster_mode)
{
    if(m_edgeRenderPass->isEnabled() && _raster_mode != 5)
    {
        m_edgeRenderPass->setEnabled(false);
    }

    if(_raster_mode == 1) //SURFACE.
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Fill);
    }
    else if(_raster_mode == 2) //POINTS.
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Points);
    }
    else if(_raster_mode == 4) //LINES.
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Lines);
    }
    else if(_raster_mode == 5) //EDGE.
    {
        if(!m_edgeRenderPass->isEnabled())
        {
            m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Fill);
            m_edgeRenderPass->setEnabled(true);
        }
    }
}

} // namespace sight::viz::qt3d::techniques
