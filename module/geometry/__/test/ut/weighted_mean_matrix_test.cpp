/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <core/runtime/runtime.hpp>

#include <data/matrix4.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

#include <doctest/doctest.h>

static const double DELTA = 10e-2;

namespace
{

struct weighted_mean_matrix_tester
{
    weighted_mean_matrix_tester()
    {
        CHECK_NOTHROW(srv = sight::service::add("sight::module::geometry::weighted_mean_matrix"));
        CHECK(srv != nullptr);
        CHECK(srv->is_a("sight::module::geometry::weighted_mean_matrix"));
    }

    //------------------------------------------------------------------------------

    ~weighted_mean_matrix_tester()
    {
        CHECK_NOTHROW(srv->stop().wait());
        sight::service::remove(srv);
    }

    //------------------------------------------------------------------------------

    void set_config(float _weight = 0.5F)
    {
        std::stringstream config_string;
        config_string
        << "<in key=\"raw\" uid=\"raw\" />"
           "<inout key=\"damped\" uid=\"damped\" />"
           "<properties weight=\"" << _weight << R"(" enabled="true" />)";

        sight::service::base::config_t config;
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);

        srv->configure();
        srv->start().wait();
    }

    //------------------------------------------------------------------------------

    void init()
    {
        input  = std::make_shared<sight::data::matrix4>();
        output = std::make_shared<sight::data::matrix4>();

        srv->set_input(input, "raw", true);
        srv->set_inout(output, "damped");
    }

    //------------------------------------------------------------------------------

    void check_identity()
    {
        const auto coefs = output->get_content();

        CHECK(1. == doctest::Approx(coefs[0]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[1]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[2]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[3]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(coefs[4]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(coefs[5]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[6]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[7]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(coefs[8]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[9]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(coefs[10]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[11]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(coefs[12]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[13]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(coefs[14]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(coefs[15]).epsilon(DELTA));
    }

    sight::service::base::sptr srv;
    sight::data::matrix4::sptr input;
    sight::data::matrix4::sptr output;
};

} // namespace

TEST_SUITE("sight::module::geometry::weighted_mean_matrix")
{
    TEST_CASE_FIXTURE(weighted_mean_matrix_tester, "pass_through_test")
    {
        init();
        set_config();

        srv->update().wait();

        check_identity();
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(weighted_mean_matrix_tester, "translation_test")
    {
        init();
        set_config();

        // identity
        input->at(3)  = 0;
        input->at(7)  = 0;
        input->at(11) = 0;

        srv->update().wait();

        // translate
        input->at(3)  = 10;
        input->at(7)  = 20;
        input->at(11) = 30;

        srv->update().wait();

        const auto res = output->get_content();

        CHECK(1. == doctest::Approx(res[0]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[1]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[2]).epsilon(DELTA));
        CHECK(5. == doctest::Approx(res[3]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[4]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[5]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[6]).epsilon(DELTA));
        CHECK(10. == doctest::Approx(res[7]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[8]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[9]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[10]).epsilon(DELTA));
        CHECK(15. == doctest::Approx(res[11]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[12]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[13]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[14]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[15]).epsilon(DELTA));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(weighted_mean_matrix_tester, "full_test_weight50")
    {
        // First matrice identity
        // Second Matrice 100° on Ry

        init();
        set_config(0.5F);

        srv->update().wait();

        // Rotation part
        input->at(0) = -0.1736482;
        input->at(1) = 0.0000000;
        input->at(2) = 0.9848077;

        input->at(4) = 0.;
        input->at(5) = 1.;
        input->at(6) = 0.;

        input->at(8)  = -0.9848077;
        input->at(9)  = 0.;
        input->at(10) = -0.1736482;

        // Translation part
        input->at(3)  = 1;
        input->at(7)  = 2;
        input->at(11) = 30;

        srv->update().wait();

        // weight 0.5 should give a result around 50°
        const auto res = output->get_content();

        CHECK(0.6427876 == doctest::Approx(res[0]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[1]).epsilon(DELTA));
        CHECK(0.7660444 == doctest::Approx(res[2]).epsilon(DELTA));
        CHECK(0.5 == doctest::Approx(res[3]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[4]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[5]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[6]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[7]).epsilon(DELTA));

        CHECK(-0.7660444 == doctest::Approx(res[8]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[9]).epsilon(DELTA));
        CHECK(0.6427876 == doctest::Approx(res[10]).epsilon(DELTA));
        CHECK(15. == doctest::Approx(res[11]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[12]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[13]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[14]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[15]).epsilon(DELTA));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(weighted_mean_matrix_tester, "full_test_weight30")
    {
        // First matrice identity
        // Second Matrice 100° on Ry

        init();
        set_config(0.3F);

        srv->update().wait();

        // Rotation part
        input->at(0) = -0.1736482;
        input->at(1) = 0.0000000;
        input->at(2) = 0.9848077;

        input->at(4) = 0.;
        input->at(5) = 1.;
        input->at(6) = 0.;

        input->at(8)  = -0.9848077;
        input->at(9)  = 0.;
        input->at(10) = -0.1736482;

        // Translation part
        input->at(3)  = 10;
        input->at(7)  = 100;
        input->at(11) = 1000;

        srv->update().wait();

        // weight 0.7 should give a result around 30°

        const auto res = output->get_content();

        CHECK(0.8660254 == doctest::Approx(res[0]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[1]).epsilon(DELTA));
        CHECK(0.5000000 == doctest::Approx(res[2]).epsilon(DELTA));
        CHECK(3.0 == doctest::Approx(res[3]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[4]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[5]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[6]).epsilon(DELTA));
        CHECK(30.0 == doctest::Approx(res[7]).epsilon(DELTA));

        CHECK(-0.5000000 == doctest::Approx(res[8]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[9]).epsilon(DELTA));
        CHECK(0.8660254 == doctest::Approx(res[10]).epsilon(DELTA));
        CHECK(300.0 == doctest::Approx(res[11]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[12]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[13]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[14]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[15]).epsilon(DELTA));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(weighted_mean_matrix_tester, "full_test_weight70")
    {
        // First matrice identity
        // Second Matrice 100° on Ry

        init();
        set_config(0.7F);

        srv->update().wait();

        // Rotation part
        input->at(0) = -0.1736482;
        input->at(1) = 0.0000000;
        input->at(2) = 0.9848077;

        input->at(4) = 0.;
        input->at(5) = 1.;
        input->at(6) = 0.;

        input->at(8)  = -0.9848077;
        input->at(9)  = 0.;
        input->at(10) = -0.1736482;

        // Translation part
        input->at(3)  = 10;
        input->at(7)  = 100;
        input->at(11) = 1000;

        srv->update().wait();

        // weight 0.3 should give a result around 70°

        const auto res = output->get_content();

        CHECK(0.3420202 == doctest::Approx(res[0]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[1]).epsilon(DELTA));
        CHECK(0.9396926 == doctest::Approx(res[2]).epsilon(DELTA));
        CHECK(7. == doctest::Approx(res[3]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[4]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[5]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[6]).epsilon(DELTA));
        CHECK(70. == doctest::Approx(res[7]).epsilon(DELTA));

        CHECK(-0.9396926 == doctest::Approx(res[8]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[9]).epsilon(DELTA));
        CHECK(0.3420202 == doctest::Approx(res[10]).epsilon(DELTA));
        CHECK(700. == doctest::Approx(res[11]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[12]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[13]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[14]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[15]).epsilon(DELTA));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(weighted_mean_matrix_tester, "full_test_weight10")
    {
        // First matrice identity
        // Second Matrice 100° on Ry

        init();
        set_config(0.1F);

        srv->update().wait();

        // Rotation part
        input->at(0) = -0.1736482;
        input->at(1) = 0.0000000;
        input->at(2) = 0.9848077;

        input->at(4) = 0.;
        input->at(5) = 1.;
        input->at(6) = 0.;

        input->at(8)  = -0.9848077;
        input->at(9)  = 0.;
        input->at(10) = -0.1736482;

        // Translation part
        input->at(3)  = 10;
        input->at(7)  = 100;
        input->at(11) = 1000;

        srv->update().wait();

        const auto res = output->get_content();

        // weight 0.1 give a rotation about 10°
        CHECK(0.9848077 == doctest::Approx(res[0]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[1]).epsilon(DELTA));
        CHECK(0.1736482 == doctest::Approx(res[2]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[3]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[4]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[5]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[6]).epsilon(DELTA));
        CHECK(10. == doctest::Approx(res[7]).epsilon(DELTA));

        CHECK(-0.1736482 == doctest::Approx(res[8]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[9]).epsilon(DELTA));
        CHECK(0.9848077 == doctest::Approx(res[10]).epsilon(DELTA));
        CHECK(100. == doctest::Approx(res[11]).epsilon(DELTA));

        CHECK(0. == doctest::Approx(res[12]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[13]).epsilon(DELTA));
        CHECK(0. == doctest::Approx(res[14]).epsilon(DELTA));
        CHECK(1. == doctest::Approx(res[15]).epsilon(DELTA));
    }

//------------------------------------------------------------------------------

    TEST_CASE_FIXTURE(weighted_mean_matrix_tester, "full_test_weight100")
    {
        // First matrice identity
        // Second Matrice 100° on Ry

        init();
        set_config(1.F);

        srv->update().wait();

        // Rotation part
        input->at(0) = -0.1736482;
        input->at(1) = 0.0000000;
        input->at(2) = 0.9848077;

        input->at(4) = 0.;
        input->at(5) = 1.;
        input->at(6) = 0.;

        input->at(8)  = -0.9848077;
        input->at(9)  = 0.;
        input->at(10) = -0.1736482;

        // Translation part
        input->at(3)  = 1;
        input->at(7)  = 2;
        input->at(11) = 30;

        srv->update().wait();

        // weight 1.0 should give the latest matrix, no averaging.
        const auto res = output->get_content();
        const auto in  = input->get_content();

        for(std::size_t i = 0 ; i < in.size() ; ++i)
        {
            CHECK(in[i] == doctest::Approx(res[i]).epsilon(DELTA));
        }
    }
} // TEST_SUITE
