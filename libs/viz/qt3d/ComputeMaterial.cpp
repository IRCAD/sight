/************************************************************************
 *
 * Copyright (C) 2021-2022 IRCAD France
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

#include "ComputeMaterial.hpp"

#include "core/FrameGraph.hpp"

#include <core/runtime/path.hpp>

#include <QByteArray>

#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QDispatchCompute>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QMemoryBarrier>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QTechniqueFilter>

#include <QUrl>

namespace sight::viz::qt3d
{

//------------------------------------------------------------------------------

ComputeMaterial::ComputeMaterial(Qt3DCore::QNode* _parent) :
    QMaterial(_parent),
    m_effect(new Qt3DRender::QEffect),
    m_computeShader(new Qt3DRender::QShaderProgram),
    m_computeRenderPass(new Qt3DRender::QRenderPass),
    m_indexBuffer(new Qt3DRender::QBuffer),
    m_indexBufferParameter(new Qt3DRender::QParameter),
    m_computeFilterKey(new Qt3DRender::QFilterKey),
    m_computeTechnique(new Qt3DRender::QTechnique)
{
    //Initialize shader program
    const auto computeShaderPath = sight::core::runtime::getLibraryResourceFilePath(
        "viz_qt3d/glsl/quadToTriangleMesh_CP.glsl"
    );
    const auto source = QUrl::fromLocalFile(QString::fromStdString(computeShaderPath.string()));
    m_computeShader->setComputeShaderCode(Qt3DRender::QShaderProgram::loadSource(source));

    m_computeRenderPass->setShaderProgram(m_computeShader);

    //Initialize parameters
    this->setIndexBuffer(m_indexBuffer);
    m_computeRenderPass->addParameter(m_indexBufferParameter);

    //Set OpenGL version
    m_computeTechnique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    m_computeTechnique->graphicsApiFilter()->setMajorVersion(4);
    m_computeTechnique->graphicsApiFilter()->setMinorVersion(3);
    m_computeTechnique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);

    //Set filter Keys
    m_computeFilterKey->setName(QStringLiteral("pass"));
    m_computeFilterKey->setValue(QStringLiteral("compute"));

    //Add to technique
    m_computeTechnique->addFilterKey(m_computeFilterKey);
    m_computeTechnique->addRenderPass(m_computeRenderPass);

    m_effect->addTechnique(m_computeTechnique);
    this->setEffect(m_effect);
}

//------------------------------------------------------------------------------

ComputeMaterial::~ComputeMaterial()
= default;

//------------------------------------------------------------------------------

void ComputeMaterial::setIndexBuffer(Qt3DRender::QBuffer* _buffer)
{
    m_indexBuffer = _buffer;
    m_indexBufferParameter->setName(QStringLiteral("c_in_index"));

    //Set the buffer as parameter data
    QVariant tmpVariant;
    tmpVariant.setValue(m_indexBuffer);
    m_indexBufferParameter->setValue(tmpVariant);
}

//------------------------------------------------------------------------------

void ComputeMaterial::updateFrameGraph(viz::qt3d::core::FrameGraph* _frameGraph, int _numberOfCells)
{
    // Memory barrier to wait for compute shader to finish before rendering the mesh.
    auto* const memBarrier = new Qt3DRender::QMemoryBarrier();
    memBarrier->setWaitOperations(Qt3DRender::QMemoryBarrier::VertexAttributeArray);

    // Set the memory barrier as a child of framegraph's camera selector.
    _frameGraph->addNode(memBarrier, _frameGraph->getCameraSelector());

    // FrameGraph node handling openGL dispatch call.
    auto* const emptyNode = new Qt3DRender::QFrameGraphNode();
    {
        auto* const computeTechFilter = new Qt3DRender::QTechniqueFilter(emptyNode);
        computeTechFilter->addMatch(m_computeFilterKey);
        {
            auto* const dispatch = new Qt3DRender::QDispatchCompute(computeTechFilter);
            dispatch->setWorkGroupX(_numberOfCells);
        }
    }

    // Creates a new branch in the framegraph which is child of framegraph's camera selector.
    _frameGraph->addNode(
        emptyNode,
        qobject_cast<Qt3DRender::QFrameGraphNode*>(_frameGraph->getCameraSelector()->parent())
    );
}

} //namespace sight::viz::qt3d
