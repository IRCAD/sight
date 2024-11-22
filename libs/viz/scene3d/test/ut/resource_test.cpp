/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "resource_test.hpp"

#include <data/image.hpp>
#include <data/transfer_function.hpp>

#include <utest_data/generator/image.hpp>

#include <viz/scene3d/texture.hpp>
#include <viz/scene3d/transfer_function.hpp>
#include <viz/scene3d/utils.hpp>

#include <OGRE/OgreRenderWindow.h>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::viz::scene3d::ut::resource_test);

namespace sight::viz::scene3d::ut
{

static Ogre::RenderWindow* s_window = nullptr;

//------------------------------------------------------------------------------

void resource_test::setUp()
{
    if(s_window == nullptr)
    {
    }
}

//------------------------------------------------------------------------------

void resource_test::tearDown()
{
}

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

void resource_test::texture_test()
{
    data::image::sptr image = std::make_shared<data::image>();

    utest_data::generator::image::generate_image(
        image,
        {40, 40, 40},
        {1., 1., 1.},
        {0., 0., 0.},
        core::type::UINT8,
        data::image::pixel_format_t::gray_scale
    );
    image->set_id("image1");

    {
        auto texture1_instance1 = std::make_shared<sight::viz::scene3d::texture>(image);
        auto texture1_instance2 = std::make_shared<sight::viz::scene3d::texture>(image);
        CPPUNIT_ASSERT_EQUAL(texture1_instance1->get(), texture1_instance2->get());

        texture1_instance1->update();

        auto texture2_instance1 = std::make_shared<sight::viz::scene3d::texture>(image, "2");
        CPPUNIT_ASSERT(texture1_instance1->get() != texture2_instance1->get());

        auto texture2_instance2 = std::make_shared<sight::viz::scene3d::texture>(image, "2");
        CPPUNIT_ASSERT_EQUAL(texture2_instance1->get(), texture2_instance2->get());

        auto texture2_instance3 = std::make_shared<sight::viz::scene3d::texture>(image, "2");
        CPPUNIT_ASSERT_EQUAL(texture2_instance1->get(), texture2_instance3->get());

        texture2_instance3.reset();
        texture2_instance2.reset();
        texture2_instance1.reset();

        texture2_instance1 = std::make_shared<sight::viz::scene3d::texture>(image, "2");
        texture2_instance2 = std::make_shared<sight::viz::scene3d::texture>(image, "2");
        CPPUNIT_ASSERT_EQUAL(texture2_instance1->get(), texture2_instance2->get());
    }
}

//------------------------------------------------------------------------------

void resource_test::tf_test()
{
    data::transfer_function::sptr tf = data::transfer_function::create_default_tf();
    tf->set_id("default");

    {
        auto tf_instance1 = std::make_shared<sight::viz::scene3d::transfer_function>(tf);
        tf_instance1->update();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::viz::scene3d::ut
