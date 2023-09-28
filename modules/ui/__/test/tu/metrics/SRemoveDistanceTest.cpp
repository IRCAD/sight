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

#include "SRemoveDistanceTest.hpp"

#include <data/helper/MedicalImage.hpp>
#include <data/Image.hpp>
#include <data/Point.hpp>
#include <data/Vector.hpp>

#include <service/op.hpp>

#include <ui/__/dialog/selector_base.hpp>
#include <ui/__/macros.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::metrics::ut::SRemoveDistanceTest);

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

void SRemoveDistanceTest::setUp()
{
    m_removeDistance = service::add("sight::module::ui::metrics::SRemoveDistance");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::metrics::SRemoveDistance'",
        m_removeDistance
    );
}

//------------------------------------------------------------------------------

void SRemoveDistanceTest::tearDown()
{
    if(!m_removeDistance->isStopped())
    {
        CPPUNIT_ASSERT_NO_THROW(m_removeDistance->stop().get());
    }

    service::remove(m_removeDistance);
}

//------------------------------------------------------------------------------

static data::PointList::sptr createPl(const std::vector<data::Point::sptr>& v)
{
    auto res = std::make_shared<data::PointList>();
    res->setPoints(v);
    return res;
}

//------------------------------------------------------------------------------

static data::Image::sptr createImage()
{
    auto image = std::make_shared<data::Image>();
    image->resize({1, 2, 3}, core::type::UINT8, data::Image::RGB);

    auto createPl = [](const std::vector<data::Point::sptr>& v)
                    {
                        auto res = std::make_shared<data::PointList>();
                        res->setPoints(v);
                        return res;
                    };

    auto distances = std::make_shared<data::Vector>();
    distances->assign(
        {
            createPl({std::make_shared<data::Point>(0.), std::make_shared<data::Point>(1.)}),
            createPl({std::make_shared<data::Point>(0.), std::make_shared<data::Point>(2.)}),
            createPl({std::make_shared<data::Point>(0.), std::make_shared<data::Point>(3.)})
        });
    data::helper::MedicalImage::setDistances(*image, distances);

    return image;
}

//------------------------------------------------------------------------------

void SRemoveDistanceTest::removeAllTest()
{
    auto image = createImage();

    m_removeDistance->setInOut(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->start().get());

    selector_dummy::choice = "ALL";
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->update().get());

    auto distances = data::helper::MedicalImage::getDistances(*image);
    CPPUNIT_ASSERT(distances == nullptr || distances->empty());
}

//------------------------------------------------------------------------------

void SRemoveDistanceTest::removeOneTest()
{
    auto image = createImage();

    m_removeDistance->setInOut(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->start().get());

    selector_dummy::choice = "2 mm";
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->update().get());

    auto distances = data::helper::MedicalImage::getDistances(*image);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), distances->size());
    CPPUNIT_ASSERT_EQUAL(
        *createPl({std::make_shared<data::Point>(0.), std::make_shared<data::Point>(1.)}),
        *std::dynamic_pointer_cast<data::PointList>((*distances)[0])
    );
    CPPUNIT_ASSERT_EQUAL(
        *createPl({std::make_shared<data::Point>(0.), std::make_shared<data::Point>(3.)}),
        *std::dynamic_pointer_cast<data::PointList>((*distances)[1])
    );
}

//------------------------------------------------------------------------------

void SRemoveDistanceTest::removeLastTest()
{
    auto image = createImage();

    m_removeDistance->setInOut(image, "image");
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->configure());
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->start().get());

    m_removeDistance->slot("removeLastDistance")->run();

    auto distances = data::helper::MedicalImage::getDistances(*image);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), distances->size());
    CPPUNIT_ASSERT_EQUAL(
        *createPl({std::make_shared<data::Point>(0.), std::make_shared<data::Point>(1.)}),
        *std::dynamic_pointer_cast<data::PointList>((*distances)[0])
    );
    CPPUNIT_ASSERT_EQUAL(
        *createPl({std::make_shared<data::Point>(0.), std::make_shared<data::Point>(2.)}),
        *std::dynamic_pointer_cast<data::PointList>((*distances)[1])
    );
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::metrics::ut
