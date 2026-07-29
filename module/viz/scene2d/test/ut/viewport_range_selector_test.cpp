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

#include <service/op.hpp>

#include <utest/service_fixture.hpp>

#include <viz/scene2d/adaptor.hpp>
#include <viz/scene2d/data/event.hpp>
#include <viz/scene2d/data/viewport.hpp>
#include <viz/scene2d/graphics_view.hpp>
#include <viz/scene2d/render.hpp>

#include <doctest/doctest.h>

#include <QPointF>

namespace
{

struct view_port_fixture : public sight::utest::service_fixture
{
    view_port_fixture() :
        sight::utest::service_fixture("sight::module::viz::scene2d::adaptor::viewport_range_selector")
    {
        m_service->set_id("viewport_selector");

        m_container = sight::service::add("sight::module::ui::frame");

        sight::service::config_t container_config;
        container_config.put("gui.frame.name", "viewportRangeSelectorTest");
        container_config.put("gui.frame.min_size.<xmlattr>.width", 640);
        container_config.put("gui.frame.min_size.<xmlattr>.height", 480);
        container_config.put(
            "registry.view.<xmlattr>.sid",
            "viewport_selector_render"
        );

        m_container->configure(container_config);
        m_container->start().get();

        m_render = sight::service::add(
            "sight::viz::scene2d::render",
            "viewport_selector_render"
        );

        const std::string render_config =
            "<scene>"
            "    <scene x='0' y='0' width='200' height='100'/>"
            "    <axis id='xAxis' origin='0' scale='1' scaleType='LINEAR'/>"
            "    <axis id='yAxis' origin='0' scale='1' scaleType='LINEAR'/>"
            "    <adaptor uid='viewport_selector'/>"
            "</scene>";

        m_render->set_config(render_config);
        m_render->configure();
        m_render->start().get();

        m_viewport =
            std::make_shared<sight::viz::scene2d::data::viewport>();

        m_selected_viewport =
            std::make_shared<sight::viz::scene2d::data::viewport>();

        m_service->set_inout(m_viewport, "viewport");
        m_service->set_inout(
            m_selected_viewport,
            "selectedViewport"
        );

        const std::string selector_config =
            "<config "
            "xAxis='xAxis' "
            "yAxis='yAxis' "
            "initialPos='50' "
            "initialWidth='100'/>";

        m_service->set_config(selector_config);
        m_service->configure();
        m_service->start().get();
    }

    ~view_port_fixture() override
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

    sight::viz::scene2d::data::viewport::sptr m_viewport;
    sight::viz::scene2d::data::viewport::sptr m_selected_viewport;
};

} // namespace

TEST_CASE_FIXTURE(view_port_fixture, "viewport_range_selector_process_interaction")
{
    auto render =
        std::dynamic_pointer_cast<sight::viz::scene2d::render>(m_render);

    auto selector =
        std::dynamic_pointer_cast<sight::viz::scene2d::adaptor>(m_service);

    REQUIRE(render != nullptr);
    REQUIRE(selector != nullptr);

    const auto mapped_scene_x =
        [&](double _x)
        {
            const QPoint point =
                render->get_view()->mapFromScene(QPointF(_x, 0.5));

            return render->map_to_scene(
        {
            static_cast<double>(point.x()),
            static_cast<double>(point.y())
        }).x;
        };

    const auto make_event =
        [&](auto _type, double _x)
        {
            const QPoint point =
                render->get_view()->mapFromScene(QPointF(_x, 0.5));

            sight::viz::scene2d::data::event event;
            event.set_type(_type);
            event.set_coord(
        {
            static_cast<double>(point.x()),
            static_cast<double>(point.y())
        });

            return event;
        };

    // Middle hover
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            100.
        );

        selector->process_interaction(event);

        CHECK(
            render->get_view()->cursor().shape()
            == Qt::OpenHandCursor
        );
    }

    // Middle press
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_button_press,
            100.
        );

        selector->process_interaction(event);

        CHECK(
            render->get_view()->cursor().shape()
            == Qt::ClosedHandCursor
        );
    }

    // Middle drag
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            110.
        );

        selector->process_interaction(event);

        const double expected_x = 50. + mapped_scene_x(110.) - mapped_scene_x(100.);
        CHECK(m_selected_viewport->x() == doctest::Approx(expected_x));
    }

    // Middle release
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_button_release,
            110.
        );

        selector->process_interaction(event);

        CHECK(
            render->get_view()->cursor().shape()
            == Qt::OpenHandCursor
        );
    }
    // Left border hover
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            60.
        );

        selector->process_interaction(event);

        CHECK(
            render->get_view()->cursor().shape()
            == Qt::SizeHorCursor
        );
    }

    // Right border hover
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            160.
        );

        selector->process_interaction(event);

        CHECK(
            render->get_view()->cursor().shape()
            == Qt::SizeHorCursor
        );
    }
    // Left border
    {
        auto press = make_event(
            sight::viz::scene2d::data::event::mouse_button_press,
            60.
        );

        selector->process_interaction(press);

        auto move = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            55.
        );

        selector->process_interaction(move);

        CHECK(m_selected_viewport->x() == doctest::Approx(mapped_scene_x(55.)));

        auto release = make_event(
            sight::viz::scene2d::data::event::mouse_button_release,
            55.
        );

        selector->process_interaction(release);
    }

    // Right border
    {
        auto press = make_event(
            sight::viz::scene2d::data::event::mouse_button_press,
            160.
        );

        selector->process_interaction(press);

        auto move = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            170.
        );

        selector->process_interaction(move);

        const double expected_width = mapped_scene_x(170.) - mapped_scene_x(55.);
        CHECK(m_selected_viewport->width() == doctest::Approx(expected_width));

        auto release = make_event(
            sight::viz::scene2d::data::event::mouse_button_release,
            170.
        );

        selector->process_interaction(release);
    }

    // Outside shutter
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            10.
        );

        selector->process_interaction(event);

        CHECK(
            render->get_view()->cursor().shape()
            == Qt::ArrowCursor
        );
    }
    // Press outside shutter
    {
        auto hover = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            10.
        );
        selector->process_interaction(hover);

        auto press = make_event(
            sight::viz::scene2d::data::event::mouse_button_press,
            10.
        );
        selector->process_interaction(press);

        CHECK(
            render->get_view()->cursor().shape()
            == Qt::ArrowCursor
        );
    }
}
