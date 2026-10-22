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

#include <utest/service_fixture.hpp>
#include <utest/wait.hpp>

#include <data/real.hpp>
#include <service/op.hpp>
#include <viz/scene2d/render.hpp>

#include <QCoreApplication>
#include <QGraphicsRectItem>
#include <doctest/doctest.h>

namespace
{

struct square_fixture : sight::utest::service_fixture
{
    explicit square_fixture(bool _bind_coordinates = true) :
        sight::utest::service_fixture("sight::module::viz::scene2d::adaptor::square")
    {
        m_service->set_id("settings_square");
        m_frame = sight::service::add("sight::module::ui::frame");
        m_frame->set_config(
            "<gui><frame><name>settings</name></frame></gui>"
            "<registry><view sid='settings_square_render'/></registry>"
        );
        m_frame->configure();
        m_frame->start().get();
        m_render = sight::service::add("sight::viz::scene2d::render", "settings_square_render");
        m_render->set_config(
            "<scene><scene x='0' y='0' width='200' height='100'/>"
            "<axis id='xAxis' origin='0' scale='1' scaleType='LINEAR'/>"
            "<axis id='yAxis' origin='0' scale='1' scaleType='LINEAR'/>"
            "<adaptor uid='settings_square'/></scene>"
        );
        m_render->configure();
        m_render->start().get();
        m_x = std::make_shared<sight::data::real>(12.);
        m_y = std::make_shared<sight::data::real>(34.);
        if(_bind_coordinates)
        {
            m_service->set_input(m_x, "config.x", true);
            m_service->set_input(m_y, "config.y", true);
        }

        m_service->set_config(
            _bind_coordinates
            ? "<config size='15' color='blue' xAxis='xAxis' yAxis='yAxis'/>"
            : "<config x='12' y='34' size='15' color='blue' xAxis='xAxis' yAxis='yAxis'/>"
        );
        m_service->configure();
        m_service->start().get();
    }

    ~square_fixture() override
    {
        if(m_service->started())
        {
            m_service->stop().get();
        }

        for(const auto& service : {m_render, m_frame})
        {
            if(service->started())
            {
                service->stop().get();
            }

            sight::service::remove(service);
        }
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QPointF position() const
    {
        QCoreApplication::processEvents();
        auto render = std::dynamic_pointer_cast<sight::viz::scene2d::render>(m_render);
        for(const auto* item : render->get_scene()->items())
        {
            if(const auto* rectangle = dynamic_cast<const QGraphicsRectItem*>(item))
            {
                return rectangle->mapToScene(rectangle->rect().topLeft());
            }
        }

        return {};
    }

    sight::service::base::sptr m_frame;
    sight::service::base::sptr m_render;
    sight::data::real::sptr m_x;
    sight::data::real::sptr m_y;
};

struct literal_square_fixture : square_fixture
{
    literal_square_fixture() :
        square_fixture(false)
    {
    }
};

} // namespace

TEST_SUITE("sight::module::viz::scene2d::adaptor::square")
{
    TEST_CASE_FIXTURE(literal_square_fixture, "literal_coordinates_apply_at_start_and_after_restart")
    {
        CHECK(position() == QPointF(12., 34.));
        m_service->stop().get();
        m_service->start().get();
        CHECK(position() == QPointF(12., 34.));
    }

    TEST_CASE_FIXTURE(square_fixture, "data_coordinates_apply_at_start_and_after_restart")
    {
        CHECK(position() == QPointF(12., 34.));
        m_x->set_value(56.);
        m_x->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_FAIL_WAIT(position() == QPointF(56., 34.));
        m_service->stop().get();
        m_y->set_value(78.);
        m_y->async_emit(sight::data::signals::MODIFIED);
        m_service->start().get();
        CHECK(position() == QPointF(56., 78.));
    }
}
