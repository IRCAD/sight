/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
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

#include <core/runtime/operations.hpp>

#include <QGraphicsApiFilter>
#include <QRasterMode>
#include <QRenderPass>
#include <QShaderProgram>

#include <Qt3DRender/QParameter>

#include <QUrl>

#include <fstream>

namespace sight::viz::qt3d
{

namespace techniques
{

Lighting::Lighting()
{
    // Specifies graphics API to use with this technique.
    this->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    this->graphicsApiFilter()->setMajorVersion(3);
    this->graphicsApiFilter()->setMinorVersion(3);
    this->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    // Light parameters. May be reworked when adding light management to qt3d sight features.
    m_lightPosition = new Qt3DRender::QParameter(QStringLiteral("u_f3LightPosition"), QVector3D(0.0f, 0.0f, 0.0f));
    this->addParameter(m_lightPosition);

    m_lightIntensity = new Qt3DRender::QParameter(QStringLiteral("u_f3LightIntensity"), QVector3D(0.0f, 0.0f, 0.0f));
    this->addParameter(m_lightIntensity);

    // Lighting mode parameter.
    m_lightingMode = new Qt3DRender::QParameter(QStringLiteral("u_iLightingMode"), LightingMode::PHONG);
    this->addParameter(m_lightingMode);

    // Point normals visualisation shader program & render pass : render normals if specified.
    const auto vertexShaderNormalPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/normalVisu_VP.glsl");
    const auto geometryShaderNormalPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/normalVisu_GP.glsl");
    const auto fragmentShaderNormalPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/normalVisu_FP.glsl");
    Qt3DRender::QShaderProgram* const normalShaderProgram = new Qt3DRender::QShaderProgram();
    normalShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                        fromStdString(
                                                                                                            vertexShaderNormalPath
                                                                                                            .string()))));
    normalShaderProgram->setGeometryShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                          fromStdString(
                                                                                                              geometryShaderNormalPath
                                                                                                              .string()))));
    normalShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                          fromStdString(
                                                                                                              fragmentShaderNormalPath
                                                                                                              .string()))));

    m_normalPass = new Qt3DRender::QRenderPass();
    m_normalPass->setShaderProgram(normalShaderProgram);
    this->addRenderPass(m_normalPass);

    // Cell normals visualisation shader program & render pass : render normals if specified.
    const auto vertexShaderCellNormalPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/normalVisu_VP.glsl");
    const auto geometryShaderCellNormalPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/cellsNormalVisu_GP.glsl");
    const auto fragmentShaderCellNormalPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/normalVisu_FP.glsl");
    Qt3DRender::QShaderProgram* const cellNormalShaderProgram = new Qt3DRender::QShaderProgram();
    cellNormalShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                            fromStdString(
                                                                                                                vertexShaderCellNormalPath
                                                                                                                .string()))));
    cellNormalShaderProgram->setGeometryShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                              fromStdString(
                                                                                                                  geometryShaderCellNormalPath
                                                                                                                  .
                                                                                                                  string()))));
    cellNormalShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                              fromStdString(
                                                                                                                  fragmentShaderCellNormalPath
                                                                                                                  .
                                                                                                                  string()))));

    m_cellNormalPass = new Qt3DRender::QRenderPass();
    m_cellNormalPass->setShaderProgram(cellNormalShaderProgram);
    this->addRenderPass(m_cellNormalPass);

    // Lighting shader program & render pass : renders the mesh using selected illumination algorithm.
    const auto vertexShaderPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/defaultRender_VP.glsl");
    const auto fragmentShaderPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/defaultRender_FP.glsl");
    Qt3DRender::QShaderProgram* const renderShaderProgram = new Qt3DRender::QShaderProgram();
    renderShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                        fromStdString(
                                                                                                            vertexShaderPath
                                                                                                            .string()))));
    renderShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                          fromStdString(
                                                                                                              fragmentShaderPath
                                                                                                              .string()))));

    m_renderPass = new Qt3DRender::QRenderPass();
    m_renderPass->setShaderProgram(renderShaderProgram);
    this->addRenderPass(m_renderPass);

    // Adds a render pass needed with "EDGE" polygon mode.
    const auto edgeVertexShaderPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/defaultRender_VP.glsl");
    const auto edgeFragmentShaderPath = core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/edgeRender_FP.glsl");
    Qt3DRender::QShaderProgram* const edgeShaderProgram = new Qt3DRender::QShaderProgram();
    edgeShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                      fromStdString(
                                                                                                          edgeVertexShaderPath
                                                                                                          .string()))));
    edgeShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                        fromStdString(
                                                                                                            edgeFragmentShaderPath
                                                                                                            .string()))));

    m_edgeRenderPass = new Qt3DRender::QRenderPass();
    m_edgeRenderPass->setShaderProgram(edgeShaderProgram);
    auto edgeRasterMode = new Qt3DRender::QRasterMode();
    edgeRasterMode->setRasterMode(Qt3DRender::QRasterMode::Lines);
    m_edgeRenderPass->addRenderState(edgeRasterMode);
    this->addRenderPass(m_edgeRenderPass);

    // Adds QRasterMode render state to m_renderPass to control polygon mode.
    // Sets polygon mode default value to "surface".
    m_rasterModeRenderState = new Qt3DRender::QRasterMode();
    m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Fill);
    m_renderPass->addRenderState(m_rasterModeRenderState);

    // By default, sets normal visualisation to false.
    m_normalPass->setEnabled(false);
    m_cellNormalPass->setEnabled(false);

    // By default, disables "EDGE" render pass.
    m_edgeRenderPass->setEnabled(false);
}

