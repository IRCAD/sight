/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

#include "concatenate_matrices_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/matrix4.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::geometry::ut::concatenate_matrices_test);

namespace sight::module::geometry::ut
{

static const double DELTA = 1e-3;
using matrix_t = std::array<double, 16>;

//------------------------------------------------------------------------------

void concatenate_matrices_test::setUp()
{
}

//------------------------------------------------------------------------------

void concatenate_matrices_test::tearDown()
{
}

//------------------------------------------------------------------------------

void check_matrix(const matrix_t& _expected, const sight::data::matrix4& _actual)
{
    for(size_t i = 0 ; i < _expected.size() ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(_expected[i], _actual[i], DELTA);
    }
}

//------------------------------------------------------------------------------

void concatenate_matrices_test::identity()
{
    auto srv = sight::service::add("sight::module::geometry::concatenate_matrices");
    CPPUNIT_ASSERT(srv != nullptr);
    CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::concatenate_matrices"));

    // One identity matrix as input
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"matrix\">"
           "     <key uid=\"m1\" />"
           "</in>";
        sight::service::base::config_t config;
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);
    }

    auto input  = std::make_shared<sight::data::matrix4>();
    auto output = std::make_shared<sight::data::matrix4>();

    const matrix_t identity = sight::data::matrix4::identity();
    check_matrix(identity, *input);
    check_matrix(identity, *output);

    srv->set_input(input, "matrix", true, false, 0);
    srv->set_inout(output, "output");

    srv->configure();
    srv->start().get();
    srv->update().get();

    check_matrix(identity, *output);

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());

    // Add a second identity matrix as input
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"matrix\">"
           "     <key uid=\"m1\" />"
           "     <key uid=\"m2\" />"
           "</in>";
        sight::service::base::config_t config;
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);
    }
    auto input2 = std::make_shared<sight::data::matrix4>();
    srv->set_input(input2, "matrix", true, false, 1);

    srv->configure();
    srv->start().get();
    srv->update().get();

    check_matrix(identity, *output);

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());

    sight::service::remove(srv);
}

//------------------------------------------------------------------------------

void concatenate_matrices_test::single()
{
    // Silly test to verify the output = the input if there is only one matrix

    auto srv = sight::service::add("sight::module::geometry::concatenate_matrices");
    CPPUNIT_ASSERT(srv != nullptr);
    CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::concatenate_matrices"));

    {
        std::stringstream config_string;
        config_string
        << "<in group=\"matrix\">"
           "     <key uid=\"m1\" />"
           "</in>";
        sight::service::base::config_t config;
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);
    }

    auto input  = std::make_shared<sight::data::matrix4>();
    auto output = std::make_shared<sight::data::matrix4>();

    const matrix_t m = {0., 0., 0., 12.,
                        1., -0.866, -0.5, -51.34,
                        0., 0.5, -0.866, 1.314,
                        0., 0., 0., 1.
    };
    *input = m;
    check_matrix(m, *input);

    const matrix_t identity = sight::data::matrix4::identity();
    check_matrix(identity, *output);

    srv->set_input(input, "matrix", true, false, 0);
    srv->set_inout(output, "output");

    srv->configure();
    srv->start().get();
    srv->update().get();

    check_matrix(m, *output);

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());

    sight::service::remove(srv);
}

//------------------------------------------------------------------------------

void concatenate_matrices_test::multiply_2()
{
    auto srv = sight::service::add("sight::module::geometry::concatenate_matrices");
    CPPUNIT_ASSERT(srv != nullptr);
    CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::concatenate_matrices"));

    std::stringstream config_string;
    config_string
    << "<in group=\"matrix\">"
       "     <key uid=\"m1\" />"
       "     <key uid=\"m2\" />"
       "</in>";
    sight::service::base::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);

    auto in1    = std::make_shared<sight::data::matrix4>();
    auto in2    = std::make_shared<sight::data::matrix4>();
    auto output = std::make_shared<sight::data::matrix4>();

    *in1 = {
        1., 0., 0., 4.,
        0., 1., 0., -5.,
        0., 0., 1., 2.,
        0., 0., 0., 1.
    };

    *in2 = {1., 0., 0., 0.,
            0., -0.866, -0.5, 0.,
            0., 0.5, -0.866, 0.,
            0., 0., 0., 1.
    };

    srv->set_input(in1, "matrix", true, false, 0);
    srv->set_input(in2, "matrix", true, false, 1);
    srv->set_inout(output, "output");

    srv->configure();
    srv->start().get();

    const matrix_t identity = sight::data::matrix4::identity();
    check_matrix(identity, *output);

    srv->update().get();

    const matrix_t expected = {
        1., 0., 0., 4.,
        0., -0.866, -0.5, -5.,
        0., 0.5, -0.866, 2.,
        0., 0., 0., 1.
    };

    check_matrix(expected, *output);

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());

    sight::service::remove(srv);
}

