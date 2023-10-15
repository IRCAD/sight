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

#include "viz/qt3d/core/FrameGraph.hpp"

#include <QCameraSelector>
#include <QClearBuffers>
#include <QCullFace>
#include <QDepthTest>
#include <QRenderStateSet>
#include <QRenderSurfaceSelector>
#include <QVector3D>
#include <QViewport>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::qt3dTest::ut::frame_graph_test);

namespace sight::viz::qt3dTest::ut
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

void frame_graph_test::initializeFrameGraph()
{
    test_application app;

    auto* frameGraph = new sight::viz::qt3d::core::FrameGraph();

    ASSERT_NOT_NULL(frameGraph);

    // Asserts QRenderSurfaceSelector exists.
    CPPUNIT_ASSERT_EQUAL(frameGraph->children().size(), 1);

    auto* renderSurfaceSelector = qobject_cast<Qt3DRender::QRenderSurfaceSelector*>(frameGraph->childNodes()[0]);
    ASSERT_NOT_NULL(renderSurfaceSelector);

    // Asserts QViewport exists.
    CPPUNIT_ASSERT_EQUAL(renderSurfaceSelector->children().size(), 1);

    auto* viewport = qobject_cast<Qt3DRender::QViewport*>(renderSurfaceSelector->childNodes()[0]);
    ASSERT_NOT_NULL(viewport);

    // Asserts QCameraSelector exists.
    CPPUNIT_ASSERT_EQUAL(viewport->children().size(), 1);

    auto* cameraSelector = qobject_cast<Qt3DRender::QCameraSelector*>(viewport->childNodes()[0]);
    ASSERT_NOT_NULL(cameraSelector);

    // Asserts QClearBuffers exists.
    CPPUNIT_ASSERT_EQUAL(cameraSelector->children().size(), 1);

    auto* clearBuffers = qobject_cast<Qt3DRender::QClearBuffers*>(cameraSelector->childNodes()[0]);
    ASSERT_NOT_NULL(clearBuffers);

    // Asserts QRenderStateSet exists.
    CPPUNIT_ASSERT_EQUAL(clearBuffers->children().size(), 1);

    auto* renderStateSet = qobject_cast<Qt3DRender::QRenderStateSet*>(clearBuffers->childNodes()[0]);
    ASSERT_NOT_NULL(renderStateSet);

    // Asserts renderStateSet has right render states.
    CPPUNIT_ASSERT_EQUAL(renderStateSet->renderStates().size(), 2);

    auto* culling = qobject_cast<Qt3DRender::QCullFace*>(renderStateSet->renderStates()[0]);
    ASSERT_NOT_NULL(culling);
    CPPUNIT_ASSERT_EQUAL(culling->mode(), Qt3DRender::QCullFace::NoCulling);

    auto* depthTest = qobject_cast<Qt3DRender::QDepthTest*>(renderStateSet->renderStates()[1]);
    ASSERT_NOT_NULL(depthTest);
    CPPUNIT_ASSERT_EQUAL(depthTest->depthFunction(), Qt3DRender::QDepthTest::Less);

    delete frameGraph;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3dTest::ut
