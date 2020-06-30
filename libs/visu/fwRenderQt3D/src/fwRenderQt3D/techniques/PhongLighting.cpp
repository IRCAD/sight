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

#include "fwRenderQt3D/techniques/PhongLighting.hpp"

#include <fwRuntime/operations.hpp>

#include <QGraphicsApiFilter>
#include <QRasterMode>
#include <QRenderPass>
#include <QShaderProgram>

#include <Qt3DRender/QParameter>

#include <QUrl>

#include <fstream>

namespace fwRenderQt3D
{

namespace techniques
{

PhongLighting::PhongLighting()
{
    // Specifies graphics API to use with this technique
    this->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    this->graphicsApiFilter()->setMajorVersion(3);
    this->graphicsApiFilter()->setMinorVersion(3);
    this->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    // Light parameters. May be reworked when adding light management to qt3d sight features
    m_lightPosition = new Qt3DRender::QParameter(QStringLiteral("u_f3LightPosition"), QVector3D(0.0f, 0.0f, 0.0f));
    this->addParameter(m_lightPosition);

    m_lightIntensity = new Qt3DRender::QParameter(QStringLiteral("u_f3LightIntensity"), QVector3D(0.0f, 0.0f, 0.0f));
    this->addParameter(m_lightIntensity);

    // Phong shader program & render pass : renders the mesh using phong illumination
    this->fixShaderSyntaxe();
    const auto vertexShaderPath = ::fwRuntime::getLibraryResourceFilePath(
        "fwRenderQt3D-" FWRENDERQT3D_VER "/fwRenderQt3D/glsl/phong_VP.glsl");
    const auto fragmentShaderPath = ::fwRuntime::getLibraryResourceFilePath(
        "fwRenderQt3D-" FWRENDERQT3D_VER "/fwRenderQt3D/glsl/phong_FP.glsl");
    Qt3DRender::QShaderProgram* const glShaderProgram = new Qt3DRender::QShaderProgram();
    glShaderProgram->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                    fromStdString(
                                                                                                        vertexShaderPath
                                                                                                        .string()))));
    glShaderProgram->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile(QString::
                                                                                                      fromStdString(
                                                                                                          fragmentShaderPath
                                                                                                          .string()))));

    m_renderPass = new Qt3DRender::QRenderPass();
    m_renderPass->setShaderProgram(glShaderProgram);
    this->addRenderPass(m_renderPass);

    // Normal visualisation shader program & render pass : render normals if specified
    const auto vertexShaderNormalPath = ::fwRuntime::getLibraryResourceFilePath(
        "fwRenderQt3D-" FWRENDERQT3D_VER "/fwRenderQt3D/glsl/normalVisu_VP.glsl");
    const auto geometryShaderNormalPath = ::fwRuntime::getLibraryResourceFilePath(
        "fwRenderQt3D-" FWRENDERQT3D_VER "/fwRenderQt3D/glsl/normalVisu_GP.glsl");
    const auto fragmentShaderNormalPath = ::fwRuntime::getLibraryResourceFilePath(
        "fwRenderQt3D-" FWRENDERQT3D_VER "/fwRenderQt3D/glsl/normalVisu_FP.glsl");
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

    // Add QRasterMode render state to m_renderPass to control polygon mode.
    // Set polygon mode default value to "surface".
    m_rasterModeRenderState = new Qt3DRender::QRasterMode();
    m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Fill);
    m_renderPass->addRenderState(m_rasterModeRenderState);

    // By default, set normal visualisation to false
    m_normalPass->setEnabled(false);
}

PhongLighting::~PhongLighting()
{
}

//------------------------------------------------------------------------------

QVector3D PhongLighting::getLightPosition()
{
    return qvariant_cast<QVector3D>(m_lightPosition->value());
}

//------------------------------------------------------------------------------

QVector3D PhongLighting::getLightIntensity()
{
    return qvariant_cast<QVector3D>(m_lightIntensity->value());
}

//------------------------------------------------------------------------------

void PhongLighting::setLightPosition(QVector3D _lightPosition)
{
    m_lightPosition->setValue(_lightPosition);
}

//------------------------------------------------------------------------------

void PhongLighting::setLightIntensity(QVector3D _lightIntensity)
{
    m_lightIntensity->setValue(_lightIntensity);
}

//------------------------------------------------------------------------------

void PhongLighting::showNormals(bool _isEnabled)
{
    m_normalPass->setEnabled(_isEnabled);
}

//------------------------------------------------------------------------------

void PhongLighting::updateRasterMode(int _rasterMode)
{
    if(_rasterMode == 1) //SURFACE
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Fill);
    }
    else if(_rasterMode == 2) //POINTS
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Points);
    }
    else if(_rasterMode == 4) //LINES
    {
        m_rasterModeRenderState->setRasterMode(Qt3DRender::QRasterMode::Lines);
    }
    else if(_rasterMode == 5) //EDGE (TODO)
    {
    }
}

//------------------------------------------------------------------------------

void PhongLighting::fixShaderSyntaxe()
{
    const auto path = ::fwRuntime::getLibraryResourceFilePath(
        "fwRenderQt3D-" FWRENDERQT3D_VER "/fwRenderQt3D/glsl/lighting.glsl");

    const auto outPath = path.u8string().append(".tmp");

    std::ifstream lightingShaderIn(path);
    std::ofstream lightingShaderOut(outPath);
    if (lightingShaderIn.is_open() && lightingShaderOut.is_open())
    {

        std::string toErase = "#version";
        std::string line;

        while(std::getline(lightingShaderIn, line))
        {
            if(line.find(toErase) != std::string::npos)
            {
                line.erase(0, line.length());
            }
            lightingShaderOut << line << std::endl;
        }

        lightingShaderIn.close();
        lightingShaderOut.close();

        remove(path);
        rename(outPath, path);
    }
}

} // namespace techniques.

} // namespace fwRenderQt3D.