//------------------------------------------------------------------------------

void concatenate_matrices_test::multiply_4()
{
    auto srv = sight::service::add("sight::module::geometry::concatenate_matrices");
    CPPUNIT_ASSERT(srv != nullptr);
    CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::concatenate_matrices"));

    std::stringstream config_string;
    config_string
    << "<in group=\"matrix\">"
       "     <key uid=\"m1\" />"
       "     <key uid=\"m2\" />"
       "     <key uid=\"m3\" />"
       "     <key uid=\"m4\" />"
       "</in>";
    sight::service::base::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);

    auto in1    = std::make_shared<sight::data::matrix4>();
    auto in2    = std::make_shared<sight::data::matrix4>();
    auto in3    = std::make_shared<sight::data::matrix4>();
    auto in4    = std::make_shared<sight::data::matrix4>();
    auto output = std::make_shared<sight::data::matrix4>();

    *in1 = {
        1., 0., 0., 4.,
        0., 1., 0., -5.,
        0., 0., 1., 2.,
        0., 0., 0., 1.
    };
    *in2 = {1., 0., 0., 0.,
            0., -0.866, -0.5, 0.,
            0., 0.5, -0.866, 0.,
            0., 0., 0., 1.
    };
    *in3 = {0.5, 0., 0.866, 0.,
            0., 1., 0., 0.,
            -0.866, 0., 0.5, 0.,
            0., 0., 0., 1.
    };
    *in4 = {-0.866, -0.5, 0., 14.94,
            0.5, -0.866, 0., 0.,
            0., 0., 1., -27.,
            0., 0., 0., 1.
    };

    srv->set_input(in1, "matrix", true, false, 0);
    srv->set_input(in2, "matrix", true, false, 1);
    srv->set_input(in3, "matrix", true, false, 2);
    srv->set_input(in4, "matrix", true, false, 3);
    srv->set_inout(output, "output");

    srv->configure();
    srv->start().get();

    const matrix_t identity = sight::data::matrix4::identity();
    check_matrix(identity, *output);

    srv->update().get();

    const matrix_t expected = {-0.433, -0.25, 0.866, -11.912,
                               -0.807978, 0.533456, -0.25, 8.21902,
                               -0.399462, -0.807978, -0.433, 24.8953,
                               0., 0., 0., 1.,
    };
    check_matrix(expected, *output);

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());

    sight::service::remove(srv);
}

//------------------------------------------------------------------------------