Lighting::~Lighting()
{
}

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
    int lightingMode = qvariant_cast<int>(m_lightingMode->value());
    return static_cast< Lighting::LightingMode >(lightingMode);
}

//------------------------------------------------------------------------------

void Lighting::setLightPosition(QVector3D _lightPosition)
{
    m_lightPosition->setValue(_lightPosition);
}

//------------------------------------------------------------------------------

void Lighting::setLightIntensity(QVector3D _lightIntensity)
{
    m_lightIntensity->setValue(_lightIntensity);
}

//------------------------------------------------------------------------------

void Lighting::setLightingMode(Lighting::LightingMode _lightingMode)
{
    m_lightingMode->setValue(_lightingMode);
}

//------------------------------------------------------------------------------

void Lighting::enableCellsNormals(bool _isEnabled)
{
    m_isCellsNormalsEnabled = _isEnabled;
}

//------------------------------------------------------------------------------

void Lighting::showNormals(bool _isEnabled)
{
    if(m_isCellsNormalsEnabled)
    {
        // If cell normal visu is enabled, disables point normals pass if necessary and enables cell normals pass.
        if(m_normalPass->isEnabled())
        {
            m_normalPass->setEnabled(false);
        }
        m_cellNormalPass->setEnabled(_isEnabled);
    }
    else
    {
        // If point normal visu is enabled, disables cell normals pass if necessary and enables point normals pass.
        if(m_cellNormalPass->isEnabled())
        {
            m_cellNormalPass->setEnabled(false);
        }
        m_normalPass->setEnabled(_isEnabled);
    }
}

//------------------------------------------------------------------------------

void Lighting::updateRasterMode(int _rasterMode)
{
    if(m_edgeRenderPass->isEnabled() && _rasterMode != 5)
    {
        m_edgeRenderPass->setEnabled(false);
    }

    if(_rasterMode == 1) //SURFACE.
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Fill);
    }
    else if(_rasterMode == 2) //POINTS.
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Points);
    }
    else if(_rasterMode == 4) //LINES.
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Lines);
    }
    else if(_rasterMode == 5) //EDGE.
    {
        if(!m_edgeRenderPass->isEnabled())
        {
            m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Fill);
            m_edgeRenderPass->setEnabled(true);
        }
    }
}

} // namespace techniques.

} // namespace sight::viz::qt3d.
