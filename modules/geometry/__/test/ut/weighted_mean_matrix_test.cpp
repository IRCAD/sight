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

#include "weighted_mean_matrix_test.hpp"

#include <core/runtime/runtime.hpp>

#include <data/landmarks.hpp>
#include <data/matrix4.hpp>

#include <service/op.hpp>

#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::geometry::ut::weighted_mean_matrix_test);

namespace sight::module::geometry::ut
{

static const double DELTA = 10e-2;

//------------------------------------------------------------------------------

void weighted_mean_matrix_test::setUp()
{
}

//------------------------------------------------------------------------------

void weighted_mean_matrix_test::tearDown()
{
}

//------------------------------------------------------------------------------

class weighted_mean_matrix_tester final
{
public:

    weighted_mean_matrix_tester()
    {
        CPPUNIT_ASSERT_NO_THROW(srv = sight::service::add("sight::module::geometry::weighted_mean_matrix"));
        CPPUNIT_ASSERT(srv != nullptr);
        CPPUNIT_ASSERT(srv->is_a("sight::module::geometry::weighted_mean_matrix"));
    }

    //------------------------------------------------------------------------------

    ~weighted_mean_matrix_tester()
    {
        CPPUNIT_ASSERT_NO_THROW(srv->stop().wait());
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

        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., coefs[0], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[1], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[2], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[3], DELTA);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[4], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., coefs[5], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[6], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[7], DELTA);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[8], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[9], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., coefs[10], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[11], DELTA);

        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[12], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[13], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(0., coefs[14], DELTA);
        CPPUNIT_ASSERT_DOUBLES_EQUAL(1., coefs[15], DELTA);
    }

    sight::service::base::sptr srv;
    sight::data::matrix4::sptr input;
    sight::data::matrix4::sptr output;
};

//------------------------------------------------------------------------------

void weighted_mean_matrix_test::pass_through_test()
{
    weighted_mean_matrix_tester service_tester;
    service_tester.init();
    service_tester.set_config();

    service_tester.srv->update().wait();

    service_tester.check_identity();
}

