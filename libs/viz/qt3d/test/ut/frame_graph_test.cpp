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

#include "frame_graph_test.hpp"

#include "test_application.hpp"

#include "viz/qt3d/core/frame_graph.hpp"

#include <QCameraSelector>
#include <QClearBuffers>
#include <QCullFace>
#include <QDepthTest>
#include <QRenderStateSet>
#include <QRenderSurfaceSelector>
#include <QVector3D>
#include <QViewport>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::qt3d_test::ut::frame_graph_test);

namespace sight::viz::qt3d_test::ut
{

//------------------------------------------------------------------------------

frame_graph_test::frame_graph_test()
= default;

//------------------------------------------------------------------------------

frame_graph_test::~frame_graph_test()
= default;

//------------------------------------------------------------------------------

void frame_graph_test::setUp()
{
}

//------------------------------------------------------------------------------

void frame_graph_test::tearDown()
{
}

#define ASSERT_NOT_NULL(expr) if((expr) == nullptr){throw std::runtime_error(#expr " is null.");}

//------------------------------------------------------------------------------

void frame_graph_test::initialize_frame_graph()
{
    test_application app;

    auto* frame_graph = new sight::viz::qt3d::core::frame_graph();

    ASSERT_NOT_NULL(frame_graph);

    // Asserts QRenderSurfaceSelector exists.
    CPPUNIT_ASSERT_EQUAL(frame_graph->children().size(), 1);

    auto* render_surface_selector = qobject_cast<Qt3DRender::QRenderSurfaceSelector*>(frame_graph->childNodes()[0]);
    ASSERT_NOT_NULL(render_surface_selector);

    // Asserts QViewport exists.
    CPPUNIT_ASSERT_EQUAL(render_surface_selector->children().size(), 1);

    auto* viewport = qobject_cast<Qt3DRender::QViewport*>(render_surface_selector->childNodes()[0]);
    ASSERT_NOT_NULL(viewport);

    // Asserts QCameraSelector exists.
    CPPUNIT_ASSERT_EQUAL(viewport->children().size(), 1);

    auto* camera_selector = qobject_cast<Qt3DRender::QCameraSelector*>(viewport->childNodes()[0]);
    ASSERT_NOT_NULL(camera_selector);

    // Asserts QClearBuffers exists.
    CPPUNIT_ASSERT_EQUAL(camera_selector->children().size(), 1);

    auto* clear_buffers = qobject_cast<Qt3DRender::QClearBuffers*>(camera_selector->childNodes()[0]);
    ASSERT_NOT_NULL(clear_buffers);

    // Asserts QRenderStateSet exists.
    CPPUNIT_ASSERT_EQUAL(clear_buffers->children().size(), 1);

    auto* render_state_set = qobject_cast<Qt3DRender::QRenderStateSet*>(clear_buffers->childNodes()[0]);
    ASSERT_NOT_NULL(render_state_set);

    // Asserts renderStateSet has right render states.
    CPPUNIT_ASSERT_EQUAL(render_state_set->renderStates().size(), 2);

    auto* culling = qobject_cast<Qt3DRender::QCullFace*>(render_state_set->renderStates()[0]);
    ASSERT_NOT_NULL(culling);
    CPPUNIT_ASSERT_EQUAL(culling->mode(), Qt3DRender::QCullFace::NoCulling);

    auto* depth_test = qobject_cast<Qt3DRender::QDepthTest*>(render_state_set->renderStates()[1]);
    ASSERT_NOT_NULL(depth_test);
    CPPUNIT_ASSERT_EQUAL(depth_test->depthFunction(), Qt3DRender::QDepthTest::Less);

    delete frame_graph;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3d_test::ut
