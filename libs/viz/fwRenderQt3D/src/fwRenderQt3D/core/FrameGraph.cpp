/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#include "fwRenderQt3D/core/FrameGraph.hpp"

#include <QCameraSelector>
#include <QClearBuffers>
#include <QCullFace>
#include <QDepthTest>
#include <QRenderStateSet>
#include <QRenderSurfaceSelector>
#include <QViewport>

namespace fwRenderQt3D
{

namespace core
{

FrameGraph::FrameGraph(Qt3DCore::QNode* _parent) :
    QFrameGraphNode(_parent)
{
    m_renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector(this);
    {
        m_viewport = new Qt3DRender::QViewport(m_renderSurfaceSelector);
        {
            m_cameraSelector = new Qt3DRender::QCameraSelector(m_viewport);
            {
                m_clearBuffers = new Qt3DRender::QClearBuffers(m_cameraSelector);
                {
                    m_renderStateSet = new Qt3DRender::QRenderStateSet(m_clearBuffers);

                    // Adds a render state to avoid culling.
                    QPointer< Qt3DRender::QCullFace > const culling = new Qt3DRender::QCullFace();
                    culling->setMode(Qt3DRender::QCullFace::NoCulling);
                    m_renderStateSet->addRenderState(culling);

                    // Adds a render state to add depth test.
                    QPointer< Qt3DRender::QDepthTest > const depthTest = new Qt3DRender::QDepthTest();
                    depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);
                    m_renderStateSet->addRenderState(depthTest);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

FrameGraph::~FrameGraph()
{
}

//------------------------------------------------------------------------------

Qt3DCore::QEntity* const FrameGraph::getCamera() const
{
    return m_cameraSelector->camera();
}

//------------------------------------------------------------------------------

QColor FrameGraph::getClearColor() const
{
    return m_clearBuffers->clearColor();
}

//------------------------------------------------------------------------------

Qt3DRender::QClearBuffers::BufferType FrameGraph::getBuffersToClear() const
{
    return m_clearBuffers->buffers();
}

//------------------------------------------------------------------------------

void FrameGraph::setCamera(Qt3DCore::QEntity* _camera)
{
    m_cameraSelector->setCamera(_camera);
}

//------------------------------------------------------------------------------

void FrameGraph::setClearColor(const QColor& _color)
{
    m_clearBuffers->setClearColor(_color);
}

//------------------------------------------------------------------------------

void FrameGraph::setBuffersToClear(Qt3DRender::QClearBuffers::BufferType _buffers)
{
    m_clearBuffers->setBuffers(_buffers);
}

} // namespace core.

} // namespace fwRenderQt3D.
