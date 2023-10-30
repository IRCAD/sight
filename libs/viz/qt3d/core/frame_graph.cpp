/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include "viz/qt3d/core/frame_graph.hpp"

#include <QCameraSelector>
#include <QClearBuffers>
#include <QCullFace>
#include <QDepthTest>
#include <QRenderStateSet>
#include <QRenderSurfaceSelector>
#include <QViewport>

namespace sight::viz::qt3d::core
{

frame_graph::frame_graph(Qt3DCore::QNode* _parent) :
    QFrameGraphNode(_parent),
    m_render_surface_selector(new Qt3DRender::QRenderSurfaceSelector(this))
{
    {
        m_viewport = new Qt3DRender::QViewport(m_render_surface_selector);
        {
            m_camera_selector = new Qt3DRender::QCameraSelector(m_viewport);
            {
                m_clear_buffers = new Qt3DRender::QClearBuffers(m_camera_selector);
                {
                    m_render_state_set = new Qt3DRender::QRenderStateSet(m_clear_buffers);

                    // Adds a render state to avoid culling.
                    QPointer<Qt3DRender::QCullFace> const culling = new Qt3DRender::QCullFace();
                    culling->setMode(Qt3DRender::QCullFace::NoCulling);
                    m_render_state_set->addRenderState(culling);

                    // Adds a render state to add depth test.
                    QPointer<Qt3DRender::QDepthTest> const depth_test = new Qt3DRender::QDepthTest();
                    depth_test->setDepthFunction(Qt3DRender::QDepthTest::Less);
                    m_render_state_set->addRenderState(depth_test);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------

frame_graph::~frame_graph()
= default;

//------------------------------------------------------------------------------

Qt3DCore::QEntity* frame_graph::get_camera() const
{
    return m_camera_selector->camera();
}

//------------------------------------------------------------------------------

QColor frame_graph::get_clear_color() const
{
    return m_clear_buffers->clearColor();
}

//------------------------------------------------------------------------------

Qt3DRender::QClearBuffers::BufferType frame_graph::get_buffers_to_clear() const
{
    return m_clear_buffers->buffers();
}

//------------------------------------------------------------------------------

Qt3DRender::QCameraSelector* frame_graph::get_camera_selector()
{
    return m_camera_selector;
}

//------------------------------------------------------------------------------

void frame_graph::set_camera(Qt3DCore::QEntity* _camera)
{
    m_camera_selector->setCamera(_camera);
}

//------------------------------------------------------------------------------

void frame_graph::set_clear_color(const QColor& _color)
{
    m_clear_buffers->setClearColor(_color);
}

//------------------------------------------------------------------------------

void frame_graph::set_buffers_to_clear(Qt3DRender::QClearBuffers::BufferType _buffers)
{
    m_clear_buffers->setBuffers(_buffers);
}

//------------------------------------------------------------------------------

QVector<Qt3DRender::QFrameGraphNode*> frame_graph::get_all_nodes()
{
    QVector<Qt3DRender::QFrameGraphNode*> result;
    get_all_nodes_rec(result, this);

    return result;
}

//------------------------------------------------------------------------------

void frame_graph::get_all_nodes_rec(
    QVector<Qt3DRender::QFrameGraphNode*>& _nodes,
    Qt3DRender::QFrameGraphNode* _current_node
)
{
    auto children = _current_node->children();
    if(!children.isEmpty())
    {
        for(auto& i : children)
        {
            if(qobject_cast<Qt3DRender::QFrameGraphNode*>(i) != nullptr)
            {
                _nodes.push_back(qobject_cast<Qt3DRender::QFrameGraphNode*>(i));
                get_all_nodes_rec(_nodes, qobject_cast<Qt3DRender::QFrameGraphNode*>(i));
            }
        }
    }
}

//------------------------------------------------------------------------------

void frame_graph::add_node(Qt3DRender::QFrameGraphNode* _node, Qt3DRender::QFrameGraphNode* _parent)
{
    auto nodes = this->get_all_nodes();
    for(auto& node : nodes)
    {
        if(qobject_cast<Qt3DRender::QFrameGraphNode*>(node)->parentFrameGraphNode() == _parent)
        {
            node->setParent(_node);
        }
    }

    _node->setParent(_parent);
}

} // namespace sight::viz::qt3d::core
