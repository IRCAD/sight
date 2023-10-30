/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "remove_distance_test.hpp"

#include <data/helper/medical_image.hpp>
#include <data/image.hpp>
#include <data/point.hpp>
#include <data/vector.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/selector_base.hpp>
#include <ui/__/macros.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::metrics::ut::remove_distance_test);

namespace sight::module::ui::metrics::ut
{

class selector_dummy : public sight::ui::dialog::selector_base
{
public:

    //------------------------------------------------------------------------------

    void set_choices_preset(choices_preset_t /*_selections*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_title(std::string /*_title*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_message(const std::string& /*msg*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_multiple(bool /*_multiple*/) override
    {
    }

    //------------------------------------------------------------------------------

    void add_custom_button(const std::string& /*label*/, std::function<void()> /*clickedFn*/) override
    {
    }

    static std::string s_choice;

    //------------------------------------------------------------------------------

    selector_dummy::selections_t show() override
    {
        return {s_choice};
    }
};

std::string selector_dummy::s_choice;

SIGHT_REGISTER_GUI(selector_dummy, sight::ui::dialog::selector_base::REGISTRY_KEY);

//------------------------------------------------------------------------------

void remove_distance_test::setUp()
{
    m_remove_distance = service::add("sight::module::ui::metrics::remove_distance");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::metrics::remove_distance'",
        m_remove_distance
    );
}

//------------------------------------------------------------------------------

void remove_distance_test::tearDown()
{
    if(!m_remove_distance->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_remove_distance->stop().get());
    }

    service::remove(m_remove_distance);
}

//------------------------------------------------------------------------------

static data::point_list::sptr create_pl(const std::vector<data::point::sptr>& _v)
{
    auto res = std::make_shared<data::point_list>();
    res->set_points(_v);
    return res;
}

//------------------------------------------------------------------------------

static data::image::sptr create_image()
{
    auto image = std::make_shared<data::image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::image::rgb);

    auto create_pl = [](const std::vector<data::point::sptr>& _v)
                     {
                         auto res = std::make_shared<data::point_list>();
                         res->set_points(_v);
                         return res;
                     };

    auto distances = std::make_shared<data::vector>();
    distances->assign(
        {
            create_pl({std::make_shared<data::point>(0.), std::make_shared<data::point>(1.)}),
            create_pl({std::make_shared<data::point>(0.), std::make_shared<data::point>(2.)}),
            create_pl({std::make_shared<data::point>(0.), std::make_shared<data::point>(3.)})
        });
    data::helper::medical_image::set_distances(*image, distances);

    return image;
}

//------------------------------------------------------------------------------

void remove_distance_test::remove_all_test()
{
    auto image = create_image();

    m_remove_distance->set_inout(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_remove_distance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_remove_distance->start().get());

    selector_dummy::s_choice = "ALL";
    CPPUNIT_ASSERT_NO_THROW(m_remove_distance->update().get());

    auto distances = data::helper::medical_image::get_distances(*image);
    CPPUNIT_ASSERT(distances == nullptr || distances->empty());
}

//------------------------------------------------------------------------------

void remove_distance_test::remove_one_test()
{
    auto image = create_image();

    m_remove_distance->set_inout(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_remove_distance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_remove_distance->start().get());

    selector_dummy::s_choice = "2 mm";
    CPPUNIT_ASSERT_NO_THROW(m_remove_distance->update().get());

    auto distances = data::helper::medical_image::get_distances(*image);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), distances->size());
    CPPUNIT_ASSERT_EQUAL(
        *create_pl({std::make_shared<data::point>(0.), std::make_shared<data::point>(1.)}),
        *std::dynamic_pointer_cast<data::point_list>((*distances)[0])
    );
    CPPUNIT_ASSERT_EQUAL(
        *create_pl({std::make_shared<data::point>(0.), std::make_shared<data::point>(3.)}),
        *std::dynamic_pointer_cast<data::point_list>((*distances)[1])
    );
}

//------------------------------------------------------------------------------

void remove_distance_test::remove_last_test()
{
    auto image = create_image();

    m_remove_distance->set_inout(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_remove_distance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_remove_distance->start().get());

    m_remove_distance->slot("removeLastDistance")->run();

    auto distances = data::helper::medical_image::get_distances(*image);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), distances->size());
    CPPUNIT_ASSERT_EQUAL(
        *create_pl({std::make_shared<data::point>(0.), std::make_shared<data::point>(1.)}),
        *std::dynamic_pointer_cast<data::point_list>((*distances)[0])
    );
    CPPUNIT_ASSERT_EQUAL(
        *create_pl({std::make_shared<data::point>(0.), std::make_shared<data::point>(2.)}),
        *std::dynamic_pointer_cast<data::point_list>((*distances)[1])
    );
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::metrics::ut
