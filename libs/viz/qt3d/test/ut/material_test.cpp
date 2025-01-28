/************************************************************************
 *
 * Copyright (C) 2020-2025 IRCAD France
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
CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::qt3d_test::ut::material_test);

namespace sight::viz::qt3d_test::ut
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

void material_test::initialize_material()
{
    test_application app;

    auto sight_material = std::make_shared<data::material>();
    auto* qt3d_material = new viz::qt3d::data::material();

    // Initializes qt3dMaterial according to sightMaterial.
    qt3d_material->update_polygon_mode(sight_material->get_representation_mode());
    qt3d_material->update_options_mode(sight_material->get_options_mode());
    qt3d_material->set_shading(static_cast<int>(sight_material->get_shading_mode()));
    qt3d_material->update_rgba_mode(sight_material);

    // Asserts qt3dMaterial RBG is equal to sightMaterial RGB (Approximately equal due to float comparaison).
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sight_material->ambient()->red()),
        qt3d_material->get_ambient().redF(),
        0.01F
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sight_material->ambient()->green()),
        qt3d_material->get_ambient().greenF(),
        0.01F
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sight_material->ambient()->blue()),
        qt3d_material->get_ambient().blueF(),
        0.01F
    );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sight_material->diffuse()->red()),
        qt3d_material->get_diffuse().redF(),
        0.01F
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sight_material->diffuse()->green()),
        qt3d_material->get_diffuse().greenF(),
        0.01F
    );
    CPPUNIT_ASSERT_DOUBLES_EQUAL(
        static_cast<float>(sight_material->diffuse()->blue()),
        qt3d_material->get_diffuse().blueF(),
        0.01F
    );

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2F, qt3d_material->get_specular().x(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2F, qt3d_material->get_specular().y(), 0.00001);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.2F, qt3d_material->get_specular().z(), 0.00001);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(25.0F, qt3d_material->get_shininess(), 0.00001);

    CPPUNIT_ASSERT_EQUAL(1, qt3d_material->effect()->techniques().size());
    auto* tech = qt3d_material->effect()->techniques()[0];

    CPPUNIT_ASSERT_EQUAL(4, tech->renderPasses().size());
    CPPUNIT_ASSERT_EQUAL(3, tech->parameters().size());

    // Asserts qt3dMaterial and sightMaterial rendering options are equals.
    auto* normal_pass      = tech->renderPasses()[0];
    auto* cell_normal_pass = tech->renderPasses()[1];
    auto* render_pass      = tech->renderPasses()[2];
    auto* edge_render_pass = tech->renderPasses()[3];

    // Default polygonMode must be set to SURFACE.
    CPPUNIT_ASSERT_EQUAL(
        Qt3DRender::QRasterMode::Fill,
        qobject_cast<Qt3DRender::QRasterMode*>(render_pass->renderStates()[0])->rasterMode()
    );
    CPPUNIT_ASSERT_EQUAL(false, edge_render_pass->isEnabled());

    // Default optionMode must be set to STANDARD.
    CPPUNIT_ASSERT_EQUAL(false, normal_pass->isEnabled());
    CPPUNIT_ASSERT_EQUAL(false, cell_normal_pass->isEnabled());

    //Default lightingMode must be set to PHONG.
    CPPUNIT_ASSERT_EQUAL(
        static_cast<int>(sight_material->get_shading_mode()),
        qvariant_cast<int>(tech->parameters()[2]->value())
    );

    delete qt3d_material;
}

//------------------------------------------------------------------------------

} // namespace sight::viz::qt3d_test::ut