void concatenate_matrices_test::multiply_inverse()
{
    auto srv = sight::service::add("sight::module::geometry::concatenate_matrices");
    CPPUNIT_ASSERT(srv != nullptr);
    CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::concatenate_matrices"));

    std::stringstream config_string;
    config_string
    << "<in group=\"matrix\">"
       "     <key uid=\"m1\" inverse=\"true\"/>"
       "     <key uid=\"m2\" />"
       "     <key uid=\"m3\" inverse=\"true\"/>"
       "     <key uid=\"m4\" inverse=\"true\"/>"
       "</in>";
    sight::service::base::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);

    auto in1    = std::make_shared<sight::data::matrix4>();
    auto in2    = std::make_shared<sight::data::matrix4>();
    auto in3    = std::make_shared<sight::data::matrix4>();
    auto in4    = std::make_shared<sight::data::matrix4>();
    auto output = std::make_shared<sight::data::matrix4>();

    *in1 = {
        1., 0., 0., 4.,
        0., 1., 0., -5.,
        0., 0., 1., 2.,
        0., 0., 0., 1.
    };
    *in2 = {1., 0., 0., 0.,
            0., -0.866, -0.5, 0.,
            0., 0.5, -0.866, 0.,
            0., 0., 0., 1.
    };
    *in3 = {0.5, 0., 0.866, 0.,
            0., 1., 0., 0.,
            -0.866, 0., 0.5, 0.,
            0., 0., 0., 1.
    };
    *in4 = {-0.866, -0.5, 0., 14.94,
            0.5, -0.866, 0., 0.,
            0., 0., 1., -27.,
            0., 0., 0., 1.
    };

    srv->set_input(in1, "matrix", true, false, 0);
    srv->set_input(in2, "matrix", true, false, 1);
    srv->set_input(in3, "matrix", true, false, 2);
    srv->set_input(in4, "matrix", true, false, 3);
    srv->set_inout(output, "output");

    srv->configure();
    srv->start().get();

    const matrix_t identity = sight::data::matrix4::identity();
    check_matrix(identity, *output);

    // Stress a bit to ensure we always get the same result
    for(int i = 0 ; i < 500 ; ++i)
    {
        srv->update().get();

        const matrix_t expected = {-0.433038, 0.250022, -0.866038, -20.9134,
                                   0.80803, 0.53347, -0.250011, -13.8223,
                                   0.399508, -0.80803, -0.433019, -19.6602,
                                   0., 0., 0., 1.
        };
        check_matrix(expected, *output);
    }

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());

    sight::service::remove(srv);
}

//------------------------------------------------------------------------------

void concatenate_matrices_test::auto_connect()
{
    auto srv = sight::service::add("sight::module::geometry::concatenate_matrices");
    CPPUNIT_ASSERT(srv != nullptr);
    CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::concatenate_matrices"));

    std::stringstream config_string;
    config_string
    << "<in group=\"matrix\">"
       "     <key uid=\"m1\" auto_connect=\"false\"/>"
       "     <key uid=\"m2\" />"
       "</in>";
    sight::service::base::config_t config;
    boost::property_tree::read_xml(config_string, config);
    srv->set_config(config);

    auto in1    = std::make_shared<sight::data::matrix4>();
    auto in2    = std::make_shared<sight::data::matrix4>();
    auto output = std::make_shared<sight::data::matrix4>();

    const matrix_t identity = sight::data::matrix4::identity();
    check_matrix(identity, *in1);
    check_matrix(identity, *in2);

    srv->set_input(in1, "matrix", false, false, 0);
    srv->set_input(in2, "matrix", true, false, 1);
    srv->set_inout(output, "output");

    srv->configure();
    srv->start().get();

    srv->update().get();
    check_matrix(identity, *output);

    {
        // Matrix 1 is not auto connected
        *in1 = {
            1., 0., 0., 4.,
            0., 1., 0., -5.,
            0., 0., 1., 2.,
            0., 0., 0., 1.
        };
        auto sig = in1->signal<sight::data::matrix4::modified_signal_t>(sight::data::matrix4::MODIFIED_SIG);
        sig->emit();
        check_matrix(identity, *output);

        // Thus we require an update
        srv->update().get();
        check_matrix(in1->get_content(), *output);
    }

    {
        // Matrix 2 is auto connected
        *in2 = {1., 0., 0., 0.,
                0., -0.866, -0.5, 0.,
                0., 0.5, -0.866, 0.,
                0., 0., 0., 1.
        };
        auto sig = in2->signal<sight::data::matrix4::modified_signal_t>(sight::data::matrix4::MODIFIED_SIG);
        sig->emit();

        const matrix_t expected = {
            1., 0., 0., 4.,
            0., -0.866, -0.5, -5.,
            0., 0.5, -0.866, 2.,
            0., 0., 0., 1.
        };
        check_matrix(expected, *output);
    }

    CPPUNIT_ASSERT_NO_THROW(srv->stop().get());

    sight::service::remove(srv);
}

} // namespace sight::module::geometry::ut
