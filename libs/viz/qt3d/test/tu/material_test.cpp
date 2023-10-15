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

#include "material_test.hpp"

#include "test_application.hpp"

#include "viz/qt3d/data/material.hpp"

#include <data/material.hpp>

#include <Qt3DRender/QEffect>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QRasterMode>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QTechnique>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::qt3dTest::ut::material_test);

namespace sight::viz::qt3dTest::ut
{

//------------------------------------------------------------------------------

material_test::material_test()
= default;

//------------------------------------------------------------------------------

material_test::~material_test()
= default;

//------------------------------------------------------------------------------

void material_test::setUp()
{
}

//------------------------------------------------------------------------------

void material_test::tearDown()
{
}

//------------------------------------------------------------------------------

void material_test::initializeMaterial()
{
    test_application app;

    auto sightMaterial = std::make_shared<data::material>();
    auto* qt3dMaterial = new viz::qt3d::data::material();

    // Initializes qt3dMaterial according to sightMaterial.
    qt3dMaterial->updatePolygonMode(sightMaterial->getRepresentationMode());
    qt3dMaterial->updateOptionsMode(sightMaterial->getOptionsMode());
    qt3dMaterial->updateShadingMode(sightMaterial->getShadingMode());
    qt3dMaterial->updateRGBAMode(sightMaterial);

    // Asserts qt3dMaterial RBG is equal to sightMaterial RGB (Approximately equal due to float comparaison).
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sightMaterial->ambient()->red()),
        qt3dMaterial->getAmbient().redF(),
        0.01F
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sightMaterial->ambient()->green()),
        qt3dMaterial->getAmbient().greenF(),
        0.01F
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sightMaterial->ambient()->blue()),
        qt3dMaterial->getAmbient().blueF(),
        0.01F
    );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sightMaterial->diffuse()->red()),
        qt3dMaterial->getDiffuse().redF(),
        0.01F
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sightMaterial->diffuse()->green()),
        qt3dMaterial->getDiffuse().greenF(),
        0.01F
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sightMaterial->diffuse()->blue()),
        qt3dMaterial->getDiffuse().blueF(),
        0.01F
    );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2F, qt3dMaterial->getSpecular().x(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2F, qt3dMaterial->getSpecular().y(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2F, qt3dMaterial->getSpecular().z(), 0.00001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(25.0F, qt3dMaterial->getShininess(), 0.00001);

    CPPUNIT_ASSERT_EQUAL(1, qt3dMaterial->effect()->techniques().size());
    auto* tech = qt3dMaterial->effect()->techniques()[0];

    CPPUNIT_ASSERT_EQUAL(4, tech->renderPasses().size());
    CPPUNIT_ASSERT_EQUAL(3, tech->parameters().size());

    // Asserts qt3dMaterial and sightMaterial rendering options are equals.
    auto* normalPass     = tech->renderPasses()[0];
    auto* cellNormalPass = tech->renderPasses()[1];
    auto* renderPass     = tech->renderPasses()[2];
    auto* edgeRenderPass = tech->renderPasses()[3];

    // Default polygonMode must be set to SURFACE.
    CPPUNIT_ASSERT_EQUAL(
        Qt3DRender::QRasterMode::Fill,
        qobject_cast<Qt3DRender::QRasterMode*>(renderPass->renderStates()[0])->rasterMode()
    );
    CPPUNIT_ASSERT_EQUAL(false, edgeRenderPass->isEnabled());

    // Default optionMode must be set to STANDARD.
    CPPUNIT_ASSERT_EQUAL(false, normalPass->isEnabled());
    CPPUNIT_ASSERT_EQUAL(false, cellNormalPass->isEnabled());

    //Default lightingMode must be set to PHONG.
    CPPUNIT_ASSERT_EQUAL(
        static_cast<int>(sightMaterial->getShadingMode()),
        qvariant_cast<int>(tech->parameters()[2]->value())
    );

    delete qt3dMaterial;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3dTest::ut
