/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <core/thread/worker.hpp>
#include <data/image_series.hpp>
#include <ui/qt/slice_selector.hpp>
#include <ui/test/gui_fixture.hpp>

#include <doctest/doctest.h>

#include <QComboBox>
#include <QImage>
#include <QSlider>

//------------------------------------------------------------------------------

static bool has_fiducial_mark(QSlider& _slider)
{
    _slider.resize(320, 40);
    QImage snapshot(_slider.size(), QImage::Format_RGB32);
    _slider.render(&snapshot);
    for(int y = 0 ; y < snapshot.height() ; ++y)
    {
        for(int x = 0 ; x < snapshot.width() ; ++x)
        {
            if(snapshot.pixelColor(x, y) == QColor(Qt::magenta))
            {
                return true;
            }
        }
    }

    return false;
}

TEST_SUITE("sight::module::ui::qt::image::slice_index_position_editor")
{
    TEST_CASE_FIXTURE(sight::ui::test::gui_fixture, "marks_follow_image_and_orientation")
    {
        for(const std::string orientation : {"axial", "sagittal", "frontal"})
        {
            CAPTURE(orientation);
            test_service(
                "sight::module::ui::qt::image::slice_index_position_editor",
                [orientation](const sight::service::base::sptr& _service)
            {
                auto first  = std::make_shared<sight::data::image_series>();
                auto second = std::make_shared<sight::data::image_series>();
                for(const auto& image : {first, second})
                {
                    image->resize(
                        {16, 20, 24},
                        sight::core::type::UINT8,
                        sight::data::image::pixel_format_t::gray_scale
                    );
                    image->set_spacing({1., 1., 1.});
                    image->set_origin({0., 0., 0.});
                }

                sight::data::fiducials_series::fiducial fiducial;
                fiducial.shape_type   = sight::data::fiducials_series::shape::point;
                fiducial.contour_data = {{.x = 3., .y = 4., .z = 5.}};
                sight::data::fiducials_series::fiducial_set group;
                group.color             = sight::vec4f_t {1.F, 0.F, 1.F, 1.F};
                group.fiducial_sequence = {fiducial};
                first->get_fiducials()->set_fiducial_sets({group});

                sight::core::thread::get_default_worker()->post_task<void>(
                    [_service, first, orientation]
                {
                    _service->set_inout(first, "data.image");
                    sight::service::config_t config;
                    config.put("config.<xmlattr>.orientation", orientation);
                    _service->configure(config);
                    _service->start().get();
                }).get();

                const auto selector = find_widget<sight::ui::qt::slice_selector>(_service->get_id());
                REQUIRE(!selector.isNull());
                sight::core::thread::get_default_worker()->post_task<void>(
                    [_service, first, second, selector]
                {
                    auto* slider = selector->findChild<QSlider*>();
                    REQUIRE(slider != nullptr);
                    CHECK(has_fiducial_mark(*slider));

                    const auto update_type = _service->slot("updateSliceType");
                    REQUIRE(update_type != nullptr);
                    update_type->run(0, 2);
                    CHECK(has_fiducial_mark(*slider));

                    _service->set_inout(second, "data.image");
                    _service->swap_key("data.image", first).get();
                    CHECK_FALSE(has_fiducial_mark(*slider));

                    _service->set_inout(first, "data.image");
                    _service->swap_key("data.image", second).get();
                    CHECK(has_fiducial_mark(*slider));

                    sight::ui::qt::slice_selector extra(true, false, std::uint8_t {1});
                    int notifications = 0;
                    extra.set_change_type_callback([&notifications](int){++notifications;});
                    extra.set_type_selection(2);
                    extra.set_type_selection(1);
                    CHECK(notifications == 0);
                    extra.findChild<QComboBox*>()->setCurrentIndex(0);
                    CHECK(notifications == 1);
                }).get();
            });
        }
    }
}
