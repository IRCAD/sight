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

#include <algorithm>
#include <service/op.hpp>

#include <utest/service_fixture.hpp>

#include <core/type.hpp>

#include <data/image.hpp>

#include <viz/scene2d/adaptor.hpp>
#include <viz/scene2d/data/event.hpp>
#include <viz/scene2d/data/viewport.hpp>
#include <viz/scene2d/graphics_view.hpp>
#include <viz/scene2d/render.hpp>

#include <doctest/doctest.h>

#include <QGraphicsEllipseItem>
#include <QGraphicsSimpleTextItem>
#include <QPointF>

namespace
{

struct histogram_fixture : public sight::utest::service_fixture
{
    histogram_fixture() :
        sight::utest::service_fixture(
            "sight::module::viz::scene2d::adaptor::histogram"
        )
    {
        m_service->set_id("histogram");

        // Container
        m_container = sight::service::add("sight::module::ui::frame");

        sight::service::config_t container_config;
        container_config.put("gui.frame.name", "histogramTest");
        container_config.put("gui.frame.min_size.<xmlattr>.width", 640);
        container_config.put("gui.frame.min_size.<xmlattr>.height", 480);
        container_config.put(
            "registry.view.<xmlattr>.sid",
            "histogram_render"
        );

        m_container->configure(container_config);
        m_container->start().get();

        // Render
        m_render = sight::service::add(
            "sight::viz::scene2d::render",
            "histogram_render"
        );

        const std::string render_config =
            "<scene>"
            "    <scene x='0' y='0' width='256' height='256'/>"
            "    <axis id='xAxis' origin='0' scale='1' scaleType='LINEAR'/>"
            "    <axis id='yAxis' origin='0' scale='1' scaleType='LINEAR'/>"
            "    <adaptor uid='histogram'/>"
            "</scene>";

        m_render->set_config(render_config);
        m_render->configure();
        m_render->start().get();

        // Viewport
        m_viewport =
            std::make_shared<sight::viz::scene2d::data::viewport>();

        m_viewport->set_x(0.);
        m_viewport->set_y(0.);
        m_viewport->set_width(256.);
        m_viewport->set_height(256.);

        // Image containing every value from 0 to 255.
        m_image = std::make_shared<sight::data::image>();

        m_image->resize(
            {16, 16, 1},
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );

        m_image->set_spacing({1., 1., 1.});
        m_image->set_origin({0., 0., 0.});

        {
            const auto lock = m_image->dump_lock();

            std::size_t index = 0;

            for(std::uint8_t& value : m_image->range<std::uint8_t>())
            {
                value = static_cast<std::uint8_t>(index++);
            }
        }

        m_service->set_input(m_image, "image");
        m_service->set_inout(m_viewport, "viewport");

        const std::string histogram_config =
            "<config "
            "xAxis='xAxis' "
            "yAxis='yAxis' "
            "color='#FFFFFF'>"
            "    <cursor "
            "        color='#FFFFFF' "
            "        borderColor='#FFFFFF' "
            "        size='6' "
            "        textColor='#FFFFFF' "
            "        fontSize='8'/>"
            "</config>";

        m_service->set_config(histogram_config);
        m_service->configure();
        m_service->start().get();
    }

    ~histogram_fixture() override
    {
        if(m_service->started())
        {
            m_service->stop().get();
        }

        if(m_render)
        {
            if(m_render->started())
            {
                m_render->stop().get();
            }

            sight::service::remove(m_render);
        }

        if(m_container)
        {
            if(m_container->started())
            {
                m_container->stop().get();
            }

            sight::service::remove(m_container);
        }
    }

    sight::service::base::sptr m_container;
    sight::service::base::sptr m_render;

