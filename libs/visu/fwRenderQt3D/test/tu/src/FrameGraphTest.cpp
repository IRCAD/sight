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

#include "FrameGraphTest.hpp"

#include "fwRenderQt3D/core/FrameGraph.hpp"

#include <QCameraSelector>
#include <QClearBuffers>
#include <QCullFace>
#include <QDepthTest>
#include <QRenderStateSet>
#include <QRenderSurfaceSelector>
#include <QVector3D>
#include <QViewport>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(::fwRenderQt3DTest::ut::FrameGraphTest);

namespace fwRenderQt3DTest
{

namespace ut
{

//------------------------------------------------------------------------------

FrameGraphTest::FrameGraphTest()
{

}

//------------------------------------------------------------------------------

FrameGraphTest::~FrameGraphTest()
{
}

//------------------------------------------------------------------------------

void FrameGraphTest::setUp()
{
}

//------------------------------------------------------------------------------

void FrameGraphTest::tearDown()
{
}

//------------------------------------------------------------------------------

void FrameGraphTest::initializeFrameGraph()
{
    auto frameGraph = new fwRenderQt3D::core::FrameGraph();

    CPPUNIT_ASSERT(frameGraph != nullptr);

    // Asserts QRenderSurfaceSelector exists.
    CPPUNIT_ASSERT_EQUAL(frameGraph->children().size(), 1);

    auto renderSurfaceSelector = qobject_cast < Qt3DRender::QRenderSurfaceSelector* >(frameGraph->childNodes()[0]);
    CPPUNIT_ASSERT(renderSurfaceSelector != nullptr);

    // Asserts QViewport exists.
    CPPUNIT_ASSERT_EQUAL(renderSurfaceSelector->children().size(), 1);

    auto viewport = qobject_cast < Qt3DRender::QViewport* >(renderSurfaceSelector->childNodes()[0]);
    CPPUNIT_ASSERT(viewport != nullptr);

    // Asserts QCameraSelector exists.
    CPPUNIT_ASSERT_EQUAL(viewport->children().size(), 1);

    auto cameraSelector = qobject_cast < Qt3DRender::QCameraSelector* >(viewport->childNodes()[0]);
    CPPUNIT_ASSERT(cameraSelector != nullptr);

    // Asserts QClearBuffers exists.
    CPPUNIT_ASSERT_EQUAL(cameraSelector->children().size(), 1);

    auto clearBuffers = qobject_cast < Qt3DRender::QClearBuffers* >(cameraSelector->childNodes()[0]);
    CPPUNIT_ASSERT(clearBuffers != nullptr);

    // Asserts QRenderStateSet exists.
    CPPUNIT_ASSERT_EQUAL(clearBuffers->children().size(), 1);

    auto renderStateSet = qobject_cast < Qt3DRender::QRenderStateSet* >(clearBuffers->childNodes()[0]);
    CPPUNIT_ASSERT(renderStateSet != nullptr);

    // Asserts renderStateSet has right render states.
    CPPUNIT_ASSERT_EQUAL(renderStateSet->renderStates().size(), 2);

    auto culling = qobject_cast < Qt3DRender::QCullFace* >(renderStateSet->renderStates()[0]);
    CPPUNIT_ASSERT(culling != nullptr);
    CPPUNIT_ASSERT_EQUAL(culling->mode(), Qt3DRender::QCullFace::NoCulling);

    auto depthTest = qobject_cast < Qt3DRender::QDepthTest* >(renderStateSet->renderStates()[1]);
    CPPUNIT_ASSERT(depthTest != nullptr);
    CPPUNIT_ASSERT_EQUAL(depthTest->depthFunction(), Qt3DRender::QDepthTest::Less);

    delete depthTest;
    delete culling;
    delete renderStateSet;
    delete clearBuffers;
    delete cameraSelector;
    delete viewport;
    delete renderSurfaceSelector;
    delete frameGraph;

}

//------------------------------------------------------------------------------

} // namespace ut.

} // namespace fwRenderQt3DTest.
