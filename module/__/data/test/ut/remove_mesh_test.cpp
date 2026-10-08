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

#include <data/model_series.hpp>
#include <data/reconstruction.hpp>
#include <data/string.hpp>

#include <utest/service_fixture.hpp>
#include <utest/wait.hpp>

#include <doctest/doctest.h>

namespace
{

struct service_fixture : public sight::utest::service_fixture
{
    service_fixture() :
        sight::utest::service_fixture("sight::module::data::remove_mesh")
    {
    }
};

} // namespace

//-----------------------------------------------------------------------------

static sight::data::reconstruction::sptr make_reconstruction(const std::string& _type, const std::string& _name)
{
    auto reconstruction = std::make_shared<sight::data::reconstruction>();
    reconstruction->set_structure_type(_type);
    reconstruction->set_organ_name(_name);
    return reconstruction;
}

//-----------------------------------------------------------------------------

static void set_type_filter(
    const sight::service::base::sptr& _service,
    std::vector<sight::data::string::sptr>& _owned_filters,
    std::size_t _index,
    const std::string& _pattern
)
{
    _owned_filters.push_back(std::make_shared<sight::data::string>(_pattern));
    _service->set_input(_owned_filters.back(), "organ.type", true, false, _index);
}

//-----------------------------------------------------------------------------

static void set_name_filter(
    const sight::service::base::sptr& _service,
    std::vector<sight::data::string::sptr>& _owned_filters,
    std::size_t _index,
    const std::string& _pattern
)
{
    _owned_filters.push_back(std::make_shared<sight::data::string>(_pattern));
    _service->set_input(_owned_filters.back(), "organ.name", true, false, _index);
}

//-----------------------------------------------------------------------------

TEST_SUITE("sight::module::data::remove_mesh")
{
    TEST_CASE_FIXTURE(service_fixture, "type_and_name_rules_remove_matches_and_emit_removed_reconstructions")
    {
        auto liver_surface  = make_reconstruction("Liver", "liver-surface");
        auto liver_volume   = make_reconstruction("Liver", "liver-volume");
        auto kidney_surface = make_reconstruction("Kidney", "kidney-surface");
        auto kidney_volume  = make_reconstruction("Kidney", "kidney-volume");
        auto skin_surface   = make_reconstruction("Skin", "skin-surface");

        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({liver_surface, liver_volume, kidney_surface, kidney_volume, skin_surface});
        m_service->set_inout(model, "data.model");

        // The first rule combines type and name; the second has no type criterion.
        std::vector<sight::data::string::sptr> filters;
        set_type_filter(m_service, filters, 0, "(Liver|Kidney)");
        set_name_filter(m_service, filters, 0, ".*surface.*");
        set_name_filter(m_service, filters, 1, "kidney-volume");

        std::vector<sight::data::model_series::reconstruction_vector_t> removed_batches;
        auto removed_slot = new_slot(
            [&removed_batches](sight::data::model_series::reconstruction_vector_t _removed)
        {
            removed_batches.push_back(std::move(_removed));
        });
        model->signal(sight::data::model_series::signals::RECONSTRUCTIONS_REMOVED)->connect(removed_slot);

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());

        const auto& remaining = model->get_reconstruction_db();
        REQUIRE(remaining.size() == 2);
        CHECK(remaining[0] == liver_volume);
        CHECK(remaining[1] == skin_surface);

        SIGHT_TEST_WAIT(removed_batches.size() == 1);
        REQUIRE(removed_batches.size() == 1);
        REQUIRE(removed_batches[0].size() == 3);
        CHECK(removed_batches[0][0] == liver_surface);
        CHECK(removed_batches[0][1] == kidney_surface);
        CHECK(removed_batches[0][2] == kidney_volume);

        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "type_only_filter")
    {
        auto liver  = make_reconstruction("Liver", "left");
        auto kidney = make_reconstruction("Kidney", "right");
        auto model  = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({liver, kidney});
        m_service->set_inout(model, "data.model");
        std::vector<sight::data::string::sptr> filters;
        set_type_filter(m_service, filters, 0, "Kid.*");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        REQUIRE(model->get_reconstruction_db().size() == 1);
        CHECK(model->get_reconstruction_db()[0] == liver);
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "repeated_organ_tags_bind_hierarchically")
    {
        auto liver_surface  = make_reconstruction("Liver", "liver-surface");
        auto liver_volume   = make_reconstruction("Liver", "liver-volume");
        auto kidney_surface = make_reconstruction("Kidney", "kidney-surface");
        auto skin_volume    = make_reconstruction("Skin", "skin-volume");
        auto model          = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({liver_surface, liver_volume, kidney_surface, skin_volume});
        m_service->set_inout(model, "data.model");
        m_service->set_config("<organ type='Liver'/><organ name='.*surface'/>");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        REQUIRE(model->get_reconstruction_db().size() == 1);
        CHECK(model->get_reconstruction_db()[0] == skin_volume);
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "name_only_filter")
    {
        auto exact_name   = make_reconstruction("Liver", "surface");
        auto partial_name = make_reconstruction("Liver", "left-surface");
        auto liver_volume = make_reconstruction("Liver", "left-volume");
        auto model        = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({exact_name, partial_name, liver_volume});
        m_service->set_inout(model, "data.model");
        std::vector<sight::data::string::sptr> filters;
        set_name_filter(m_service, filters, 0, "surface");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        REQUIRE(model->get_reconstruction_db().size() == 2);
        CHECK(model->get_reconstruction_db()[0] == partial_name);
        CHECK(model->get_reconstruction_db()[1] == liver_volume);
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "no_match_preserves_reconstructions")
    {
        auto liver = make_reconstruction("Liver", "surface");
        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({liver});
        m_service->set_inout(model, "data.model");
        std::vector<sight::data::string::sptr> filters;
        set_type_filter(m_service, filters, 0, "Kidney");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        REQUIRE(model->get_reconstruction_db().size() == 1);
        CHECK(model->get_reconstruction_db()[0] == liver);
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "no_rules_preserve_reconstructions")
    {
        auto liver = make_reconstruction("Liver", "surface");
        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({liver});
        m_service->set_inout(model, "data.model");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_NOTHROW(m_service->update().get());
        REQUIRE(model->get_reconstruction_db().size() == 1);
        CHECK(model->get_reconstruction_db()[0] == liver);
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "missing_model_series")
    {
        m_service->set_inout(sight::data::model_series::sptr {}, "data.model");
        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_THROWS(m_service->update().get());
        CHECK_NOTHROW(m_service->stop().get());
    }

//-----------------------------------------------------------------------------

    TEST_CASE_FIXTURE(service_fixture, "invalid_regular_expression")
    {
        auto model = std::make_shared<sight::data::model_series>();
        model->set_reconstruction_db({make_reconstruction("Liver", "surface")});
        m_service->set_inout(model, "data.model");
        std::vector<sight::data::string::sptr> filters;
        set_type_filter(m_service, filters, 0, "[");

        CHECK_NOTHROW(m_service->configure());
        CHECK_NOTHROW(m_service->start().get());
        CHECK_THROWS(m_service->update().get());
        CHECK_EQ(model->get_reconstruction_db().size(), std::size_t(1));
        CHECK_NOTHROW(m_service->stop().get());
    }
} // namespace sight::module::data::remove_mesh