//------------------------------------------------------------------------------
void weighted_mean_matrix_test::translation_test()
{
    weighted_mean_matrix_tester service_tester;
    service_tester.init();
    service_tester.set_config();

    // identity
    service_tester.input->at(3)  = 0;
    service_tester.input->at(7)  = 0;
    service_tester.input->at(11) = 0;

    service_tester.srv->update().wait();

    // translate
    service_tester.input->at(3)  = 10;
    service_tester.input->at(7)  = 20;
    service_tester.input->at(11) = 30;

    service_tester.srv->update().wait();

    const auto res = service_tester.output->get_content();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[0], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[1], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[2], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(5., res[3], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[4], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[5], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[6], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10., res[7], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[8], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[9], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[10], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(15., res[11], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[12], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[13], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[14], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[15], DELTA);
}

//------------------------------------------------------------------------------

void weighted_mean_matrix_test::full_test_weight50()
{
    // First matrice identity
    // Second Matrice 100° on Ry

    weighted_mean_matrix_tester service_tester;
    service_tester.init();
    service_tester.set_config(0.5F);

    service_tester.srv->update().wait();

    // Rotation part
    service_tester.input->at(0) = -0.1736482;
    service_tester.input->at(1) = 0.0000000;
    service_tester.input->at(2) = 0.9848077;

    service_tester.input->at(4) = 0.;
    service_tester.input->at(5) = 1.;
    service_tester.input->at(6) = 0.;

    service_tester.input->at(8)  = -0.9848077;
    service_tester.input->at(9)  = 0.;
    service_tester.input->at(10) = -0.1736482;

    // Translation part
    service_tester.input->at(3)  = 1;
    service_tester.input->at(7)  = 2;
    service_tester.input->at(11) = 30;

    service_tester.srv->update().wait();

    // weight 0.5 should give a result around 50°
    const auto res = service_tester.output->get_content();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.6427876, res[0], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[1], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.7660444, res[2], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5, res[3], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[4], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[5], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[6], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[7], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.7660444, res[8], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[9], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.6427876, res[10], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(15., res[11], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[12], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[13], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[14], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[15], DELTA);
}

//------------------------------------------------------------------------------

void weighted_mean_matrix_test::full_test_weight30()
{
    // First matrice identity
    // Second Matrice 100° on Ry

    weighted_mean_matrix_tester service_tester;
    service_tester.init();
    service_tester.set_config(0.3F);

    service_tester.srv->update().wait();

    // Rotation part
    service_tester.input->at(0) = -0.1736482;
    service_tester.input->at(1) = 0.0000000;
    service_tester.input->at(2) = 0.9848077;

    service_tester.input->at(4) = 0.;
    service_tester.input->at(5) = 1.;
    service_tester.input->at(6) = 0.;

    service_tester.input->at(8)  = -0.9848077;
    service_tester.input->at(9)  = 0.;
    service_tester.input->at(10) = -0.1736482;

    // Translation part
    service_tester.input->at(3)  = 10;
    service_tester.input->at(7)  = 100;
    service_tester.input->at(11) = 1000;

    service_tester.srv->update().wait();

    // weight 0.7 should give a result around 30°

    const auto res = service_tester.output->get_content();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.8660254, res[0], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[1], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.5000000, res[2], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, res[3], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[4], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[5], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[6], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(30.0, res[7], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.5000000, res[8], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[9], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.8660254, res[10], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(300.0, res[11], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[12], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[13], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[14], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[15], DELTA);
}

//------------------------------------------------------------------------------

void weighted_mean_matrix_test::full_test_weight70()
{
    // First matrice identity
    // Second Matrice 100° on Ry

    weighted_mean_matrix_tester service_tester;
    service_tester.init();
    service_tester.set_config(0.7F);

    service_tester.srv->update().wait();

    // Rotation part
    service_tester.input->at(0) = -0.1736482;
    service_tester.input->at(1) = 0.0000000;
    service_tester.input->at(2) = 0.9848077;

    service_tester.input->at(4) = 0.;
    service_tester.input->at(5) = 1.;
    service_tester.input->at(6) = 0.;

    service_tester.input->at(8)  = -0.9848077;
    service_tester.input->at(9)  = 0.;
    service_tester.input->at(10) = -0.1736482;

    // Translation part
    service_tester.input->at(3)  = 10;
    service_tester.input->at(7)  = 100;
    service_tester.input->at(11) = 1000;

    service_tester.srv->update().wait();

    // weight 0.3 should give a result around 70°

    const auto res = service_tester.output->get_content();

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3420202, res[0], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[1], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.9396926, res[2], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(7., res[3], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[4], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[5], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[6], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(70., res[7], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.9396926, res[8], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[9], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.3420202, res[10], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(700., res[11], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[12], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[13], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[14], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[15], DELTA);
}

//------------------------------------------------------------------------------

void weighted_mean_matrix_test::full_test_weight10()
{
    // First matrice identity
    // Second Matrice 100° on Ry

    weighted_mean_matrix_tester service_tester;
    service_tester.init();
    service_tester.set_config(0.1F);

    service_tester.srv->update().wait();

    // Rotation part
    service_tester.input->at(0) = -0.1736482;
    service_tester.input->at(1) = 0.0000000;
    service_tester.input->at(2) = 0.9848077;

    service_tester.input->at(4) = 0.;
    service_tester.input->at(5) = 1.;
    service_tester.input->at(6) = 0.;

    service_tester.input->at(8)  = -0.9848077;
    service_tester.input->at(9)  = 0.;
    service_tester.input->at(10) = -0.1736482;

    // Translation part
    service_tester.input->at(3)  = 10;
    service_tester.input->at(7)  = 100;
    service_tester.input->at(11) = 1000;

    service_tester.srv->update().wait();

    const auto res = service_tester.output->get_content();

    // weight 0.1 give a rotation about 10°
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.9848077, res[0], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[1], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.1736482, res[2], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[3], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[4], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[5], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[6], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(10., res[7], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(-0.1736482, res[8], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[9], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0.9848077, res[10], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(100., res[11], DELTA);

    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[12], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[13], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(0., res[14], DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(1., res[15], DELTA);
}

//------------------------------------------------------------------------------

void weighted_mean_matrix_test::full_test_weight100()
{
    // First matrice identity
    // Second Matrice 100° on Ry

    weighted_mean_matrix_tester service_tester;
    service_tester.init();
    service_tester.set_config(1.F);

    service_tester.srv->update().wait();

    // Rotation part
    service_tester.input->at(0) = -0.1736482;
    service_tester.input->at(1) = 0.0000000;
    service_tester.input->at(2) = 0.9848077;

    service_tester.input->at(4) = 0.;
    service_tester.input->at(5) = 1.;
    service_tester.input->at(6) = 0.;

    service_tester.input->at(8)  = -0.9848077;
    service_tester.input->at(9)  = 0.;
    service_tester.input->at(10) = -0.1736482;

    // Translation part
    service_tester.input->at(3)  = 1;
    service_tester.input->at(7)  = 2;
    service_tester.input->at(11) = 30;

    service_tester.srv->update().wait();

    // weight 1.0 should give the latest matrix, no averaging.
    const auto res = service_tester.output->get_content();
    const auto in  = service_tester.input->get_content();

    for(std::size_t i = 0 ; i < in.size() ; ++i)
    {
        CPPUNIT_ASSERT_DOUBLES_EQUAL(in[i], res[i], DELTA);
    }
}

} // namespace sight::module::geometry::ut
