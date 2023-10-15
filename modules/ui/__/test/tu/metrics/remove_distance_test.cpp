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

#include <data/helper/MedicalImage.hpp>
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

    void setTitle(std::string /*_title*/) override
    {
    }

    //------------------------------------------------------------------------------

    void setMessage(const std::string& /*msg*/) override
    {
    }

    //------------------------------------------------------------------------------

    void set_multiple(bool /*_multiple*/) override
    {
    }

    //------------------------------------------------------------------------------

    void addCustomButton(const std::string& /*label*/, std::function<void()> /*clickedFn*/) override
    {
    }

    static std::string choice;

    //------------------------------------------------------------------------------

    selector_dummy::selections_t show() override
    {
        return {choice};
    }
};

std::string selector_dummy::choice;

SIGHT_REGISTER_GUI(selector_dummy, sight::ui::dialog::selector_base::REGISTRY_KEY);

//------------------------------------------------------------------------------

void remove_distance_test::setUp()
{
    m_removeDistance = service::add("sight::module::ui::metrics::remove_distance");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::metrics::remove_distance'",
        m_removeDistance
    );
}

//------------------------------------------------------------------------------

void remove_distance_test::tearDown()
{
    if(!m_removeDistance->stopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_removeDistance->stop().get());
    }

    service::remove(m_removeDistance);
}

//------------------------------------------------------------------------------

static data::point_list::sptr createPl(const std::vector<data::point::sptr>& v)
{
    auto res = std::make_shared<data::point_list>();
    res->setPoints(v);
    return res;
}

//------------------------------------------------------------------------------

static data::image::sptr createImage()
{
    auto image = std::make_shared<data::image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::image::RGB);

    auto createPl = [](const std::vector<data::point::sptr>& v)
                    {
                        auto res = std::make_shared<data::point_list>();
                        res->setPoints(v);
                        return res;
                    };

    auto distances = std::make_shared<data::vector>();
    distances->assign(
        {
            createPl({std::make_shared<data::point>(0.), std::make_shared<data::point>(1.)}),
            createPl({std::make_shared<data::point>(0.), std::make_shared<data::point>(2.)}),
            createPl({std::make_shared<data::point>(0.), std::make_shared<data::point>(3.)})
        });
    data::helper::MedicalImage::setDistances(*image, distances);

    return image;
}

//------------------------------------------------------------------------------

void remove_distance_test::removeAllTest()
{
    auto image = createImage();

    m_removeDistance->set_inout(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->start().get());

    selector_dummy::choice = "ALL";
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->update().get());

    auto distances = data::helper::MedicalImage::getDistances(*image);
    CPPUNIT_ASSERT(distances == nullptr || distances->empty());
}

//------------------------------------------------------------------------------

void remove_distance_test::removeOneTest()
{
    auto image = createImage();

    m_removeDistance->set_inout(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->start().get());

    selector_dummy::choice = "2 mm";
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->update().get());

    auto distances = data::helper::MedicalImage::getDistances(*image);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), distances->size());
    CPPUNIT_ASSERT_EQUAL(
        *createPl({std::make_shared<data::point>(0.), std::make_shared<data::point>(1.)}),
        *std::dynamic_pointer_cast<data::point_list>((*distances)[0])
    );
    CPPUNIT_ASSERT_EQUAL(
        *createPl({std::make_shared<data::point>(0.), std::make_shared<data::point>(3.)}),
        *std::dynamic_pointer_cast<data::point_list>((*distances)[1])
    );
}

//------------------------------------------------------------------------------

void remove_distance_test::removeLastTest()
{
    auto image = createImage();

    m_removeDistance->set_inout(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->start().get());

    m_removeDistance->slot("removeLastDistance")->run();

    auto distances = data::helper::MedicalImage::getDistances(*image);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), distances->size());
    CPPUNIT_ASSERT_EQUAL(
        *createPl({std::make_shared<data::point>(0.), std::make_shared<data::point>(1.)}),
        *std::dynamic_pointer_cast<data::point_list>((*distances)[0])
    );
    CPPUNIT_ASSERT_EQUAL(
        *createPl({std::make_shared<data::point>(0.), std::make_shared<data::point>(2.)}),
        *std::dynamic_pointer_cast<data::point_list>((*distances)[1])
    );
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::metrics::ut