    sight::data::image::sptr m_image;
    sight::viz::scene2d::data::viewport::sptr m_viewport;
};

} // namespace
TEST_CASE_FIXTURE(
    histogram_fixture,
    "histogram_process_interaction"
)
{
    using event_t = sight::viz::scene2d::data::event;

    auto render =
        std::dynamic_pointer_cast<sight::viz::scene2d::render>(m_render);

    auto histogram =
        std::dynamic_pointer_cast<sight::viz::scene2d::adaptor>(m_service);

    REQUIRE(render != nullptr);
    REQUIRE(histogram != nullptr);

    const auto make_event =
        [&](auto _type,
            double _x,
            double _y,
            event_t::modifier _modifier = event_t::no_modifier)
        {
            const QPoint point =
                render->get_view()->mapFromScene(QPointF(_x, _y));

            event_t event;
            event.set_type(_type);
            event.set_modifier(_modifier);

            event.set_coord(
        {
            static_cast<double>(point.x()),
            static_cast<double>(point.y())
        });

            return event;
        };

    const auto cursor_visible =
        [&]()
        {
            const auto items = render->get_scene()->items();

            return std::ranges::any_of(
                items,
                [](QGraphicsItem* _item)
        {
            const auto* ellipse =
                dynamic_cast<QGraphicsEllipseItem*>(_item);

            return ellipse != nullptr && ellipse->isVisible();
        });
        };

    const auto label_visible =
        [&]()
        {
            const auto items = render->get_scene()->items();

            return std::ranges::any_of(
                items,
                [](QGraphicsItem* _item)
        {
            const auto* label =
                dynamic_cast<QGraphicsSimpleTextItem*>(_item);

            return label != nullptr && label->isVisible();
        });
        };
    // vertical scaling
    {
        auto event = make_event(
            event_t::mouse_wheel_up,
            128.,
            0.
        );

        histogram->process_interaction(event);

        CHECK(event.is_accepted());
    }

    // Normal wheel down
    {
        auto event = make_event(
            event_t::mouse_wheel_down,
            128.,
            0.
        );

        histogram->process_interaction(event);

        CHECK(event.is_accepted());
    }

    // fast vertical scaling
    {
        auto event = make_event(
            event_t::mouse_wheel_up,
            128.,
            0.,
            event_t::shift_modifier
        );

        histogram->process_interaction(event);

        CHECK(event.is_accepted());
    }

    // Shift + wheel down
    {
        auto event = make_event(
            event_t::mouse_wheel_down,
            128.,
            0.,
            event_t::shift_modifier
        );

        histogram->process_interaction(event);

        CHECK(event.is_accepted());
    }

    // increase histogram bin width
    {
        auto event = make_event(
            event_t::mouse_wheel_up,
            128.,
            0.,
            event_t::control_modifier
        );

        histogram->process_interaction(event);

        // This branch intentionally doesn't accept the event.
        CHECK_FALSE(event.is_accepted());
    }

    // decrease histogram bin width
    {
        auto event = make_event(
            event_t::mouse_wheel_down,
            128.,
            0.,
            event_t::control_modifier
        );

        histogram->process_interaction(event);

        CHECK_FALSE(event.is_accepted());
    }

    // Enter histogram
    {
        auto event = make_event(
            event_t::enter_event,
            128.,
            0.
        );

        histogram->process_interaction(event);

        CHECK(cursor_visible());
        CHECK_FALSE(label_visible());
    }

    // Mouse move while inside
    {
        auto event = make_event(
            event_t::mouse_move,
            100.,
            0.
        );

        histogram->process_interaction(event);

        CHECK(cursor_visible());
        CHECK_FALSE(label_visible());
    }

    // Mouse press
    {
        auto event = make_event(
            event_t::mouse_button_press,
            100.,
            0.
        );

        histogram->process_interaction(event);

        CHECK(cursor_visible());
        CHECK(label_visible());
    }

    // Mouse move while interacting
    {
        auto event = make_event(
            event_t::mouse_move,
            150.,
            0.
        );

        histogram->process_interaction(event);

        CHECK(cursor_visible());
        CHECK(label_visible());
    }

    // Release
    {
        auto event = make_event(
            event_t::mouse_button_release,
            150.,
            0.
        );

        histogram->process_interaction(event);

        CHECK(cursor_visible());
        CHECK_FALSE(label_visible());
    }

    // Move outside histogram range
    {
        auto event = make_event(
            event_t::mouse_move,
            -10.,
            0.
        );

        histogram->process_interaction(event);

        CHECK_FALSE(cursor_visible());
        CHECK_FALSE(label_visible());
    }

    // Enter again
    {
        auto event = make_event(
            event_t::enter_event,
            128.,
            0.
        );

        histogram->process_interaction(event);

        CHECK(cursor_visible());
    }

    // Leave
    {
        auto event = make_event(
            event_t::leave_event,
            128.,
            0.
        );

        histogram->process_interaction(event);

        CHECK_FALSE(cursor_visible());
        CHECK_FALSE(label_visible());
    }

    // Stopped service
    {
        m_service->stop().get();

        auto event = make_event(
            event_t::mouse_wheel_up,
            128.,
            0.
        );

        histogram->process_interaction(event);

        CHECK_FALSE(event.is_accepted());
    }
}
