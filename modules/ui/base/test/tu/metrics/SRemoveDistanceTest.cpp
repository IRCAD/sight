/************************************************************************
 *
 * Copyright (C) 2022 IRCAD France
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

#include <service/base.hpp>

#include <ui/base/dialog/ISelectorDialog.hpp>
#include <ui/base/registry/macros.hpp>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::ui::base::metrics::ut::SRemoveDistanceTest);

namespace sight::module::ui::base::metrics::ut
{

class DummySelectorDialog : public sight::ui::base::dialog::ISelectorDialog
{
public:

    explicit DummySelectorDialog(sight::ui::base::factory::Key /*key*/)
    {
    }

    //------------------------------------------------------------------------------

    void setSelections(std::vector<std::string> /*_selections*/) override
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

    void addCustomButton(const std::string& /*label*/, std::function<void()> /*clickedFn*/) override
    {
    }

    static std::string choice;

    //------------------------------------------------------------------------------

    std::string show() override
    {
        return choice;
    }
};

std::string DummySelectorDialog::choice;

SIGHT_REGISTER_GUI(DummySelectorDialog, sight::ui::base::dialog::ISelectorDialog::REGISTRY_KEY);

//------------------------------------------------------------------------------

void SRemoveDistanceTest::setUp()
{
    m_removeDistance = service::add("sight::module::ui::base::metrics::SRemoveDistance");
    CPPUNIT_ASSERT_MESSAGE(
        "Failed to create service 'sight::module::ui::base::metrics::SRemoveDistance'",
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
    auto res = data::PointList::New();
    res->setPoints(v);
    return res;
}

//------------------------------------------------------------------------------

static data::Image::sptr createImage()
{
    auto image = data::Image::New();
    image->resize({1, 2, 3}, core::Type::UINT8, data::Image::RGB);

    auto createPl = [](const std::vector<data::Point::sptr>& v)
                    {
                        auto res = data::PointList::New();
                        res->setPoints(v);
                        return res;
                    };

    auto distances = data::Vector::New();
    distances->assign(
        {
            createPl({data::Point::New(0.), data::Point::New(1.)}),
            createPl({data::Point::New(0.), data::Point::New(2.)}),
            createPl({data::Point::New(0.), data::Point::New(3.)})
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

    DummySelectorDialog::choice = "ALL";
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

    DummySelectorDialog::choice = "2 mm";
    CPPUNIT_ASSERT_NO_THROW(m_removeDistance->update().get());

    auto distances = data::helper::MedicalImage::getDistances(*image);
    CPPUNIT_ASSERT_EQUAL(std::size_t(2), distances->size());
    CPPUNIT_ASSERT_EQUAL(
        *createPl({data::Point::New(0.), data::Point::New(1.)}),
        *data::PointList::dynamicCast((*distances)[0])
    );
    CPPUNIT_ASSERT_EQUAL(
        *createPl({data::Point::New(0.), data::Point::New(3.)}),
        *data::PointList::dynamicCast((*distances)[1])
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
        *createPl({data::Point::New(0.), data::Point::New(1.)}),
        *data::PointList::dynamicCast((*distances)[0])
    );
    CPPUNIT_ASSERT_EQUAL(
        *createPl({data::Point::New(0.), data::Point::New(2.)}),
        *data::PointList::dynamicCast((*distances)[1])
    );
}

//------------------------------------------------------------------------------

} // namespace sight::module::ui::base::metrics::ut
