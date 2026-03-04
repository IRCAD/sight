/************************************************************************
 *
 * Copyright (C) 2022-2026 IRCAD France
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

#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/slots.hpp>
#include <core/com/slots.hxx>
#include <core/runtime/runtime.hpp>

#include <data/frame_tl.hpp>
#include <data/image.hpp>
#include <data/image_series.hpp>
#include <data/matrix4.hpp>
#include <data/matrix_tl.hpp>
#include <data/mt/weak_ptr.hpp>

#include <service/op.hpp>

// Need to be included before wait.hpp until we remove cppunit
#include <doctest/doctest.h>
#include <utest/wait.hpp>

#include <boost/property_tree/xml_parser.hpp>

namespace
{

class fixture
{
public:

    explicit fixture()
    {
        sight::core::runtime::init();
        sight::core::runtime::load_module(std::string("sight::module::sync"));
        srv = sight::service::add("sight::module::sync::synchronizer");
        CHECK(srv != nullptr);
    }

    //------------------------------------------------------------------------------

    ~fixture()
    {
        if(srv->started())
        {
            srv->stop().wait();
        }

        sight::service::remove(srv);
    }

    //------------------------------------------------------------------------------

    void set_config(std::stringstream& _config_string)
    {
        sight::service::config_t config;
        boost::property_tree::read_xml(_config_string, config);
        srv->set_config(config);

        srv->configure();
    }

    //------------------------------------------------------------------------------
    void init_standard_in_out()
    {
        // create and set the input TL
        m_frame_tl_1 = std::make_shared<sight::data::frame_tl>();
        m_frame_tl_1->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );
        m_frame_tl_2 = std::make_shared<sight::data::frame_tl>();
        m_frame_tl_2->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );
        m_matrix_tl_1 = std::make_shared<sight::data::matrix_tl>();
        m_matrix_tl_1->init_pool_size(4);

        srv->set_input(m_frame_tl_1, "frame_tl", true, false, 0);
        srv->set_input(m_frame_tl_2, "frame_tl", true, false, 1);
        srv->set_input(m_matrix_tl_1, "matrix_tl", true, false, 0);

        // create and set the inout which will be filled in the synchronization process
        m_frame1 = std::make_shared<sight::data::image>();
        m_frame1->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame1 = m_frame1->dump_lock();
            std::fill(m_frame1->begin<std::uint8_t>(), m_frame1->end<std::uint8_t>(), std::uint8_t(0));
        }

        m_frame2 = std::make_shared<sight::data::image>();
        m_frame2->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame2 = m_frame2->dump_lock();
            std::fill(m_frame2->begin<std::uint8_t>(), m_frame2->end<std::uint8_t>(), std::uint8_t(0));
        }

        m_matrix1          = std::make_shared<sight::data::matrix4>();
        (*m_matrix1)(0, 0) = 0.; // init the first value a 0. This will be filled with the timestamp in the tests.

        srv->set_inout(m_frame1, "frames", false, false, 0);
        srv->set_inout(m_frame2, "frames", false, false, 1);
        srv->set_inout(m_matrix1, "matrix", false, false, 0);
    }

    //------------------------------------------------------------------------------

    void add_frame_to_frame_tl(sight::data::frame_tl::sptr& _frame_tl, const std::uint8_t _timestamp)
    {
        const SPTR(sight::data::frame_tl::buffer_t) data = _frame_tl->create_buffer(_timestamp);
        std::uint8_t* elt_buffer                         = data->add_element(0);
        memset(elt_buffer, _timestamp, m_frame_size[0] * m_frame_size[1]);

        _frame_tl->push_object(data);
    }

    //------------------------------------------------------------------------------

    static void add_matrix_to_matrix_tl(
        sight::data::matrix_tl::sptr& _matrix_tl,
        unsigned int _element_index,
        const std::uint8_t _timestamp
)
    {
        const SPTR(sight::data::matrix_tl::buffer_t) data = _matrix_tl->create_buffer(_timestamp);
        std::array<float, 16> matrix {1., 0., 0., 0.,
                                      0., 1., 0., 0.,
                                      0., 0., 1., 0.,
                                      0., 0., 0., 1.
        };
        matrix[0] = float(_timestamp);
        data->set_element(matrix, _element_index);
        _matrix_tl->push_object(data);
    }

    //------------------------------------------------------------------------------

    static void add_matrix_to_matrix_tl(sight::data::matrix_tl::sptr& _matrix_tl, const std::uint8_t _timestamp)
    {
        add_matrix_to_matrix_tl(_matrix_tl, 0, _timestamp);
    }

    //------------------------------------------------------------------------------

    void add_matrix_to_matrix_tl(
        sight::data::matrix_tl::sptr& _matrix_tl,
        std::vector<unsigned int> _element_index_list,
        const std::uint8_t _timestamp
)
    {
        // The purpose here is to fill a matrix TL with a new matrix, which needs to store
        // some information, to be able to find the creation timestamp and element Index
        // once it is synchronized, in the out variable. TO do this, the first value is filled with
        // timestamp * 10 + elementIndex value.
        // by testing the first value of the matrix, it  is possible to verify that the
        // appropriate matrix has be put to a given output.

        const SPTR(sight::data::matrix_tl::buffer_t) data = _matrix_tl->create_buffer(_timestamp);

        for(const auto element_index : _element_index_list)
        {
            std::array<float, 16> matrix {1., 0., 0., 0.,
                                          0., 1., 0., 0.,
                                          0., 0., 1., 0.,
                                          0., 0., 0., 1.
            };
            matrix[0] = float(_timestamp * 10 + element_index);
            data->set_element(matrix, element_index);
        }

        _matrix_tl->push_object(data);
        srv->update().wait();
    }

    //------------------------------------------------------------------------------

    void static check_matrix(const sight::data::matrix4::sptr& _matrix, const double _value)
    {
        // The purpose here is to verify that the appropriate matrix has been put in an output variable.
        // To do this, the addMatrixToMatrixTL method fills the first value of the matrix with
        // timestamp * 10 + elementIndex value.
        // by testing the first value of the matrix, it  is possible to verify that the
        // appropriate matrix has be put to a given output.

        CHECK(_matrix);

        //test all values which should be 0, except the diagonal one which are 1. and the first one which is the
        // timestamp.
        for(std::size_t i = 1 ; i < 16 ; ++i)
        {
            const double matrix_value   = (*_matrix)[i];
            const double expected_value = i % 5 == 0 ? 1. : 0.;
            CHECK_EQ(expected_value, matrix_value);
        }

        const double matrix_value = (*_matrix)(0, 0);
        CHECK_EQ(_value, matrix_value);
    }

    //------------------------------------------------------------------------------

    void static check_frame(
        const sight::data::image::sptr& _image,
        const std::uint8_t _value
)
    {
        CHECK(_image);
        const auto dump_lock                                         = _image->dump_lock();
        const sight::data::image::const_iterator<std::uint8_t> begin = _image->cbegin<std::uint8_t>();
        const sight::data::image::const_iterator<std::uint8_t> end   = _image->cend<std::uint8_t>();
        // Avoid mistakes in case the range is null
        CHECK(begin != end);

        std::for_each(
            begin,
            end,
            [&_value](const auto& _x)
            {
                CHECK_EQ(_value, _x);
            });
    }

    sight::service::base::sptr srv;

    sight::data::frame_tl::sptr m_frame_tl_1;
    sight::data::frame_tl::sptr m_frame_tl_2;
    sight::data::matrix_tl::sptr m_matrix_tl_1;
    sight::data::image::sptr m_frame1;
    sight::data::image::sptr m_frame2;
    sight::data::matrix4::sptr m_matrix1;

    const sight::data::image::size_t m_frame_size {2, 2, 1};
    const int m_synch_wait_duration_ms {60};
};

class simple_fixture : public fixture
{
public:

    simple_fixture()
    {
        init_standard_in_out();

        std::stringstream config_string;
        config_string
        << "<in group=\"frame_tl\">"
           "    <key uid=\"frameTL1\" />"
           "    <key uid=\"frameTL2\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame1\" tl=\"0\"/>"
           "    <key uid=\"frame2\" tl=\"1\" />"
           "</inout>"
           "<in group=\"matrix_tl\">"
           "    <key uid=\"matrixTL1\" />"
           "</in>"
           "<inout group=\"matrix\">"
           "    <key uid=\"matrix1\" tl=\"0\"/>"
           "</inout>"
           "<tolerance>5</tolerance>";
        sight::service::config_t config;
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);

        srv->configure();
        srv->start().wait();
        srv->update().wait();
    }
};

} // namespace

//------------------------------------------------------------------------------

TEST_SUITE("sight::module::sync::synchronizer")
{
    TEST_CASE_FIXTURE(simple_fixture, "single_frame_tl_population")
    {
        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection = srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // populate the TLs
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        add_frame_to_frame_tl(m_frame_tl_1, 2);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        //test the output
        check_frame(m_frame1, 2);
        check_frame(m_frame2, 0);
        check_matrix(m_matrix1, 0);

        // populate the TL
        add_frame_to_frame_tl(m_frame_tl_1, 3);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 3);
        //test the output
        check_frame(m_frame1, 3);
        check_frame(m_frame2, 0);
        check_matrix(m_matrix1, 0);
    }

    TEST_CASE_FIXTURE(simple_fixture, "single_matrix_tl_population")
    {
        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection = srv->signal("synchronization_done")->connect(slot_synchronization_done);

        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 2);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        check_frame(m_frame1, 0);
        check_frame(m_frame2, 0);
        check_matrix(m_matrix1, 2);

        add_matrix_to_matrix_tl(m_matrix_tl_1, 3);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 3);
        check_frame(m_frame1, 0);
        check_frame(m_frame2, 0);
        check_matrix(m_matrix1, 3);
    }

    TEST_CASE_FIXTURE(simple_fixture, "basic_synchronisation")
    {
        //signal - slot connections for tests
        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // This is done just to handle automatic synch at first data push
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        srv->slot("reset")->run();

        //time 1: synch
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        check_frame(m_frame1, 1);
        check_frame(m_frame2, 1);
        check_matrix(m_matrix1, 1);

        //time 2: synch
        add_frame_to_frame_tl(m_frame_tl_1, 2);
        add_frame_to_frame_tl(m_frame_tl_2, 2);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 2);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);
        check_frame(m_frame1, 2);
        check_frame(m_frame2, 2);
        check_matrix(m_matrix1, 2);

        //time 3: synch
        add_frame_to_frame_tl(m_frame_tl_1, 6);
        add_frame_to_frame_tl(m_frame_tl_2, 6);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 6);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 6);
        check_frame(m_frame1, 6);
        check_frame(m_frame2, 6);
        check_matrix(m_matrix1, 6);

        //time 4: no synch, as new timestamp is min (8, 8, 6) = 6 = last_timestamp
        add_frame_to_frame_tl(m_frame_tl_1, 8);
        add_frame_to_frame_tl(m_frame_tl_2, 8);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        check_frame(m_frame1, 6);
        check_frame(m_frame2, 6);
        check_matrix(m_matrix1, 6);

        //time 5: synch, with timestamp  min(9,8,9)=8 => take frame/matrix closest to 8
        add_frame_to_frame_tl(m_frame_tl_1, 9);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 9);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 8);
        check_frame(m_frame1, 8);
        check_frame(m_frame2, 8);
        check_matrix(m_matrix1, 9);

        //time 6: no synch, as new timestamp is min (10, 8, 9) = 8 = last_timestamp
        add_frame_to_frame_tl(m_frame_tl_1, 10);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        check_frame(m_frame1, 8);
        check_frame(m_frame2, 8);
        check_matrix(m_matrix1, 9);

        //time 7: no synch, as new timestamp is min (11, 8, 11) = 8 = last_timestamp
        add_frame_to_frame_tl(m_frame_tl_1, 11);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 11);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        check_frame(m_frame2, 8);
        check_matrix(m_matrix1, 9);

        //time 8: synch, with timestamp  min(12,12,11)=11 => take frame/matrix closest to 11
        add_frame_to_frame_tl(m_frame_tl_1, 12);
        add_frame_to_frame_tl(m_frame_tl_2, 12);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 11);
        check_frame(m_frame1, 11);
        check_frame(m_frame2, 12);
        check_matrix(m_matrix1, 11);

        //time 9: no synch, as new timestamp is min (13, 13, 11) = 11 = last_timestamp
        add_frame_to_frame_tl(m_frame_tl_1, 13);
        add_frame_to_frame_tl(m_frame_tl_2, 13);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 11);
        check_frame(m_frame1, 11);
        check_frame(m_frame2, 12);
        check_matrix(m_matrix1, 11);

        //time 10: synch, with timestamp  min(14,13,14)=13 => take frame/matrix closest to 13
        add_frame_to_frame_tl(m_frame_tl_1, 14);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 14);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 13);
        check_frame(m_frame1, 13);
        check_frame(m_frame2, 13);
        check_matrix(m_matrix1, 14);

        //time 11: synch
        add_frame_to_frame_tl(m_frame_tl_1, 15);
        add_frame_to_frame_tl(m_frame_tl_2, 15);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 15);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 15);
        check_frame(m_frame1, 15);
        check_frame(m_frame2, 15);
        check_matrix(m_matrix1, 15);
    }

    TEST_CASE_FIXTURE(simple_fixture, "time_gap_synchronisation")
    {
        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // This is done just to handle automatic synch at first data push
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        srv->slot("reset")->run();

        //time 1: synch
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        check_frame(m_frame1, 1);
        check_frame(m_frame2, 1);
        check_matrix(m_matrix1, 1);

        //time 2: synch
        add_frame_to_frame_tl(m_frame_tl_1, 2);
        add_frame_to_frame_tl(m_frame_tl_2, 2);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 2);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);
        check_frame(m_frame1, 2);
        check_frame(m_frame2, 2);
        check_matrix(m_matrix1, 2);

        //time 3: ignore matrix too far away  timestamp  min(50, 49)=49
        add_frame_to_frame_tl(m_frame_tl_1, 50);
        add_frame_to_frame_tl(m_frame_tl_2, 49);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 3);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 49);
        check_frame(m_frame1, 50);
        check_frame(m_frame2, 49);
        check_matrix(m_matrix1, 2);

        //time 4: synch, with timestamp  min(51,50,49)=49 => last_timestamp
        add_frame_to_frame_tl(m_frame_tl_1, 51);
        add_frame_to_frame_tl(m_frame_tl_2, 50);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 49);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 49);
        check_frame(m_frame1, 50);
        check_frame(m_frame2, 49);
        check_matrix(m_matrix1, 2);

        //time 5: synch, with timestamp  min(52,50,50)=50 => take frame/matrix closest to 50
        add_frame_to_frame_tl(m_frame_tl_1, 52);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 50);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 50);
        check_frame(m_frame1, 50);
        check_frame(m_frame2, 50);
        check_matrix(m_matrix1, 50);

        //time 6: no synch, as new timestamp is min (53, 51, 50) = 50 = last_timestamp
        add_frame_to_frame_tl(m_frame_tl_1, 53);
        add_frame_to_frame_tl(m_frame_tl_2, 51);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        check_frame(m_frame1, 50);
        check_frame(m_frame2, 50);
        check_matrix(m_matrix1, 50);

        //time 7: synch, with timestamp  min(54,52,52)=52 => take frame/matrix closest to 52
        add_frame_to_frame_tl(m_frame_tl_1, 54);
        add_frame_to_frame_tl(m_frame_tl_2, 52);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 52);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 52);
        check_frame(m_frame1, 52);
        check_frame(m_frame2, 52);
        check_matrix(m_matrix1, 52);

        //time 8: synch
        add_frame_to_frame_tl(m_frame_tl_1, 55);
        add_frame_to_frame_tl(m_frame_tl_2, 55);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 55);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 55);
        check_frame(m_frame1, 55);
        check_frame(m_frame2, 55);
        check_matrix(m_matrix1, 55);
    }

    TEST_CASE_FIXTURE(simple_fixture, "reset_timeline_synchronisation")
    {
        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // This is done just to handle automatic synch at first data push
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        srv->slot("reset")->run();

        //time 1: synch
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        check_frame(m_frame1, 1);
        check_frame(m_frame2, 1);
        check_matrix(m_matrix1, 1);

        //time 2: synch
        add_frame_to_frame_tl(m_frame_tl_1, 2);
        add_frame_to_frame_tl(m_frame_tl_2, 2);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 2);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);
        check_frame(m_frame1, 2);
        check_frame(m_frame2, 2);
        check_matrix(m_matrix1, 2);

        //time 3: synch
        add_frame_to_frame_tl(m_frame_tl_1, 6);
        add_frame_to_frame_tl(m_frame_tl_2, 6);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 6);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 6);
        last_timestamp_synch = 0;
        check_frame(m_frame1, 6);
        check_frame(m_frame2, 6);
        check_matrix(m_matrix1, 6);

        srv->slot("reset")->run();

        //time 4: synch, with timestamp  min(6,8,8)=6
        // it is not the same as the previous one because of the reset
        add_frame_to_frame_tl(m_frame_tl_1, 8);
        add_frame_to_frame_tl(m_frame_tl_2, 8);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 6);
        check_frame(m_frame1, 6);
        check_frame(m_frame2, 6);
        check_matrix(m_matrix1, 6);

        //time 5: synch, with timestamp  min(9,8,9)=8 => take frame/matrix closest to 8
        add_frame_to_frame_tl(m_frame_tl_1, 9);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 9);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 8);
        check_frame(m_frame1, 8);
        check_frame(m_frame2, 8);
        check_matrix(m_matrix1, 9);

        //time 6: no synch, timestamp  min(10,8,9)= 8 = last_timestamp
        add_frame_to_frame_tl(m_frame_tl_1, 10);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        check_frame(m_frame1, 8);
        check_frame(m_frame2, 8);
        check_matrix(m_matrix1, 9);

        //time 7: sync, with, timestamp  min(11,11,9)= 9 => take frame/matrix closest to 9
        add_frame_to_frame_tl(m_frame_tl_1, 11);
        add_frame_to_frame_tl(m_frame_tl_2, 11);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 9);
        check_frame(m_frame1, 9);
        check_frame(m_frame2, 8);
        check_matrix(m_matrix1, 9);
    }

    TEST_CASE_FIXTURE(simple_fixture, "reset_and_loop_synchronisation")
    {
        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // This is done just to handle automatic synch at first data push
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        srv->slot("reset")->run();

        //time 1: synch
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        check_frame(m_frame1, 1);
        check_frame(m_frame2, 1);
        check_matrix(m_matrix1, 1);

        //time 2: synch
        add_frame_to_frame_tl(m_frame_tl_1, 3);
        add_frame_to_frame_tl(m_frame_tl_2, 3);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 3);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 3);
        check_frame(m_frame1, 3);
        check_frame(m_frame2, 3);
        check_matrix(m_matrix1, 3);

        //time 3: no synch, new timestamp is in the past
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        check_frame(m_frame1, 3);
        check_frame(m_frame2, 3);
        check_matrix(m_matrix1, 3);

        //time 3: synch
        add_frame_to_frame_tl(m_frame_tl_1, 6);
        add_frame_to_frame_tl(m_frame_tl_2, 6);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 6);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 6);
        check_frame(m_frame1, 6);
        check_frame(m_frame2, 6);
        check_matrix(m_matrix1, 6);

        //time 4: no synch, timestamp  min(8,8,6)= 6 = last_timestamp
        add_frame_to_frame_tl(m_frame_tl_1, 8);
        add_frame_to_frame_tl(m_frame_tl_2, 8);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        check_frame(m_frame1, 6);
        check_frame(m_frame2, 6);
        check_matrix(m_matrix1, 6);

        //time 5: synch, with timestamp  min(9,8,9)=8 => take frame/matrix closest to 8
        add_frame_to_frame_tl(m_frame_tl_1, 9);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 9);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 8);
        check_frame(m_frame1, 8);
        check_frame(m_frame2, 8);
        check_matrix(m_matrix1, 9);

        m_frame_tl_1->clear_timeline();
        m_frame_tl_2->clear_timeline();
        m_matrix_tl_1->clear_timeline();
        srv->slot("reset")->run();

        //time 6: sync without anything in timeline:
        srv->update().wait();

        //time 7: sync
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        check_frame(m_frame1, 1);
        check_frame(m_frame2, 1);
        check_matrix(m_matrix1, 1);

        //time 8: synch
        add_frame_to_frame_tl(m_frame_tl_1, 2);
        add_frame_to_frame_tl(m_frame_tl_2, 2);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 2);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);
        check_frame(m_frame1, 2);
        check_frame(m_frame2, 2);
        check_matrix(m_matrix1, 2);
    }

    TEST_CASE_FIXTURE(fixture, "single_matrix_tl_config")
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"matrix_tl\">"
           "   <key uid=\"matrixTL1\" />"
           "</in>"
           "<inout group=\"matrix\">"
           "   <key uid=\"matrix0\" tl=\"0\" index=\"1\" sendStatus=\"true\" />"
           "   <key uid=\"matrix1\" tl=\"0\" index=\"0\" />"
           "</inout>"
           "<legacyAutoSync>true</legacyAutoSync>";

        set_config(config_string);

        // create input TLs
        sight::data::matrix_tl::sptr matrix_tl_1 = std::make_shared<sight::data::matrix_tl>();
        matrix_tl_1->init_pool_size(4);
        srv->set_input(matrix_tl_1, "matrix_tl", true, false, 0);

        // create output vars
        auto matrix0 = std::make_shared<sight::data::matrix4>();
        auto matrix1 = std::make_shared<sight::data::matrix4>();
        (*matrix0)(0, 0) = 0.; // init the first value a 0. This will be filled with the timestamp in the tests.
        (*matrix1)(0, 0) = 0.; // init the first value a 0. This will be filled with the timestamp in the tests.
        srv->set_inout(matrix0, "matrix", false, false, 0);
        srv->set_inout(matrix1, "matrix", false, false, 1);

        //start
        srv->start().wait();
        srv->update().wait();

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 1);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 2);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);
        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 4);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 4);
        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 6);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 6);

        //The value in the matrix is  timestamp*10 + element_Index
        check_matrix(matrix0, 61);
        check_matrix(matrix1, 60);

        srv->stop().wait();
    }

    TEST_CASE_FIXTURE(fixture, "mixt_matrix_tl_config")
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"matrix_tl\">"
           "   <key uid=\"matrixTL1\" />"
           "   <key uid=\"matrixTL2\" />"
           "</in>"
           "<inout group=\"matrix\">"
           "    <key uid=\"matrix0\" index=\"1\"/>"
           "    <key uid=\"matrix1\" tl=\"0\" index=\"0\"/>"
           "    <key uid=\"matrix2\" tl=\"1\" index=\"0\"/>"
           "    <key uid=\"matrix3\" tl=\"1\" index=\"1\"/>"
           "    <key uid=\"matrix4\" tl=\"0\" index=\"2\"/>"
           "</inout>"
           "<legacyAutoSync>true</legacyAutoSync>";

        set_config(config_string);

        // create input TLs
        sight::data::matrix_tl::sptr matrix_tl_1 = std::make_shared<sight::data::matrix_tl>();
        sight::data::matrix_tl::sptr matrix_tl_2 = std::make_shared<sight::data::matrix_tl>();
        matrix_tl_1->init_pool_size(4);
        matrix_tl_2->init_pool_size(4);
        srv->set_input(matrix_tl_1, "matrix_tl", true, false, 0);
        srv->set_input(matrix_tl_2, "matrix_tl", true, false, 1);

        // create output vars
        auto matrix0 = std::make_shared<sight::data::matrix4>();
        auto matrix1 = std::make_shared<sight::data::matrix4>();
        auto matrix2 = std::make_shared<sight::data::matrix4>();
        auto matrix3 = std::make_shared<sight::data::matrix4>();
        auto matrix4 = std::make_shared<sight::data::matrix4>();
        (*matrix0)(0, 0) = 0.;
        (*matrix1)(0, 0) = 0.;
        (*matrix2)(0, 0) = 0.;
        (*matrix3)(0, 0) = 0.;
        (*matrix4)(0, 0) = 0.;
        srv->set_inout(matrix0, "matrix", false, false, 0);
        srv->set_inout(matrix1, "matrix", false, false, 1);
        srv->set_inout(matrix2, "matrix", false, false, 2);
        srv->set_inout(matrix3, "matrix", false, false, 3);
        srv->set_inout(matrix4, "matrix", false, false, 4);
        //start
        srv->start().wait();
        srv->update().wait();

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 1);
        add_matrix_to_matrix_tl(matrix_tl_2, {0, 1, 2}, 1);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 3);
        add_matrix_to_matrix_tl(matrix_tl_2, {0, 1, 2}, 2);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);

        //The value in the matrix is  timestamp*10 + element_Index
        check_matrix(matrix0, 31);
        check_matrix(matrix1, 30);
        check_matrix(matrix2, 20);
        check_matrix(matrix3, 21);
        check_matrix(matrix4, 32);

        srv->stop().wait();
    }

    TEST_CASE_FIXTURE(fixture, "single_frame_tl_config")
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"frame_tl\">"
           "    <key uid=\"frameTL\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame\" sendStatus=\"true\" />"
           "</inout>"
           "<legacyAutoSync>true</legacyAutoSync>";

        set_config(config_string);

        // create input TL
        auto frame_tl = std::make_shared<sight::data::frame_tl>();
        frame_tl->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        srv->set_input(frame_tl, "frame_tl", true, false, 0);

        // create output vars
        auto frame = std::make_shared<sight::data::image>();
        frame->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame = frame->dump_lock();
            std::fill(frame->begin<std::uint8_t>(), frame->end<std::uint8_t>(), std::uint8_t(0));
        }

        srv->set_inout(frame, "frames", false, false, 0);

        srv->start().wait();
        srv->update().wait();

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        add_frame_to_frame_tl(frame_tl, 1);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        check_frame(frame, 1);

        add_frame_to_frame_tl(frame_tl, 2);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);
        check_frame(frame, 2);

        add_frame_to_frame_tl(frame_tl, 4);
        add_frame_to_frame_tl(frame_tl, 6);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 6);
        check_frame(frame, 6);

        srv->stop().wait();
    }

    TEST_CASE_FIXTURE(fixture, "mixt_frame_tl_config")
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"frame_tl\">"
           "    <key uid=\"frameTL1\" />"
           "    <key uid=\"frameTL4\" />"
           "    <key uid=\"frameTL6\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame1\" sendStatus=\"true\" />"
           "    <key uid=\"frame6\" tl=\"2\" />"
           "    <key uid=\"frame4\" tl=\"1\" sendStatus=\"false\"/>"
           "    <key uid=\"frame11\" tl=\"0\"  sendStatus=\"true\" />"
           "</inout>"
           "<legacyAutoSync>true</legacyAutoSync>";

        set_config(config_string);

        // create input TL
        auto frame_tl_1 = std::make_shared<sight::data::frame_tl>();
        frame_tl_1->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        auto frame_tl_4 = std::make_shared<sight::data::frame_tl>();
        frame_tl_4->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        auto frame_tl_6 = std::make_shared<sight::data::frame_tl>();
        frame_tl_6->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        srv->set_input(frame_tl_1, "frame_tl", true, false, 0);
        srv->set_input(frame_tl_4, "frame_tl", true, false, 1);
        srv->set_input(frame_tl_6, "frame_tl", true, false, 2);

        // create output vars
        auto frame1 = std::make_shared<sight::data::image>();
        frame1->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame1 = frame1->dump_lock();
            std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
        }
        auto frame4 = std::make_shared<sight::data::image>();
        frame4->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame4 = frame4->dump_lock();
            std::fill(frame4->begin<std::uint8_t>(), frame4->end<std::uint8_t>(), std::uint8_t(0));
        }
        auto frame6 = std::make_shared<sight::data::image>();
        frame6->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame6 = frame6->dump_lock();
            std::fill(frame6->begin<std::uint8_t>(), frame6->end<std::uint8_t>(), std::uint8_t(0));
        }
        auto frame11 = std::make_shared<sight::data::image>();
        frame11->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame11 = frame11->dump_lock();
            std::fill(frame11->begin<std::uint8_t>(), frame11->end<std::uint8_t>(), std::uint8_t(0));
        }

        srv->set_inout(frame1, "frames", false, false, 0);
        srv->set_inout(frame6, "frames", false, false, 1);
        srv->set_inout(frame4, "frames", false, false, 2);
        srv->set_inout(frame11, "frames", false, false, 3);

        srv->start().wait();
        srv->update().wait();

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        add_frame_to_frame_tl(frame_tl_1, 1);
        add_frame_to_frame_tl(frame_tl_4, 1);
        add_frame_to_frame_tl(frame_tl_6, 1);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);

        add_frame_to_frame_tl(frame_tl_1, 4);
        add_frame_to_frame_tl(frame_tl_4, 5);
        add_frame_to_frame_tl(frame_tl_6, 6);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 4);

        check_frame(frame1, 4);
        check_frame(frame4, 5);
        check_frame(frame6, 6);
        check_frame(frame11, 4);

        srv->stop().wait();
    }

    TEST_CASE_FIXTURE(fixture, "full_config")
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"frame_tl\">"
           "    <key uid=\"frameTL1\" />"
           "    <key uid=\"frameTL4\" />"
           "    <key uid=\"frameTL6\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame1\" sendStatus=\"true\" />"
           "    <key uid=\"frame6\" tl=\"2\" />"
           "    <key uid=\"frame4\" tl=\"1\" sendStatus=\"false\"/>"
           "    <key uid=\"frame11\" tl=\"0\"  sendStatus=\"true\" />"
           "</inout>"
           "<in group=\"matrix_tl\">"
           "    <key uid=\"matrixTL1\" />"
           "    <key uid=\"matrixTL2\" />"
           "</in>"
           "<inout group=\"matrix\">"
           "    <key uid=\"matrix0\" index=\"1\" />"
           "    <key uid=\"matrix1\" tl=\"0\" index=\"0\" />"
           "    <key uid=\"matrix2\" tl=\"1\" index=\"0\" sendStatus=\"false\"/>"
           "    <key uid=\"matrix3\" tl=\"1\" index=\"1\"/>"
           "    <key uid=\"matrix4\" tl=\"0\" index=\"2\"/>"
           "</inout>"
           "<tolerance>500</tolerance>"
           "<legacyAutoSync>true</legacyAutoSync>";

        set_config(config_string);
        auto frame_tl_1 = std::make_shared<sight::data::frame_tl>();
        frame_tl_1->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        auto frame_tl_4 = std::make_shared<sight::data::frame_tl>();
        frame_tl_4->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        auto frame_tl_6 = std::make_shared<sight::data::frame_tl>();
        frame_tl_6->init_pool_size(
            m_frame_size[0],
            m_frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        srv->set_input(frame_tl_1, "frame_tl", true, false, 0);
        srv->set_input(frame_tl_4, "frame_tl", true, false, 1);
        srv->set_input(frame_tl_6, "frame_tl", true, false, 2);

        sight::data::matrix_tl::sptr matrix_tl_1 = std::make_shared<sight::data::matrix_tl>();
        sight::data::matrix_tl::sptr matrix_tl_2 = std::make_shared<sight::data::matrix_tl>();
        matrix_tl_1->init_pool_size(4);
        matrix_tl_2->init_pool_size(4);
        srv->set_input(matrix_tl_1, "matrix_tl", true, false, 0);
        srv->set_input(matrix_tl_2, "matrix_tl", true, false, 1);

        // create output vars
        auto frame1 = std::make_shared<sight::data::image>();
        frame1->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame1 = frame1->dump_lock();
            std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
        }
        auto frame4 = std::make_shared<sight::data::image>();
        frame4->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame4 = frame4->dump_lock();
            std::fill(frame4->begin<std::uint8_t>(), frame4->end<std::uint8_t>(), std::uint8_t(0));
        }
        auto frame6 = std::make_shared<sight::data::image>();
        frame6->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame6 = frame6->dump_lock();
            std::fill(frame6->begin<std::uint8_t>(), frame6->end<std::uint8_t>(), std::uint8_t(0));
        }
        auto frame11 = std::make_shared<sight::data::image>();
        frame11->resize(
            m_frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame11 = frame11->dump_lock();
            std::fill(frame11->begin<std::uint8_t>(), frame11->end<std::uint8_t>(), std::uint8_t(0));
        }

        srv->set_inout(frame1, "frames", false, false, 0);
        srv->set_inout(frame6, "frames", false, false, 1);
        srv->set_inout(frame4, "frames", false, false, 2);
        srv->set_inout(frame11, "frames", false, false, 3);

        auto matrix0 = std::make_shared<sight::data::matrix4>();
        auto matrix1 = std::make_shared<sight::data::matrix4>();
        auto matrix2 = std::make_shared<sight::data::matrix4>();
        auto matrix3 = std::make_shared<sight::data::matrix4>();
        auto matrix4 = std::make_shared<sight::data::matrix4>();
        (*matrix0)(0, 0) = 0.;
        (*matrix1)(0, 0) = 0.;
        (*matrix2)(0, 0) = 0.;
        (*matrix3)(0, 0) = 0.;
        (*matrix4)(0, 0) = 0.;
        srv->set_inout(matrix0, "matrix", false, false, 0);
        srv->set_inout(matrix1, "matrix", false, false, 1);
        srv->set_inout(matrix2, "matrix", false, false, 2);
        srv->set_inout(matrix3, "matrix", false, false, 3);
        srv->set_inout(matrix4, "matrix", false, false, 4);

        srv->start().wait();
        srv->update().wait();

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        add_frame_to_frame_tl(frame_tl_1, 1);
        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 1);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);

        add_frame_to_frame_tl(frame_tl_1, 2);
        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 2);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);

        check_frame(frame1, 2);
        check_frame(frame4, 0);
        check_frame(frame6, 0);
        check_frame(frame11, 2);
        check_matrix(matrix0, 21);
        check_matrix(matrix1, 20);
        check_matrix(matrix2, 0);
        check_matrix(matrix3, 0);
        check_matrix(matrix4, 22);

        // populate the remaining TLs.
        // no synch as timestamp is min (2, 3, 4) = 2 == last_timestamp
        add_frame_to_frame_tl(frame_tl_4, 3);
        add_frame_to_frame_tl(frame_tl_6, 4);
        add_matrix_to_matrix_tl(matrix_tl_2, {0, 1, 2}, 4);

        check_frame(frame1, 2);
        check_frame(frame4, 0);
        check_frame(frame6, 0);
        check_frame(frame11, 2);
        check_matrix(matrix0, 21);
        check_matrix(matrix1, 20);
        check_matrix(matrix2, 0);
        check_matrix(matrix3, 0);
        check_matrix(matrix4, 22);

        add_frame_to_frame_tl(frame_tl_1, 5);
        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 5);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 3);

        check_frame(frame1, 2);
        check_frame(frame4, 3);
        check_frame(frame6, 4);
        check_frame(frame11, 2);
        check_matrix(matrix0, 21);
        check_matrix(matrix1, 20);
        check_matrix(matrix2, 40);
        check_matrix(matrix3, 41);
        check_matrix(matrix4, 22);

        add_frame_to_frame_tl(frame_tl_1, 6);
        add_frame_to_frame_tl(frame_tl_4, 6);
        add_frame_to_frame_tl(frame_tl_6, 6);
        add_matrix_to_matrix_tl(matrix_tl_1, {0, 1, 2}, 6);
        add_matrix_to_matrix_tl(matrix_tl_2, {0, 1, 2}, 6);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 6);

        check_frame(frame1, 6);
        check_frame(frame4, 6);
        check_frame(frame6, 6);
        check_frame(frame11, 6);
        check_matrix(matrix0, 61);
        check_matrix(matrix1, 60);
        check_matrix(matrix2, 60);
        check_matrix(matrix3, 61);
        check_matrix(matrix4, 62);

        srv->stop().wait();
    }

    TEST_CASE_FIXTURE(simple_fixture, "update_config")
    {
        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // This is done just to handle automatic synch at first data push
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        srv->slot("reset")->run();

        //time 1: synch
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        check_frame(m_frame1, 1);
        check_frame(m_frame2, 1);
        check_matrix(m_matrix1, 1);

        //time 2: synch
        add_frame_to_frame_tl(m_frame_tl_1, 4);
        add_frame_to_frame_tl(m_frame_tl_2, 5);
        add_matrix_to_matrix_tl(m_matrix_tl_1, {0, 1, 2}, 6);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 4);
        check_frame(m_frame1, 4);
        check_frame(m_frame2, 5);
        check_matrix(m_matrix1, 60);
        srv->slot("setFrameBinding")->run(std::size_t(1), 0U, std::size_t(0));
        srv->slot("setMatrixBinding")->run(std::size_t(0), 1U, std::size_t(0));

        //time 3: synch with frame TL 2 on both frame outputs and matrix on second element
        add_frame_to_frame_tl(m_frame_tl_1, 10);
        add_frame_to_frame_tl(m_frame_tl_2, 11);
        add_matrix_to_matrix_tl(m_matrix_tl_1, {0, 1, 2}, 12);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 10);
        check_frame(m_frame1, 11);
        check_frame(m_frame2, 11);
        check_matrix(m_matrix1, 121);

        srv->slot("setFrameBinding")->run(std::size_t(0), 0U, std::size_t(1));

        //time 4: synch with frame TL1 => frame2 and frameTL2 => frame1
        add_frame_to_frame_tl(m_frame_tl_1, 20);
        add_frame_to_frame_tl(m_frame_tl_2, 21);
        add_matrix_to_matrix_tl(m_matrix_tl_1, {0, 1, 2}, 22);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 20);
        check_frame(m_frame1, 21);
        check_frame(m_frame2, 20);
        check_matrix(m_matrix1, 221);
    }

    TEST_CASE_FIXTURE(fixture, "send_status")
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"frame_tl\">"
           "    <key uid=\"frameTL1\"  />"
           "    <key uid=\"frameTL2\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame1\" sendStatus=\"true\" />"
           "    <key uid=\"frame2\" tl=\"1\" />"
           "</inout>"
           "<in group=\"matrix_tl\">"
           "    <key uid=\"matrixTL\" />"
           "</in>"
           "<inout group=\"matrix\">"
           "    <key uid=\"matrix0\" sendStatus=\"true\" />"
           "</inout>"
           "<tolerance>5</tolerance>";

        set_config(config_string);
        init_standard_in_out();
        srv->start().wait();
        srv->update().wait();

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        std::vector<bool> frame_var_index_synch_signal = {false, false};
        auto slot_frame_synchronized                   =
            sight::core::com::new_slot(
                [&frame_var_index_synch_signal](int _frame_var_index)
        {
            frame_var_index_synch_signal[static_cast<std::size_t>(_frame_var_index)] = true;
        });
        slot_frame_synchronized->set_worker(sight::core::thread::get_default_worker());
        auto frame_synchronized_connection =
            srv->signal("frameSynchronized")->connect(slot_frame_synchronized);

        std::vector<bool> frame_var_index_un_synch_signal = {false, false};
        auto slot_frame_unsynchronized                    =
            sight::core::com::new_slot(
                [&frame_var_index_un_synch_signal](int _frame_var_index)
        {
            frame_var_index_un_synch_signal[static_cast<std::size_t>(_frame_var_index)] = true;
        });
        slot_frame_unsynchronized->set_worker(sight::core::thread::get_default_worker());
        auto frame_unsynchronized_connection =
            srv->signal("frameUnsynchronized")->connect(slot_frame_unsynchronized);

        int matrix_var_index_synch_signal = -1;
        auto slot_matrix_synchronized     =
            sight::core::com::new_slot(
                [&matrix_var_index_synch_signal](int _matrix_var_index)
        {
            matrix_var_index_synch_signal = _matrix_var_index;
        });
        slot_matrix_synchronized->set_worker(sight::core::thread::get_default_worker());
        auto matrix_synchronized_connection =
            srv->signal("matrix_synchronized")->connect(slot_matrix_synchronized);

        int matrix_var_index_un_synch_signal = -1;
        auto slot_matrix_unsynchronized      =
            sight::core::com::new_slot(
                [&matrix_var_index_un_synch_signal](int _matrix_var_index)
        {
            matrix_var_index_un_synch_signal = _matrix_var_index;
        });
        slot_matrix_unsynchronized->set_worker(sight::core::thread::get_default_worker());
        auto matrix_unsynchronized_connection =
            srv->signal("matrix_unsynchronized")->connect(slot_matrix_unsynchronized);

        // This is done just to handle automatic synch at first data push
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        srv->slot("reset")->run();

        //time 1: synch
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        SIGHT_TEST_FAIL_WAIT(matrix_var_index_synch_signal == 0);
        SIGHT_TEST_FAIL_WAIT(frame_var_index_synch_signal[0]);
        CHECK(frame_var_index_synch_signal[1] == false);
        CHECK(frame_var_index_un_synch_signal[0] == false);
        CHECK(frame_var_index_un_synch_signal[1] == false);
        CHECK(matrix_var_index_un_synch_signal == -1);
        frame_var_index_synch_signal[0] = false;
        matrix_var_index_synch_signal   = -1;

        //time 2
        add_frame_to_frame_tl(m_frame_tl_1, 2);
        add_frame_to_frame_tl(m_frame_tl_2, 2);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 2);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);
        CHECK(frame_var_index_synch_signal[0] == false);
        CHECK(frame_var_index_synch_signal[1] == false);
        CHECK(frame_var_index_un_synch_signal[0] == false);
        CHECK(frame_var_index_un_synch_signal[1] == false);
        CHECK(matrix_var_index_synch_signal == -1);
        CHECK(matrix_var_index_un_synch_signal == -1);

        //time 3
        add_frame_to_frame_tl(m_frame_tl_1, 50);
        add_frame_to_frame_tl(m_frame_tl_2, 50);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 3);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 50);
        SIGHT_TEST_FAIL_WAIT(matrix_var_index_un_synch_signal == 0);
        CHECK(frame_var_index_synch_signal[0] == false);
        CHECK(frame_var_index_synch_signal[1] == false);
        CHECK(frame_var_index_un_synch_signal[0] == false);
        CHECK(frame_var_index_un_synch_signal[1] == false);
        CHECK(matrix_var_index_synch_signal == -1);
        CHECK(matrix_var_index_un_synch_signal == 0);
        matrix_var_index_un_synch_signal = -1;

        //time 4
        add_frame_to_frame_tl(m_frame_tl_1, 51);
        add_frame_to_frame_tl(m_frame_tl_2, 51);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 51);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 51);
        SIGHT_TEST_FAIL_WAIT(matrix_var_index_synch_signal == 0);
        CHECK(frame_var_index_synch_signal[0] == false);
        CHECK(frame_var_index_synch_signal[1] == false);
        CHECK(frame_var_index_un_synch_signal[0] == false);
        CHECK(frame_var_index_un_synch_signal[1] == false);
        CHECK(matrix_var_index_synch_signal == 0);
        CHECK(matrix_var_index_un_synch_signal == -1);
        matrix_var_index_synch_signal = -1;

        //time 5
        add_frame_to_frame_tl(m_frame_tl_1, 53);
        add_frame_to_frame_tl(m_frame_tl_2, 100);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 100);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 100);
        SIGHT_TEST_FAIL_WAIT(frame_var_index_un_synch_signal[0]);
        CHECK(frame_var_index_synch_signal[0] == false);
        CHECK(frame_var_index_synch_signal[1] == false);
        CHECK(frame_var_index_un_synch_signal[1] == false);
        CHECK(matrix_var_index_synch_signal == -1);
        CHECK(matrix_var_index_un_synch_signal == -1);
        frame_var_index_un_synch_signal[0] = false;

        //time 6
        add_frame_to_frame_tl(m_frame_tl_1, 101);
        add_frame_to_frame_tl(m_frame_tl_2, 101);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 101);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 101);
        SIGHT_TEST_FAIL_WAIT(frame_var_index_synch_signal[0]);
        CHECK(frame_var_index_synch_signal[1] == false);
        CHECK(frame_var_index_un_synch_signal[0] == false);
        CHECK(frame_var_index_un_synch_signal[1] == false);
        CHECK(matrix_var_index_synch_signal == -1);
        CHECK(matrix_var_index_un_synch_signal == -1);
        frame_var_index_synch_signal[0] = false;
    }

    TEST_CASE_FIXTURE(fixture, "delay")
    {
        std::stringstream config_string;
        config_string
        << "<in group=\"frame_tl\">"
           "    <key uid=\"frameTL1\" delay=\"2\" />"
           "    <key uid=\"frameTL2\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame1\" />"
           "    <key uid=\"frame2\" tl=\"1\" />"
           "</inout>"
           "<in group=\"matrix_tl\">"
           "    <key uid=\"matrixTL\" delay=\"3\"/>"
           "</in>"
           "<inout group=\"matrix\">"
           "    <key uid=\"matrix0\" />"
           "</inout>"
           "<tolerance>5</tolerance>"
           "<legacyAutoSync>true</legacyAutoSync>";

        set_config(config_string);
        init_standard_in_out();
        srv->start().wait();
        srv->update().wait();

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        //time 1: synch on timestamp 4, expected (2,4,1)
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        add_frame_to_frame_tl(m_frame_tl_1, 2);
        add_frame_to_frame_tl(m_frame_tl_2, 2);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 2);
        add_frame_to_frame_tl(m_frame_tl_1, 3);
        add_frame_to_frame_tl(m_frame_tl_2, 3);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 3);
        add_frame_to_frame_tl(m_frame_tl_1, 4);
        add_frame_to_frame_tl(m_frame_tl_2, 4);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 4);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 4);
        check_frame(m_frame1, 2);
        check_frame(m_frame2, 4);
        check_matrix(m_matrix1, 1);

        //time 2: synch on timestamp 5, expected (3,5,2)
        add_frame_to_frame_tl(m_frame_tl_1, 5);
        add_frame_to_frame_tl(m_frame_tl_2, 5);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 5);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 5);
        check_frame(m_frame1, 3);
        check_frame(m_frame2, 5);
        check_matrix(m_matrix1, 2);

        srv->slot("set_delay")->run(1, std::string("frameDelay_0"));
        srv->slot("set_delay")->run(-1, std::string("frameDelay_1"));
        srv->slot("set_delay")->run(-1, std::string("frameDelay_2"));
        srv->slot("set_delay")->run(-1, std::string("matrixDelay_0"));
        srv->slot("set_delay")->run(2, std::string("matrixDelay_0"));
        srv->slot("set_delay")->run(1, std::string("matrixDelay_1"));

        //time 3: synch on timestamp 6, expected (5,6,4)
        add_frame_to_frame_tl(m_frame_tl_1, 6);
        add_frame_to_frame_tl(m_frame_tl_2, 6);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 6);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 6);
        check_frame(m_frame1, 5);
        check_frame(m_frame2, 6);
        check_matrix(m_matrix1, 4);

        srv->slot("set_delay")->run(0, std::string("frameDelay_0"));
        srv->slot("set_delay")->run(0, std::string("matrixDelay_0"));

        //time 4: synch on timestamp 7, expected (7,7,7)
        add_frame_to_frame_tl(m_frame_tl_1, 7);
        add_frame_to_frame_tl(m_frame_tl_2, 7);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 7);
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 7);
        check_frame(m_frame1, 7);
        check_frame(m_frame2, 7);
        check_matrix(m_matrix1, 7);
    }

    TEST_CASE_FIXTURE(simple_fixture, "tolerance")
    {
        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // This is done just to handle automatic synch at first data push
        add_frame_to_frame_tl(m_frame_tl_1, 1);
        srv->slot("reset")->run();

        //time 1: synch
        add_frame_to_frame_tl(m_frame_tl_2, 1);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 1);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 1);
        check_frame(m_frame1, 1);
        check_frame(m_frame2, 1);
        check_matrix(m_matrix1, 1);

        //time 2: synch, all good, diff is bellow tolerance which is 5
        add_frame_to_frame_tl(m_frame_tl_1, 5);
        add_frame_to_frame_tl(m_frame_tl_2, 5);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 4);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 4);
        check_frame(m_frame1, 5);
        check_frame(m_frame2, 5);
        check_matrix(m_matrix1, 4);

        //time 3: frame 2 is stuck. Tolerance is out
        add_frame_to_frame_tl(m_frame_tl_1, 50);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 50);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 50);
        check_frame(m_frame1, 50);
        check_frame(m_frame2, 5);
        check_matrix(m_matrix1, 50);

        //time 3: frame 2 caches up
        add_frame_to_frame_tl(m_frame_tl_1, 53);
        add_frame_to_frame_tl(m_frame_tl_2, 54);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 53);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 53);
        check_frame(m_frame1, 53);
        check_frame(m_frame2, 54);
        check_matrix(m_matrix1, 53);

        //time 4: matrix is stuck
        add_frame_to_frame_tl(m_frame_tl_1, 100);
        add_frame_to_frame_tl(m_frame_tl_2, 100);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 55);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 100);
        check_frame(m_frame1, 100);
        check_frame(m_frame2, 100);
        check_matrix(m_matrix1, 53);

        //time 5: matrix catches up
        add_frame_to_frame_tl(m_frame_tl_1, 110);
        add_frame_to_frame_tl(m_frame_tl_2, 110);
        add_matrix_to_matrix_tl(m_matrix_tl_1, 111);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 110);
        check_frame(m_frame1, 110);
        check_frame(m_frame2, 110);
        check_matrix(m_matrix1, 111);
    }

    TEST_CASE_FIXTURE(fixture, "image_series_time_tagging")
    {
        const sight::data::image::size_t frame_size {2, 2, 1};

        std::stringstream config_string;
        config_string
        << "<in group=\"frame_tl\">"
           "    <key uid=\"frameTL1\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame1\" />"
           "</inout>";
        // sight::service::config_t config;
        // boost::property_tree::read_xml(config_string, config);

        set_config(config_string);

        srv->start().wait();
        srv->update().wait();

        // create and set the input TL
        sight::data::frame_tl::sptr frame_tl_1 = std::make_shared<sight::data::frame_tl>();
        frame_tl_1->init_pool_size(
            frame_size[0],
            frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        srv->set_input(frame_tl_1, "frame_tl", true, false, 0);

        // create and set the inout which will be filled in the synchronization process
        sight::data::image_series::sptr frame1 = std::make_shared<sight::data::image_series>();

        frame1->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
        frame1->resize(
            frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame1 = frame1->dump_lock();
            std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
        }

        srv->set_inout(frame1, "frames", false, false, 0);

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection =
            srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // This is done just to handle automatic synch at first data push
        const std::uint64_t timestamp = 13;

        const SPTR(sight::data::frame_tl::buffer_t) data = frame_tl_1->create_buffer(timestamp);
        std::uint8_t* elt_buffer = data->add_element(0);
        memset(elt_buffer, timestamp, frame_size[0] * frame_size[1]);

        frame_tl_1->push_object(data);

        srv->slot("try_sync")->run();

        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == timestamp);

        const auto dt = frame1->get_frame_acquisition_date_time(0);
        CHECK(dt.has_value());

        const auto time_point = frame1->get_frame_acquisition_time_point(0);
        CHECK(time_point.has_value());

        std::int64_t ts = std::chrono::duration_cast<std::chrono::milliseconds>(time_point->time_since_epoch()).count();
        CHECK(ts == timestamp);

        srv->stop().wait();
    }

    TEST_CASE_FIXTURE(fixture, "single_image_series_tl_population")
    {
        const sight::data::image::size_t frame_size {2, 2, 1};

        /// Service setup
        std::stringstream config_string;
        config_string
        << "<in group=\"frame_tl\">"
           "    <key uid=\"frameTL1\" />"
           "    <key uid=\"frameTL2\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame1\" tl=\"0\"/>"
           "    <key uid=\"frame2\" tl=\"1\" />"
           "</inout>"
           "<in group=\"matrix_tl\">"
           "    <key uid=\"matrixTL1\" />"
           "</in>"
           "<inout group=\"matrix\">"
           "    <key uid=\"matrix1\" tl=\"0\"/>"
           "</inout>"
           "<tolerance>5</tolerance>";

        set_config(config_string);

        srv->start().wait();
        srv->update().wait();

        /// Input/output setup
        // create and set the input TL
        sight::data::frame_tl::sptr frame_tl_1 = std::make_shared<sight::data::frame_tl>();
        frame_tl_1->init_pool_size(
            frame_size[0],
            frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );
        sight::data::frame_tl::sptr frame_tl_2 = std::make_shared<sight::data::frame_tl>();
        frame_tl_2->init_pool_size(
            frame_size[0],
            frame_size[1],
            sight::core::type::UINT8,
            sight::data::frame_tl::pixel_format::gray_scale
        );

        sight::data::matrix_tl::sptr matrix_tl_1 = std::make_shared<sight::data::matrix_tl>();
        matrix_tl_1->init_pool_size(4);

        srv->set_input(frame_tl_1, "frame_tl", true, false, 0);
        srv->set_input(frame_tl_2, "frame_tl", true, false, 1);
        srv->set_input(matrix_tl_1, "matrix_tl", true, false, 0);

        // create and set the inout which will be filled in the synchronization process
        sight::data::image_series::sptr frame1 = std::make_shared<sight::data::image_series>();

        frame1->set_sop_keyword(sight::data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
        frame1->resize(
            frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame1 = frame1->dump_lock();
            std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
        }

        sight::data::image::sptr frame2 = std::make_shared<sight::data::image>();
        frame2->resize(
            frame_size,
            sight::core::type::UINT8,
            sight::data::image::pixel_format_t::gray_scale
        );
        {
            auto dump_lock_frame2 = frame2->dump_lock();
            std::fill(frame2->begin<std::uint8_t>(), frame2->end<std::uint8_t>(), std::uint8_t(0));
        }

        /// Processing
        sight::data::matrix4::sptr matrix1 = std::make_shared<sight::data::matrix4>();
        (*matrix1)(0, 0) = 0.; // init the first value a 0. This will be filled with the timestamp in the tests.

        srv->set_inout(frame1, "frames", false, false, 0);
        srv->set_inout(frame2, "frames", false, false, 1);
        srv->set_inout(matrix1, "matrix", false, false, 0);

        sight::core::clock::type last_timestamp_synch = 0;
        auto slot_synchronization_done                =
            sight::core::com::new_slot(
                [&last_timestamp_synch](sight::core::clock::type _timestamp)
        {
            last_timestamp_synch = _timestamp;
        });
        slot_synchronization_done->set_worker(sight::core::thread::get_default_worker());
        auto synch_done_connection = srv->signal("synchronization_done")->connect(slot_synchronization_done);

        // populate the TLs
        add_frame_to_frame_tl(frame_tl_1, 1);
        add_frame_to_frame_tl(frame_tl_1, 2);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 2);
        //test the output
        check_frame(frame1, 2);
        check_frame(frame2, 0);
        check_matrix(matrix1, 0);

        std::optional<std::string> dt;
        std::optional<std::chrono::system_clock::time_point> time_point;
        std::int64_t ts = 0;

        // Check that the ImageSeries timestamp was written
        dt = frame1->get_frame_acquisition_date_time(0);
        CHECK(dt.has_value());
        time_point = frame1->get_frame_acquisition_time_point(0);
        CHECK(time_point.has_value());
        ts = std::chrono::duration_cast<std::chrono::milliseconds>(time_point->time_since_epoch()).count();
        CHECK(ts == 2);

        // populate the TL
        add_frame_to_frame_tl(frame_tl_1, 3);
        srv->slot("request_sync")->run();
        srv->slot("try_sync")->run();
        SIGHT_TEST_FAIL_WAIT(last_timestamp_synch == 3);
        //test the output
        check_frame(frame1, 3);
        check_frame(frame2, 0);
        check_matrix(matrix1, 0);

        // Check that the ImageSeries timestamp was written
        dt = frame1->get_frame_acquisition_date_time(0);
        CHECK(dt.has_value());
        time_point = frame1->get_frame_acquisition_time_point(0);
        CHECK(time_point.has_value());
        ts = std::chrono::duration_cast<std::chrono::milliseconds>(time_point->time_since_epoch()).count();
        CHECK(ts == 3);

        srv->stop().wait();
    }
}
