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

#include <data/boolean.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/string.hpp>

#include <service/op.hpp>

#include <utest/service_fixture.hpp>
#include <utest/wait.hpp>

#include <doctest/doctest.h>

namespace
{

struct service_fixture : public sight::utest::service_fixture
{
    service_fixture() :
        sight::utest::service_fixture("sight::module::data::show_mesh")
    {
    }
};

//-----------------------------------------------------------------------------

sight::data::reconstruction::sptr make_reconstruction(const std::string& _type, const std::string& _name)
{
    auto reconstruction = std::make_shared<sight::data::reconstruction>();
    reconstruction->set_structure_type(_type);
    reconstruction->set_organ_name(_name);
    return reconstruction;
}

//-----------------------------------------------------------------------------

void set_filter(
    const sight::service::base::sptr& _service,
    std::vector<sight::data::string::sptr>& _owned_filters,
    const std::string& _key,
    std::size_t _index,
    const std::string& _pattern
)
{
    _owned_filters.push_back(std::make_shared<sight::data::string>(_pattern));
    _service->set_input(_owned_filters.back(), _key, true, false, _index);
}

//-----------------------------------------------------------------------------

} // namespace

TEST_SUITE("sight::module::data::show_mesh")
{
    TEST_CASE_FIXTURE(service_fixture, "type_and_name_rules_apply_visibility_to_matches_only")
    {
        auto liver_surface  = make_reconstruction("Liver", "liver-surface");
        auto liver_volume   = make_reconstruction("Liver", "liver-volume");
        auto kidney_surface = make_reconstruction("Kidney", "kidney-surface");
        auto kidney_volume  = make_reconstruction("Kidney", "kidney-volume");
        auto skin_surface   = make_reconstruction("Skin", "skin-surface");
        liver_surface->set_is_visible(false);
        kidney_volume->set_is_visible(false);
        skin_surface->set_is_visible(false);

        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({liver_surface, liver_volume, kidney_surface, kidney_volume, skin_surface});
        auto visible = std::make_shared<sight::data::boolean>(true);
        m_service->set_inout(model, "data.model");
        m_service->set_input(visible, "data.visible");

        std::vector<sight::data::string::sptr> filters;
        set_filter(m_service, filters, "organ.type", 0, "(Liver|Kidney)");
        set_filter(m_service, filters, "organ.name", 0, ".*surface.*");
        set_filter(m_service, filters, "organ.name", 1, "kidney-volume");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());

        CHECK(liver_surface->get_is_visible());
        CHECK_FALSE(liver_volume->get_is_visible());
        CHECK(kidney_surface->get_is_visible());
        CHECK(kidney_volume->get_is_visible());
        CHECK_FALSE(skin_surface->get_is_visible());
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "type_only_filter_hides_matches")
    {
        auto liver  = make_reconstruction("Liver", "surface");
        auto kidney = make_reconstruction("Kidney", "surface");
        liver->set_is_visible(true);
        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({liver, kidney});
        auto visible = std::make_shared<sight::data::boolean>(false);
        m_service->set_inout(model, "data.model");
        m_service->set_input(visible, "data.visible");
        std::vector<sight::data::string::sptr> filters;
        set_filter(m_service, filters, "organ.type", 0, "Kid.*");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        CHECK(liver->get_is_visible());
        CHECK_FALSE(kidney->get_is_visible());
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "name_only_filter_shows_matches")
    {
        auto surface = make_reconstruction("Liver", "surface");
        auto volume  = make_reconstruction("Liver", "volume");
        surface->set_is_visible(false);
        volume->set_is_visible(true);
        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({surface, volume});
        auto visible = std::make_shared<sight::data::boolean>(true);
        m_service->set_inout(model, "data.model");
        m_service->set_input(visible, "data.visible");
        std::vector<sight::data::string::sptr> filters;
        set_filter(m_service, filters, "organ.name", 0, "surface");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        CHECK(surface->get_is_visible());
        CHECK(volume->get_is_visible());
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "no_rules_leave_visibility_unchanged")
    {
        auto reconstruction = make_reconstruction("Liver", "surface");
        reconstruction->set_is_visible(false);
        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({reconstruction});
        auto visible = std::make_shared<sight::data::boolean>(true);
        m_service->set_inout(model, "data.model");
        m_service->set_input(visible, "data.visible");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        CHECK_FALSE(reconstruction->get_is_visible());
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "no_matching_rule_leaves_visibility_unchanged")
    {
        auto reconstruction = make_reconstruction("Liver", "surface");
        reconstruction->set_is_visible(false);
        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({reconstruction});
        auto visible = std::make_shared<sight::data::boolean>(true);
        m_service->set_inout(model, "data.model");
        m_service->set_input(visible, "data.visible");
        std::vector<sight::data::string::sptr> filters;
        set_filter(m_service, filters, "organ.name", 0, "volume");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        CHECK_FALSE(reconstruction->get_is_visible());
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "visible_modification_triggers_update")
    {
        auto reconstruction = make_reconstruction("Liver", "surface");
        auto model          = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({reconstruction});
        auto visible = std::make_shared<sight::data::boolean>(true);
        m_service->set_inout(model, "data.model");
        m_service->set_input(visible, "data.visible");
        std::vector<sight::data::string::sptr> filters;
        set_filter(m_service, filters, "organ.name", 0, "surface");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        CHECK(reconstruction->get_is_visible());

        visible->value() = false;
        visible->async_emit(sight::data::signals::MODIFIED);
        SIGHT_TEST_WAIT(!reconstruction->get_is_visible());
        CHECK_FALSE(reconstruction->get_is_visible());
        CHECK_NOTHROW(m_service->stop().get());
    }
} // namespace sight::module::data::show_mesh
