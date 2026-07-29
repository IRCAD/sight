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

#include <data/transfer_function.hpp>

#include <viz/scene2d/adaptor.hpp>
#include <viz/scene2d/data/event.hpp>
#include <viz/scene2d/data/viewport.hpp>
#include <viz/scene2d/graphics_view.hpp>
#include <viz/scene2d/render.hpp>

#include <doctest/doctest.h>

#include <QPointF>

namespace
{

struct transfer_function_fixture : public sight::utest::service_fixture
{
    transfer_function_fixture() :
        sight::utest::service_fixture(
            "sight::module::viz::scene2d::adaptor::transfer_function"
        )
    {
        m_service->set_id("transfer_function");

        // Container
        m_container = sight::service::add("sight::module::ui::frame");

        sight::service::config_t container_config;
        container_config.put("gui.frame.name", "transferFunctionTest");
        container_config.put("gui.frame.min_size.<xmlattr>.width", 640);
        container_config.put("gui.frame.min_size.<xmlattr>.height", 480);
        container_config.put(
            "registry.view.<xmlattr>.sid",
            "transfer_function_render"
        );

        m_container->configure(container_config);
        m_container->start().get();

        // Render
        m_render = sight::service::add(
            "sight::viz::scene2d::render",
            "transfer_function_render"
        );

        const std::string render_config =
            "<scene>"
            "    <scene x='0' y='-1' width='100' height='1'/>"
            "    <axis id='xAxis' origin='0' scale='1' scaleType='LINEAR'/>"
            "    <axis id='yAxis' origin='0' scale='-1' scaleType='LINEAR'/>"
            "    <adaptor uid='transfer_function'/>"
            "</scene>";

        m_render->set_config(render_config);
        m_render->configure();
        m_render->start().get();

        // Viewport
        m_viewport =
            std::make_shared<sight::viz::scene2d::data::viewport>();

        m_viewport->set_x(0.);
        m_viewport->set_y(-1.);
        m_viewport->set_width(100.);
        m_viewport->set_height(1.);

        // Transfer function
        m_tf = std::make_shared<sight::data::transfer_function>();

        m_piece =
            std::make_shared<sight::data::transfer_function_piece>();

        m_piece->insert(
            {
                0.,
                sight::data::transfer_function::color_t(
                    1.,
                    0.,
                    0.,
                    0.2
                )
            });

        m_piece->insert(
            {
                0.5,
                sight::data::transfer_function::color_t(
                    0.,
                    1.,
                    0.,
                    0.5
                )
            });

        m_piece->insert(
            {
                1.,
                sight::data::transfer_function::color_t(
                    0.,
                    0.,
                    1.,
                    0.8
                )
            });

        m_piece->set_window_min_max(
            sight::data::transfer_function::min_max_t(20., 80.)
        );

        m_tf->pieces().push_back(m_piece);
        m_tf->fit_window();

        m_service->set_input(m_viewport, "viewport");
        m_service->set_inout(m_tf, "tf");

        const std::string adaptor_config =
            "<config "
            "xAxis='xAxis' "
            "yAxis='yAxis' "
            "interactive='true' "
            "pointSize='0.03'/>";

        m_service->set_config(adaptor_config);
        m_service->configure();
        m_service->start().get();
    }

    ~transfer_function_fixture() override
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
    sight::data::transfer_function::sptr m_tf;
    sight::data::transfer_function_piece::sptr m_piece;
};

} // namespace
TEST_CASE_FIXTURE(
    transfer_function_fixture,
    "transfer_function_process_interaction"
)
{
    auto render =
        std::dynamic_pointer_cast<sight::viz::scene2d::render>(m_render);

    auto adaptor =
        std::dynamic_pointer_cast<sight::viz::scene2d::adaptor>(m_service);

    REQUIRE(render != nullptr);
    REQUIRE(adaptor != nullptr);

    const auto make_event =
        [&](auto _type,
            double _x,
            double _y,
            decltype(sight::viz::scene2d::data::event::no_button) _button =
                sight::viz::scene2d::data::event::no_button)
        {
            const QPoint point =
                render->get_view()->mapFromScene(QPointF(_x, _y));

            sight::viz::scene2d::data::event event;
            event.set_type(_type);
            event.set_button(_button);

            event.set_coord(
        {
            static_cast<double>(point.x()),
            static_cast<double>(point.y())
        });

            return event;
        };
    // Resize
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::resize,
            0.,
            0.
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());
    }

    // Double click -> add TF point
    {
        const auto size_before = m_piece->size();

        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_button_double_click,
            25.,
            -0.4,
            sight::viz::scene2d::data::event::left_button
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());
        CHECK(m_piece->size() == size_before + 1);
    }

    // Capture a point
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_button_press,
            50.,
            -0.5,
            sight::viz::scene2d::data::event::left_button
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());
    }

    // Move captured point
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            60.,
            -0.7
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());
    }

    // Release captured point
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_button_release,
            60.,
            -0.7,
            sight::viz::scene2d::data::event::left_button
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());
    }

    // Middle press
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_button_press,
            50.,
            -0.5,
            sight::viz::scene2d::data::event::mid_button
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());
    }

// Move whole TF
    {
        const double old_level = m_piece->level();

        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_move,
            60.,
            -0.5
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());
        CHECK(m_piece->level() != doctest::Approx(old_level));
    }

    // Middle release
    {
        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_button_release,
            80.,
            -0.1,
            sight::viz::scene2d::data::event::mid_button
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());

        CHECK(
            render->get_view()->cursor().shape()
            == Qt::ArrowCursor
        );
    }

    // Wheel over TF
    {
        const double old_alpha = m_piece->begin()->second.a;

        auto event = make_event(
            sight::viz::scene2d::data::event::mouse_wheel_up,
            75.,
            -0.1
        );

        adaptor->process_interaction(event);

        CHECK(event.is_accepted());
        CHECK(m_piece->begin()->second.a > old_alpha);
    }
}
