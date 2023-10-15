/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "synchronizer_test.hpp"

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

#include <utest/wait.hpp>

#include <boost/config.hpp>
#include <boost/property_tree/xml_parser.hpp>

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION(sight::module::sync::ut::synchronizer_test);

namespace sight::module::sync::ut
{

//------------------------------------------------------------------------------

void synchronizer_test::setUp()
{
    // Set up context before running a test.
    core::runtime::init();
    core::runtime::load_module(std::string("sight::module::sync"));
}

//------------------------------------------------------------------------------

void synchronizer_test::tearDown()
{
}

//------------------------------------------------------------------------------

class SynchronizerTester final
{
public:

    SynchronizerTester()
    {
        srv = service::add("sight::module::sync::synchronizer");
        CPPUNIT_ASSERT(srv != nullptr);

        initStandardInOut();

        std::stringstream config_string;
        config_string
        << "<in group=\"frameTL\">"
           "    <key uid=\"frameTL1\" />"
           "    <key uid=\"frameTL2\" />"
           "</in>"
           "<inout group=\"frames\">"
           "    <key uid=\"frame1\" tl=\"0\"/>"
           "    <key uid=\"frame2\" tl=\"1\" />"
           "</inout>"
           "<in group=\"matrixTL\">"
           "    <key uid=\"matrixTL1\" />"
           "</in>"
           "<inout group=\"matrix\">"
           "    <key uid=\"matrix1\" tl=\"0\"/>"
           "</inout>"
           "<tolerance>5</tolerance>";
        service::config_t config;
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);

        srv->configure();
        srv->start().wait();
        srv->update().wait();
    }

    explicit SynchronizerTester(std::stringstream& config_string)
    {
        srv = service::add("sight::module::sync::synchronizer");
        CPPUNIT_ASSERT(srv != nullptr);
        service::config_t config;
        boost::property_tree::read_xml(config_string, config);
        srv->set_config(config);

        srv->configure();
    }

    //------------------------------------------------------------------------------

    ~SynchronizerTester()
    {
        if(srv->started())
        {
            srv->stop().wait();
        }

        service::remove(srv);
    }

    //------------------------------------------------------------------------------
    void initStandardInOut()
    {
        // create and set the input TL
        frameTL1 = std::make_shared<data::frame_tl>();
        frameTL1->initPoolSize(
            frameSize[0],
            frameSize[1],
            core::type::UINT8,
            sight::data::frame_tl::PixelFormat::GRAY_SCALE
        );
        frameTL2 = std::make_shared<data::frame_tl>();
        frameTL2->initPoolSize(
            frameSize[0],
            frameSize[1],
            core::type::UINT8,
            sight::data::frame_tl::PixelFormat::GRAY_SCALE
        );
        matrixTL1 = std::make_shared<data::matrix_tl>();
        matrixTL1->initPoolSize(4);

        srv->set_input(frameTL1, "frameTL", true, false, 0);
        srv->set_input(frameTL2, "frameTL", true, false, 1);
        srv->set_input(matrixTL1, "matrixTL", true, false, 0);

        // create and set the inout which will be filled in the synchronization process
        frame1 = std::make_shared<data::image>();
        frame1->resize(
            frameSize,
            core::type::UINT8,
            data::image::PixelFormat::GRAY_SCALE
        );
        {
            auto dump_lock_frame1 = frame1->dump_lock();
            std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
        }

        frame2 = std::make_shared<data::image>();
        frame2->resize(
            frameSize,
            core::type::UINT8,
            data::image::PixelFormat::GRAY_SCALE
        );
        {
            auto dump_lock_frame2 = frame2->dump_lock();
            std::fill(frame2->begin<std::uint8_t>(), frame2->end<std::uint8_t>(), std::uint8_t(0));
        }

        matrix1          = std::make_shared<data::matrix4>();
        (*matrix1)(0, 0) = 0.; // init the first value a 0. This will be filled with the timestamp in the tests.

        srv->set_inout(frame1, "frames", false, false, 0);
        srv->set_inout(frame2, "frames", false, false, 1);
        srv->set_inout(matrix1, "matrix", false, false, 0);
    }

    //------------------------------------------------------------------------------

    void addFrameToFrameTL(data::frame_tl::sptr& _frameTL, const std::uint8_t timestamp)
    {
        const SPTR(data::frame_tl::BufferType) data = _frameTL->createBuffer(timestamp);
        std::uint8_t* eltBuffer = data->addElement(0);
        memset(eltBuffer, timestamp, frameSize[0] * frameSize[1]);

        _frameTL->pushObject(data);
        srv->update().wait();
    }

    //------------------------------------------------------------------------------

    void addMatrixToMatrixTL(data::matrix_tl::sptr& _matrixTL, unsigned int elementIndex, const std::uint8_t timestamp)
    {
        const SPTR(data::matrix_tl::BufferType) data = _matrixTL->createBuffer(timestamp);
        std::array<float, 16> matrix {1., 0., 0., 0.,
                                      0., 1., 0., 0.,
                                      0., 0., 1., 0.,
                                      0., 0., 0., 1.
        };
        matrix[0] = float(timestamp);
        data->setElement(matrix, elementIndex);
        _matrixTL->pushObject(data);
        srv->update().wait();
    }

    //------------------------------------------------------------------------------

    void addMatrixToMatrixTL(data::matrix_tl::sptr& _matrixTL, const std::uint8_t timestamp)
    {
        addMatrixToMatrixTL(_matrixTL, 0, timestamp);
    }

    //------------------------------------------------------------------------------

    void addMatrixToMatrixTL(
        data::matrix_tl::sptr& _matrixTL,
        std::vector<unsigned int> elementIndexList,
        const std::uint8_t timestamp
)
    {
        // The purpose here is to fill a matrix TL with a new matrix, which needs to store
        // some information, to be able to find the creation timestamp and element Index
        // once it is synchronized, in the out variable. TO do this, the first value is filled with
        // timestamp * 10 + elementIndex value.
        // by testing the first value of the matrix, it  is possible to verify that the
        // appropriate matrix has be put to a given output.

        const SPTR(data::matrix_tl::BufferType) data = _matrixTL->createBuffer(timestamp);

        for(const auto elementIndex : elementIndexList)
        {
            std::array<float, 16> matrix {1., 0., 0., 0.,
                                          0., 1., 0., 0.,
                                          0., 0., 1., 0.,
                                          0., 0., 0., 1.
            };
            matrix[0] = float(timestamp * 10 + elementIndex);
            data->setElement(matrix, elementIndex);
        }

        _matrixTL->pushObject(data);
        srv->update().wait();
    }

    //------------------------------------------------------------------------------

    void static checkMatrix(const data::matrix4::sptr& _matrix, const double _value)
    {
        // The purpose here is to verify that the appropriate matrix has been put in an output variable.
        // To do this, the addMatrixToMatrixTL method fills the first value of the matrix with
        // timestamp * 10 + elementIndex value.
        // by testing the first value of the matrix, it  is possible to verify that the
        // appropriate matrix has be put to a given output.

        CPPUNIT_ASSERT(_matrix);

        //test all values which should be 0, except the diagonal one which are 1. and the first one which is the
        // timestamp.
        for(std::size_t i = 1 ; i < 16 ; ++i)
        {
            const double matrixValue   = (*_matrix)[i];
            const double expectedValue = i % 5 == 0 ? 1. : 0.;
            CPPUNIT_ASSERT_EQUAL(expectedValue, matrixValue);
        }

        const double matrixValue = (*_matrix)(0, 0);
        CPPUNIT_ASSERT_EQUAL(_value, matrixValue);
    }

    //------------------------------------------------------------------------------

    void static checkFrame(
        const data::image::sptr& _image,
        const std::uint8_t _value
)
    {
        CPPUNIT_ASSERT(_image);
        const auto dump_lock                                  = _image->dump_lock();
        const data::image::const_iterator<std::uint8_t> begin = _image->cbegin<std::uint8_t>();
        const data::image::const_iterator<std::uint8_t> end   = _image->cend<std::uint8_t>();
        // Avoid mistakes in case the range is null
        CPPUNIT_ASSERT(begin != end);

        std::for_each(
            begin,
            end,
            [&_value](const auto& x)
            {
                CPPUNIT_ASSERT_EQUAL(_value, x);
            });
    }

    service::base::sptr srv;

    data::frame_tl::sptr frameTL1;
    data::frame_tl::sptr frameTL2;
    data::matrix_tl::sptr matrixTL1;
    data::image::sptr frame1;
    data::image::sptr frame2;
    data::matrix4::sptr matrix1;

    const data::image::Size frameSize {2, 2, 1};
    const int synchWaitDurationMs {60};
};

//------------------------------------------------------------------------------

void synchronizer_test::singleFrameTLPopulation()
{
    SynchronizerTester tester;
    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection = tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    // populate the TLs
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.addFrameToFrameTL(tester.frameTL1, 2);
    tester.srv->slot("synchronize")->run();
    //test the output
    SynchronizerTester::checkFrame(tester.frame1, 2);
    SynchronizerTester::checkFrame(tester.frame2, 0);
    SynchronizerTester::checkMatrix(tester.matrix1, 0);

    // populate the TL
    tester.addFrameToFrameTL(tester.frameTL1, 3);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 3);
    //test the output
    SynchronizerTester::checkFrame(tester.frame1, 3);
    SynchronizerTester::checkFrame(tester.frame2, 0);
    SynchronizerTester::checkMatrix(tester.matrix1, 0);
}

//------------------------------------------------------------------------------

void synchronizer_test::singleMatrixTLPopulation()
{
    SynchronizerTester tester;
    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection = tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 2);
    tester.srv->slot("synchronize")->run();
    SynchronizerTester::checkFrame(tester.frame1, 0);
    SynchronizerTester::checkFrame(tester.frame2, 0);
    SynchronizerTester::checkMatrix(tester.matrix1, 2);

    tester.addMatrixToMatrixTL(tester.matrixTL1, 3);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 3);
    SynchronizerTester::checkFrame(tester.frame1, 0);
    SynchronizerTester::checkFrame(tester.frame2, 0);
    SynchronizerTester::checkMatrix(tester.matrix1, 3);
}

//------------------------------------------------------------------------------

/// Classical synchronisation with variations
// FrameTL1  <= f1_1 <= f1_2 <= f1_6 <= f1_8 <= f1_9 <= f1_10 <= f1_11 <= f1_12 <= f1_13 <= f1_14 <= f1_15
// FrameTL2  <= f2_1 <= f2_2 <= f2_6 <= f2_8                           <= f2_12 <= f2_13          <= f2_15
// MatrixTL1 <= m1_1 <= m1_2 <= m1_6         <= m1_9          <= m1_11                   <= m1_14 <= m1_15
//
// at each FrameTL1 a synchronisation is done.
// expected result:
// ref tsp  :  1   ,  2   ,  6   ,  8   ,   11  ,  13   ,  15
//  Frame 1 : f1_1 , f1_2 , f1_6 , f1_8 , f1_11 , f1_13 , f1_15
//  Frame 2 : f2_1 , f2_2 , f2_6 , f2_8 , f2_12 , f2_13 , f2_15
// Matrix 1 : m1_1 , m1_2 , m1_6 , m1_9 , m1_11 , m1_14 , m1_15
//
//
// details of the synchronisation:
//
//  step 1: synchTimestamp  = min (1 ,1 ,1 ) => 1 forward (f1_1, f2_1, m1_1)
//  step 2: synchTimestamp  = min (2 ,2 ,2 ) => 2 forward (f1_2, f2_2, m1_2)
//  step 3: synchTimestamp  = min (6 ,6 ,6 ) => 6 forward (f1_6, f2_6, m1_6)
//  step 4: synchTimestamp  = min (8 ,8 ,6 ) => 6 same as before: do nothing
//  step 5: synchTimestamp  = min (9 ,8 ,9 ) => 8 forward (f1_8, f2_8, m1_9)
//  step 6: synchTimestamp  = min (10 ,8 ,9 ) => 8 same as before: do nothing
//  step 7: synchTimestamp  = min (11 ,8 ,11 ) => 8 same as before: do nothing
//  step 8: synchTimestamp  = min (12 ,12 ,11 ) => 11 forward (f1_11, f2_12, m1_11)
//  step 9: synchTimestamp  = min (13 ,13 ,11 ) => 11 same as before: do nothing
//  step 10: synchTimestamp  = min (14 ,13 ,14 ) => 13 forward (f1_13, f2_13, m1_14)
//  step 11: synchTimestamp  = min (15 ,15 ,15 ) => 15 forward (f1_15, f2_15, m1_15)

void synchronizer_test::basicSynchronisation()
{
    SynchronizerTester tester;

    //signal - slot connections for tests
    core::hires_clock::type lastTimestampSynch = 0;
    bool synchronizationSkippedReceived        = false;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);
    auto slotSynchronizationSkippedReceived = sight::core::com::new_slot(
        [&synchronizationSkippedReceived]()
        {
            synchronizationSkippedReceived = true;
        });
    slotSynchronizationSkippedReceived->set_worker(sight::core::thread::get_default_worker());
    auto synchSkippedConnection = tester.srv->signal("synchronizationSkipped")->connect(
        slotSynchronizationSkippedReceived
    );

    // This is done just to handle automatic synch at first data push
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.srv->slot("reset")->run();

    //time 1: synch
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    SynchronizerTester::checkFrame(tester.frame1, 1);
    SynchronizerTester::checkFrame(tester.frame2, 1);
    SynchronizerTester::checkMatrix(tester.matrix1, 1);

    //time 2: synch
    tester.addFrameToFrameTL(tester.frameTL1, 2);
    tester.addFrameToFrameTL(tester.frameTL2, 2);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 2);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);
    SynchronizerTester::checkFrame(tester.frame1, 2);
    SynchronizerTester::checkFrame(tester.frame2, 2);
    SynchronizerTester::checkMatrix(tester.matrix1, 2);

    //time 3: synch
    tester.addFrameToFrameTL(tester.frameTL1, 6);
    tester.addFrameToFrameTL(tester.frameTL2, 6);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 6);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 6);
    SynchronizerTester::checkFrame(tester.frame1, 6);
    SynchronizerTester::checkFrame(tester.frame2, 6);
    SynchronizerTester::checkMatrix(tester.matrix1, 6);

    //time 4: no synch, as new timestamp is min (8, 8, 6) = 6 = last_timestamp
    tester.addFrameToFrameTL(tester.frameTL1, 8);
    tester.addFrameToFrameTL(tester.frameTL2, 8);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame1, 6);
    SynchronizerTester::checkFrame(tester.frame2, 6);
    SynchronizerTester::checkMatrix(tester.matrix1, 6);

    //time 5: synch, with timestamp  min(9,8,9)=8 => take frame/matrix closest to 8
    tester.addFrameToFrameTL(tester.frameTL1, 9);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 9);
    fwTestWithFailWaitMacro(lastTimestampSynch == 8);
    SynchronizerTester::checkFrame(tester.frame1, 8);
    SynchronizerTester::checkFrame(tester.frame2, 8);
    SynchronizerTester::checkMatrix(tester.matrix1, 9);

    //time 6: no synch, as new timestamp is min (10, 8, 9) = 8 = last_timestamp
    tester.addFrameToFrameTL(tester.frameTL1, 10);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame1, 8);
    SynchronizerTester::checkFrame(tester.frame2, 8);
    SynchronizerTester::checkMatrix(tester.matrix1, 9);

    //time 7: no synch, as new timestamp is min (11, 8, 11) = 8 = last_timestamp
    tester.addFrameToFrameTL(tester.frameTL1, 11);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 11);
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame2, 8);
    SynchronizerTester::checkMatrix(tester.matrix1, 9);

    //time 8: synch, with timestamp  min(12,12,11)=11 => take frame/matrix closest to 11
    tester.addFrameToFrameTL(tester.frameTL1, 12);
    tester.addFrameToFrameTL(tester.frameTL2, 12);
    fwTestWithFailWaitMacro(lastTimestampSynch == 11);
    SynchronizerTester::checkFrame(tester.frame1, 11);
    SynchronizerTester::checkFrame(tester.frame2, 12);
    SynchronizerTester::checkMatrix(tester.matrix1, 11);

    //time 9: no synch, as new timestamp is min (13, 13, 11) = 11 = last_timestamp
    tester.addFrameToFrameTL(tester.frameTL1, 13);
    tester.addFrameToFrameTL(tester.frameTL2, 13);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame1, 11);
    SynchronizerTester::checkFrame(tester.frame2, 12);
    SynchronizerTester::checkMatrix(tester.matrix1, 11);

    //time 10: synch, with timestamp  min(14,13,14)=13 => take frame/matrix closest to 13
    tester.addFrameToFrameTL(tester.frameTL1, 14);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 14);
    fwTestWithFailWaitMacro(lastTimestampSynch == 13);
    SynchronizerTester::checkFrame(tester.frame1, 13);
    SynchronizerTester::checkFrame(tester.frame2, 13);
    SynchronizerTester::checkMatrix(tester.matrix1, 14);

    //time 11: synch
    tester.addFrameToFrameTL(tester.frameTL1, 15);
    tester.addFrameToFrameTL(tester.frameTL2, 15);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 15);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 15);
    SynchronizerTester::checkFrame(tester.frame1, 15);
    SynchronizerTester::checkFrame(tester.frame2, 15);
    SynchronizerTester::checkMatrix(tester.matrix1, 15);
}

//------------------------------------------------------------------------------

///Classical synchronisation with time gap
// FrameTL1  <= f1_1 <= f1_2  <= f1_50 <= f1_51 <= f1_52 <= f1_53 <= f1_54 <= f1_55
// FrameTL2  <= f2_1 <= f2_2  <= f2_49 <= f2_50          <= f2_51 <= f2_52 <= f2_55
// MatrixTL1 <= m1_1 <= m1_2         <= m1_3  <= m1_49 <= m1_50          <= m1_52 <= m1_55
//
// at each FrameTL1 a synchronisation is done.
// expected result:
// ref tsp  :  1   ,  2   ,  49   ,  50   ,   52  ,  55
//  Frame 1 : f1_1 , f1_2 , f1_50 , f1_50 , f1_52 , f1_55
//  Frame 2 : f2_1 , f2_2 , f2_49 , f2_50 , f2_52 , f2_55
// Matrix 1 : m1_1 , m1_2 , m1_2  , m1_50 , m1_52 , m1_55
void synchronizer_test::timeGapSynchronisation()
{
    SynchronizerTester tester;
    core::hires_clock::type lastTimestampSynch = 0;
    bool synchronizationSkippedReceived        = false;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);
    auto slotSynchronizationSkippedReceived = sight::core::com::new_slot(
        [&synchronizationSkippedReceived]()
        {
            synchronizationSkippedReceived = true;
        });
    slotSynchronizationSkippedReceived->set_worker(sight::core::thread::get_default_worker());
    auto synchSkippedConnection = tester.srv->signal("synchronizationSkipped")->connect(
        slotSynchronizationSkippedReceived
    );

    // This is done just to handle automatic synch at first data push
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.srv->slot("reset")->run();

    //time 1: synch
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    SynchronizerTester::checkFrame(tester.frame1, 1);
    SynchronizerTester::checkFrame(tester.frame2, 1);
    SynchronizerTester::checkMatrix(tester.matrix1, 1);

    //time 2: synch
    tester.addFrameToFrameTL(tester.frameTL1, 2);
    tester.addFrameToFrameTL(tester.frameTL2, 2);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 2);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);
    SynchronizerTester::checkFrame(tester.frame1, 2);
    SynchronizerTester::checkFrame(tester.frame2, 2);
    SynchronizerTester::checkMatrix(tester.matrix1, 2);

    //time 3: ignore matrix too far away  timestamp  min(50, 49)=49
    tester.addFrameToFrameTL(tester.frameTL1, 50);
    tester.addFrameToFrameTL(tester.frameTL2, 49);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 3);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 49);
    SynchronizerTester::checkFrame(tester.frame1, 50);
    SynchronizerTester::checkFrame(tester.frame2, 49);
    SynchronizerTester::checkMatrix(tester.matrix1, 2);

    //time 4: synch, with timestamp  min(51,50,49)=49 => last_timestamp
    tester.addFrameToFrameTL(tester.frameTL1, 51);
    tester.addFrameToFrameTL(tester.frameTL2, 50);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 49);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 49);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame1, 50);
    SynchronizerTester::checkFrame(tester.frame2, 49);
    SynchronizerTester::checkMatrix(tester.matrix1, 2);

    //time 5: synch, with timestamp  min(52,50,50)=50 => take frame/matrix closest to 50
    tester.addFrameToFrameTL(tester.frameTL1, 52);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 50);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 50);
    SynchronizerTester::checkFrame(tester.frame1, 50);
    SynchronizerTester::checkFrame(tester.frame2, 50);
    SynchronizerTester::checkMatrix(tester.matrix1, 50);

    //time 6: no synch, as new timestamp is min (53, 51, 50) = 50 = last_timestamp
    tester.addFrameToFrameTL(tester.frameTL1, 53);
    tester.addFrameToFrameTL(tester.frameTL2, 51);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame1, 50);
    SynchronizerTester::checkFrame(tester.frame2, 50);
    SynchronizerTester::checkMatrix(tester.matrix1, 50);

    //time 7: synch, with timestamp  min(54,52,52)=52 => take frame/matrix closest to 52
    tester.addFrameToFrameTL(tester.frameTL1, 54);
    tester.addFrameToFrameTL(tester.frameTL2, 52);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 52);
    fwTestWithFailWaitMacro(lastTimestampSynch == 52);
    SynchronizerTester::checkFrame(tester.frame1, 52);
    SynchronizerTester::checkFrame(tester.frame2, 52);
    SynchronizerTester::checkMatrix(tester.matrix1, 52);

    //time 8: synch
    tester.addFrameToFrameTL(tester.frameTL1, 55);
    tester.addFrameToFrameTL(tester.frameTL2, 55);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 55);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 55);
    SynchronizerTester::checkFrame(tester.frame1, 55);
    SynchronizerTester::checkFrame(tester.frame2, 55);
    SynchronizerTester::checkMatrix(tester.matrix1, 55);
}

//------------------------------------------------------------------------------

/// Classical synchronisation with resets
// FrameTL1  <= f1_1 <= f1_2 <= f1_6           <= f1_8 <= f1_9 <= f1_10 <= f1_11
// FrameTL2  <= f2_1 <= f2_2 <= f2_6   reset   <= f2_8                  <= m1_11
// MatrixTL1 <= m1_1 <= m1_2 <= m1_6                   <= m1_9
//
// at each FrameTL1 a synchronisation is done.
// expected result:
// ref tsp  :  1   ,  2   ,  6   ,  6   ,   8  ,   9
//  Frame 1 : f1_1 , f1_2 , f1_6 , f1_6 , f1_8 , f1_9
//  Frame 2 : f2_1 , f2_2 , f2_6 , f2_6 , f2_8 , f2_8
// Matrix 1 : m1_1 , m1_2 , m1_6 , m1_6 , m1_9 , m1_9
void synchronizer_test::resetTimelineSynchronisation()
{
    SynchronizerTester tester;
    core::hires_clock::type lastTimestampSynch = 0;
    bool synchronizationSkippedReceived        = false;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);
    auto slotSynchronizationSkippedReceived = sight::core::com::new_slot(
        [&synchronizationSkippedReceived]()
        {
            synchronizationSkippedReceived = true;
        });
    slotSynchronizationSkippedReceived->set_worker(sight::core::thread::get_default_worker());
    auto synchSkippedConnection = tester.srv->signal("synchronizationSkipped")->connect(
        slotSynchronizationSkippedReceived
    );

    // This is done just to handle automatic synch at first data push
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.srv->slot("reset")->run();

    //time 1: synch
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    SynchronizerTester::checkFrame(tester.frame1, 1);
    SynchronizerTester::checkFrame(tester.frame2, 1);
    SynchronizerTester::checkMatrix(tester.matrix1, 1);

    //time 2: synch
    tester.addFrameToFrameTL(tester.frameTL1, 2);
    tester.addFrameToFrameTL(tester.frameTL2, 2);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 2);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);
    SynchronizerTester::checkFrame(tester.frame1, 2);
    SynchronizerTester::checkFrame(tester.frame2, 2);
    SynchronizerTester::checkMatrix(tester.matrix1, 2);

    //time 3: synch
    tester.addFrameToFrameTL(tester.frameTL1, 6);
    tester.addFrameToFrameTL(tester.frameTL2, 6);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 6);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 6);
    lastTimestampSynch = 0;
    SynchronizerTester::checkFrame(tester.frame1, 6);
    SynchronizerTester::checkFrame(tester.frame2, 6);
    SynchronizerTester::checkMatrix(tester.matrix1, 6);

    tester.srv->slot("reset")->run();

    //time 4: synch, with timestamp  min(6,8,8)=6
    // it is not the same as the previous one because of the reset
    tester.addFrameToFrameTL(tester.frameTL1, 8);
    tester.addFrameToFrameTL(tester.frameTL2, 8);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 6);
    SynchronizerTester::checkFrame(tester.frame1, 6);
    SynchronizerTester::checkFrame(tester.frame2, 6);
    SynchronizerTester::checkMatrix(tester.matrix1, 6);

    //time 5: synch, with timestamp  min(9,8,9)=8 => take frame/matrix closest to 8
    tester.addFrameToFrameTL(tester.frameTL1, 9);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 9);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 8);
    SynchronizerTester::checkFrame(tester.frame1, 8);
    SynchronizerTester::checkFrame(tester.frame2, 8);
    SynchronizerTester::checkMatrix(tester.matrix1, 9);

    //time 6: no synch, timestamp  min(10,8,9)= 8 = last_timestamp
    tester.addFrameToFrameTL(tester.frameTL1, 10);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame1, 8);
    SynchronizerTester::checkFrame(tester.frame2, 8);
    SynchronizerTester::checkMatrix(tester.matrix1, 9);

    //time 7: sync, with, timestamp  min(11,11,9)= 9 => take frame/matrix closest to 9
    tester.addFrameToFrameTL(tester.frameTL1, 11);
    tester.addFrameToFrameTL(tester.frameTL2, 11);
    fwTestWithFailWaitMacro(lastTimestampSynch == 9);
    SynchronizerTester::checkFrame(tester.frame1, 9);
    SynchronizerTester::checkFrame(tester.frame2, 8);
    SynchronizerTester::checkMatrix(tester.matrix1, 9);
}

//------------------------------------------------------------------------------

/// Classical synchronisation with resets and loops
// FrameTL1  <= f1_1 <= f1_3 <= f1_1 <= f1_6 <= f1_8        <= f1_1 <= f1_2
// FrameTL2  <= f2_1 <= f2_3 <= f2_1 <= f2_6 <= f2_8  reset <= f2_1 <= f2_2
// MatrixTL1 <= m1_1 <= m1_3 <= m1_1 <= m1_6                <= m1_1 <= m1_2
//
// at each FrameTL1 a synchronisation is done.
// expected result:
// ref tsp  :  1   ,  3   ,  6   ,  8   ,  1   ,  2
//  Frame 1 : f1_1 , f1_3 , f1_6 , f1_8 , f1_1 , f1_2
//  Frame 2 : f2_1 , f2_3 , f2_8 , f2_8 , f2_1 , f2_2
// Matrix 1 : m1_1 , m1_3 , m1_6 , m1_9 , m1_1 , m1_2
void synchronizer_test::resetAndLoopSynchronisation()
{
    SynchronizerTester tester;
    core::hires_clock::type lastTimestampSynch = 0;
    bool synchronizationSkippedReceived        = false;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);
    auto slotSynchronizationSkippedReceived = sight::core::com::new_slot(
        [&synchronizationSkippedReceived]()
        {
            synchronizationSkippedReceived = true;
        });
    slotSynchronizationSkippedReceived->set_worker(sight::core::thread::get_default_worker());
    auto synchSkippedConnection = tester.srv->signal("synchronizationSkipped")->connect(
        slotSynchronizationSkippedReceived
    );

    // This is done just to handle automatic synch at first data push
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.srv->slot("reset")->run();

    //time 1: synch
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    SynchronizerTester::checkFrame(tester.frame1, 1);
    SynchronizerTester::checkFrame(tester.frame2, 1);
    SynchronizerTester::checkMatrix(tester.matrix1, 1);

    //time 2: synch
    tester.addFrameToFrameTL(tester.frameTL1, 3);
    tester.addFrameToFrameTL(tester.frameTL2, 3);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 3);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 3);
    SynchronizerTester::checkFrame(tester.frame1, 3);
    SynchronizerTester::checkFrame(tester.frame2, 3);
    SynchronizerTester::checkMatrix(tester.matrix1, 3);

    //time 3: no synch, new timestamp is in the past
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame1, 3);
    SynchronizerTester::checkFrame(tester.frame2, 3);
    SynchronizerTester::checkMatrix(tester.matrix1, 3);

    //time 3: synch
    tester.addFrameToFrameTL(tester.frameTL1, 6);
    tester.addFrameToFrameTL(tester.frameTL2, 6);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 6);
    fwTestWithFailWaitMacro(lastTimestampSynch == 6);
    SynchronizerTester::checkFrame(tester.frame1, 6);
    SynchronizerTester::checkFrame(tester.frame2, 6);
    SynchronizerTester::checkMatrix(tester.matrix1, 6);

    //time 4: no synch, timestamp  min(8,8,6)= 6 = last_timestamp
    tester.addFrameToFrameTL(tester.frameTL1, 8);
    tester.addFrameToFrameTL(tester.frameTL2, 8);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    SynchronizerTester::checkFrame(tester.frame1, 6);
    SynchronizerTester::checkFrame(tester.frame2, 6);
    SynchronizerTester::checkMatrix(tester.matrix1, 6);

    //time 5: synch, with timestamp  min(9,8,9)=8 => take frame/matrix closest to 8
    tester.addFrameToFrameTL(tester.frameTL1, 9);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 9);
    fwTestWithFailWaitMacro(lastTimestampSynch == 8);
    SynchronizerTester::checkFrame(tester.frame1, 8);
    SynchronizerTester::checkFrame(tester.frame2, 8);
    SynchronizerTester::checkMatrix(tester.matrix1, 9);

    tester.frameTL1->clearTimeline();
    tester.frameTL2->clearTimeline();
    tester.matrixTL1->clearTimeline();
    tester.srv->slot("reset")->run();

    //time 6: sync without anything in timeline:
    tester.srv->update().wait();

    //time 7: sync
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    SynchronizerTester::checkFrame(tester.frame1, 1);
    SynchronizerTester::checkFrame(tester.frame2, 1);
    SynchronizerTester::checkMatrix(tester.matrix1, 1);

    //time 8: synch
    tester.addFrameToFrameTL(tester.frameTL1, 2);
    tester.addFrameToFrameTL(tester.frameTL2, 2);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 2);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);
    SynchronizerTester::checkFrame(tester.frame1, 2);
    SynchronizerTester::checkFrame(tester.frame2, 2);
    SynchronizerTester::checkMatrix(tester.matrix1, 2);
}

//------------------------------------------------------------------------------

//<service type="sight::module::sync::synchronizer" auto_connect="true">
//     <in group="matrixTL">
//         <key uid="matrixTL1" />
//     </in>
//     <inout group="matrices">
//         <key uid="matrix0" tl="0" index="1" sendStatus="true" >
//         <key uid="matrix1" tl="0" index="0" />
//     </inout>
//</service>
void synchronizer_test::singleMatrixTLConfigTest()
{
    std::stringstream config_string;
    config_string
    << "<in group=\"matrixTL\">"
       "   <key uid=\"matrixTL1\" />"
       "</in>"
       "<inout group=\"matrix\">"
       "   <key uid=\"matrix0\" tl=\"0\" index=\"1\" sendStatus=\"true\" />"
       "   <key uid=\"matrix1\" tl=\"0\" index=\"0\" />"
       "</inout>"
       "<legacyAutoSync>true</legacyAutoSync>";

    SynchronizerTester tester(config_string);

    // create input TLs
    data::matrix_tl::sptr matrixTL1 = std::make_shared<data::matrix_tl>();
    matrixTL1->initPoolSize(4);
    tester.srv->set_input(matrixTL1, "matrixTL", true, false, 0);

    // create output vars
    auto matrix0 = std::make_shared<data::matrix4>();
    auto matrix1 = std::make_shared<data::matrix4>();
    (*matrix0)(0, 0) = 0.; // init the first value a 0. This will be filled with the timestamp in the tests.
    (*matrix1)(0, 0) = 0.; // init the first value a 0. This will be filled with the timestamp in the tests.
    tester.srv->set_inout(matrix0, "matrix", false, false, 0);
    tester.srv->set_inout(matrix1, "matrix", false, false, 1);

    //start
    tester.srv->start().wait();
    tester.srv->update().wait();

    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 1);
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 2);
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);
    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 4);
    fwTestWithFailWaitMacro(lastTimestampSynch == 4);
    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 6);
    fwTestWithFailWaitMacro(lastTimestampSynch == 6);

    //The value in the matrix is  timestamp*10 + element_Index
    SynchronizerTester::checkMatrix(matrix0, 61);
    SynchronizerTester::checkMatrix(matrix1, 60);

    tester.srv->stop().wait();
}

//------------------------------------------------------------------------------

void synchronizer_test::mixtMatrixTLConfigTest()
{
    std::stringstream config_string;
    config_string
    << "<in group=\"matrixTL\">"
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

    SynchronizerTester tester(config_string);

    // create input TLs
    data::matrix_tl::sptr matrixTL1 = std::make_shared<data::matrix_tl>();
    data::matrix_tl::sptr matrixTL2 = std::make_shared<data::matrix_tl>();
    matrixTL1->initPoolSize(4);
    matrixTL2->initPoolSize(4);
    tester.srv->set_input(matrixTL1, "matrixTL", true, false, 0);
    tester.srv->set_input(matrixTL2, "matrixTL", true, false, 1);

    // create output vars
    auto matrix0 = std::make_shared<data::matrix4>();
    auto matrix1 = std::make_shared<data::matrix4>();
    auto matrix2 = std::make_shared<data::matrix4>();
    auto matrix3 = std::make_shared<data::matrix4>();
    auto matrix4 = std::make_shared<data::matrix4>();
    (*matrix0)(0, 0) = 0.;
    (*matrix1)(0, 0) = 0.;
    (*matrix2)(0, 0) = 0.;
    (*matrix3)(0, 0) = 0.;
    (*matrix4)(0, 0) = 0.;
    tester.srv->set_inout(matrix0, "matrix", false, false, 0);
    tester.srv->set_inout(matrix1, "matrix", false, false, 1);
    tester.srv->set_inout(matrix2, "matrix", false, false, 2);
    tester.srv->set_inout(matrix3, "matrix", false, false, 3);
    tester.srv->set_inout(matrix4, "matrix", false, false, 4);
    //start
    tester.srv->start().wait();
    tester.srv->update().wait();

    core::hires_clock::type lastTimestampSynch = 0;
    bool synchronizationSkippedReceived        = false;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);
    auto slotSynchronizationSkippedReceived = sight::core::com::new_slot(
        [&synchronizationSkippedReceived]()
        {
            synchronizationSkippedReceived = true;
        });
    slotSynchronizationSkippedReceived->set_worker(sight::core::thread::get_default_worker());
    auto synchSkippedConnection = tester.srv->signal("synchronizationSkipped")->connect(
        slotSynchronizationSkippedReceived
    );

    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 1);
    tester.addMatrixToMatrixTL(matrixTL2, {0, 1, 2}, 1);
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 3);
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;
    tester.addMatrixToMatrixTL(matrixTL2, {0, 1, 2}, 2);
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);

    //The value in the matrix is  timestamp*10 + element_Index
    SynchronizerTester::checkMatrix(matrix0, 31);
    SynchronizerTester::checkMatrix(matrix1, 30);
    SynchronizerTester::checkMatrix(matrix2, 20);
    SynchronizerTester::checkMatrix(matrix3, 21);
    SynchronizerTester::checkMatrix(matrix4, 32);

    tester.srv->stop().wait();
}

//------------------------------------------------------------------------------

void synchronizer_test::singleFrameTLConfigTest()
{
    std::stringstream config_string;
    config_string
    << "<in group=\"frameTL\">"
       "    <key uid=\"frameTL\" />"
       "</in>"
       "<inout group=\"frames\">"
       "    <key uid=\"frame\" sendStatus=\"true\" />"
       "</inout>"
       "<legacyAutoSync>true</legacyAutoSync>";

    SynchronizerTester tester(config_string);

    // create input TL
    auto frameTL = std::make_shared<data::frame_tl>();
    frameTL->initPoolSize(
        tester.frameSize[0],
        tester.frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    tester.srv->set_input(frameTL, "frameTL", true, false, 0);

    // create output vars
    auto frame = std::make_shared<data::image>();
    frame->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame = frame->dump_lock();
        std::fill(frame->begin<std::uint8_t>(), frame->end<std::uint8_t>(), std::uint8_t(0));
    }

    tester.srv->set_inout(frame, "frames", false, false, 0);

    tester.srv->start().wait();
    tester.srv->update().wait();

    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    tester.addFrameToFrameTL(frameTL, 1);
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    SynchronizerTester::checkFrame(frame, 1);

    tester.addFrameToFrameTL(frameTL, 2);
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);
    SynchronizerTester::checkFrame(frame, 2);

    tester.addFrameToFrameTL(frameTL, 4);
    tester.addFrameToFrameTL(frameTL, 6);
    fwTestWithFailWaitMacro(lastTimestampSynch == 6);
    SynchronizerTester::checkFrame(frame, 6);

    tester.srv->stop().wait();
}

//------------------------------------------------------------------------------

void synchronizer_test::mixtFrameTLConfigTest()
{
    std::stringstream config_string;
    config_string
    << "<in group=\"frameTL\">"
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

    SynchronizerTester tester(config_string);

    // create input TL
    auto frameTL1 = std::make_shared<data::frame_tl>();
    frameTL1->initPoolSize(
        tester.frameSize[0],
        tester.frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    auto frameTL4 = std::make_shared<data::frame_tl>();
    frameTL4->initPoolSize(
        tester.frameSize[0],
        tester.frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    auto frameTL6 = std::make_shared<data::frame_tl>();
    frameTL6->initPoolSize(
        tester.frameSize[0],
        tester.frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    tester.srv->set_input(frameTL1, "frameTL", true, false, 0);
    tester.srv->set_input(frameTL4, "frameTL", true, false, 1);
    tester.srv->set_input(frameTL6, "frameTL", true, false, 2);

    // create output vars
    auto frame1 = std::make_shared<data::image>();
    frame1->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame1 = frame1->dump_lock();
        std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
    }
    auto frame4 = std::make_shared<data::image>();
    frame4->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame4 = frame4->dump_lock();
        std::fill(frame4->begin<std::uint8_t>(), frame4->end<std::uint8_t>(), std::uint8_t(0));
    }
    auto frame6 = std::make_shared<data::image>();
    frame6->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame6 = frame6->dump_lock();
        std::fill(frame6->begin<std::uint8_t>(), frame6->end<std::uint8_t>(), std::uint8_t(0));
    }
    auto frame11 = std::make_shared<data::image>();
    frame11->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame11 = frame11->dump_lock();
        std::fill(frame11->begin<std::uint8_t>(), frame11->end<std::uint8_t>(), std::uint8_t(0));
    }

    tester.srv->set_inout(frame1, "frames", false, false, 0);
    tester.srv->set_inout(frame6, "frames", false, false, 1);
    tester.srv->set_inout(frame4, "frames", false, false, 2);
    tester.srv->set_inout(frame11, "frames", false, false, 3);

    tester.srv->start().wait();
    tester.srv->update().wait();

    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    tester.addFrameToFrameTL(frameTL1, 1);
    tester.addFrameToFrameTL(frameTL4, 1);
    tester.addFrameToFrameTL(frameTL6, 1);
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);

    tester.addFrameToFrameTL(frameTL1, 4);
    tester.addFrameToFrameTL(frameTL4, 5);
    tester.addFrameToFrameTL(frameTL6, 6);
    fwTestWithFailWaitMacro(lastTimestampSynch == 4);

    SynchronizerTester::checkFrame(frame1, 4);
    SynchronizerTester::checkFrame(frame4, 5);
    SynchronizerTester::checkFrame(frame6, 6);
    SynchronizerTester::checkFrame(frame11, 4);

    tester.srv->stop().wait();
}

//------------------------------------------------------------------------------

void synchronizer_test::fullConfigTest()
{
    std::stringstream config_string;
    config_string
    << "<in group=\"frameTL\">"
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
       "<in group=\"matrixTL\">"
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

    SynchronizerTester tester(config_string);
    auto frameTL1 = std::make_shared<data::frame_tl>();
    frameTL1->initPoolSize(
        tester.frameSize[0],
        tester.frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    auto frameTL4 = std::make_shared<data::frame_tl>();
    frameTL4->initPoolSize(
        tester.frameSize[0],
        tester.frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    auto frameTL6 = std::make_shared<data::frame_tl>();
    frameTL6->initPoolSize(
        tester.frameSize[0],
        tester.frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    tester.srv->set_input(frameTL1, "frameTL", true, false, 0);
    tester.srv->set_input(frameTL4, "frameTL", true, false, 1);
    tester.srv->set_input(frameTL6, "frameTL", true, false, 2);

    data::matrix_tl::sptr matrixTL1 = std::make_shared<data::matrix_tl>();
    data::matrix_tl::sptr matrixTL2 = std::make_shared<data::matrix_tl>();
    matrixTL1->initPoolSize(4);
    matrixTL2->initPoolSize(4);
    tester.srv->set_input(matrixTL1, "matrixTL", true, false, 0);
    tester.srv->set_input(matrixTL2, "matrixTL", true, false, 1);

    // create output vars
    auto frame1 = std::make_shared<data::image>();
    frame1->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame1 = frame1->dump_lock();
        std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
    }
    auto frame4 = std::make_shared<data::image>();
    frame4->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame4 = frame4->dump_lock();
        std::fill(frame4->begin<std::uint8_t>(), frame4->end<std::uint8_t>(), std::uint8_t(0));
    }
    auto frame6 = std::make_shared<data::image>();
    frame6->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame6 = frame6->dump_lock();
        std::fill(frame6->begin<std::uint8_t>(), frame6->end<std::uint8_t>(), std::uint8_t(0));
    }
    auto frame11 = std::make_shared<data::image>();
    frame11->resize(
        tester.frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame11 = frame11->dump_lock();
        std::fill(frame11->begin<std::uint8_t>(), frame11->end<std::uint8_t>(), std::uint8_t(0));
    }

    tester.srv->set_inout(frame1, "frames", false, false, 0);
    tester.srv->set_inout(frame6, "frames", false, false, 1);
    tester.srv->set_inout(frame4, "frames", false, false, 2);
    tester.srv->set_inout(frame11, "frames", false, false, 3);

    auto matrix0 = std::make_shared<data::matrix4>();
    auto matrix1 = std::make_shared<data::matrix4>();
    auto matrix2 = std::make_shared<data::matrix4>();
    auto matrix3 = std::make_shared<data::matrix4>();
    auto matrix4 = std::make_shared<data::matrix4>();
    (*matrix0)(0, 0) = 0.;
    (*matrix1)(0, 0) = 0.;
    (*matrix2)(0, 0) = 0.;
    (*matrix3)(0, 0) = 0.;
    (*matrix4)(0, 0) = 0.;
    tester.srv->set_inout(matrix0, "matrix", false, false, 0);
    tester.srv->set_inout(matrix1, "matrix", false, false, 1);
    tester.srv->set_inout(matrix2, "matrix", false, false, 2);
    tester.srv->set_inout(matrix3, "matrix", false, false, 3);
    tester.srv->set_inout(matrix4, "matrix", false, false, 4);

    tester.srv->start().wait();
    tester.srv->update().wait();

    core::hires_clock::type lastTimestampSynch = 0;
    bool synchronizationSkippedReceived        = false;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);
    auto slotSynchronizationSkippedReceived = sight::core::com::new_slot(
        [&synchronizationSkippedReceived]()
        {
            synchronizationSkippedReceived = true;
        });
    slotSynchronizationSkippedReceived->set_worker(sight::core::thread::get_default_worker());
    auto synchSkippedConnection = tester.srv->signal("synchronizationSkipped")->connect(
        slotSynchronizationSkippedReceived
    );

    tester.addFrameToFrameTL(frameTL1, 1);
    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 1);
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);

    tester.addFrameToFrameTL(frameTL1, 2);
    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 2);
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);

    SynchronizerTester::checkFrame(frame1, 2);
    SynchronizerTester::checkFrame(frame4, 0);
    SynchronizerTester::checkFrame(frame6, 0);
    SynchronizerTester::checkFrame(frame11, 2);
    SynchronizerTester::checkMatrix(matrix0, 21);
    SynchronizerTester::checkMatrix(matrix1, 20);
    SynchronizerTester::checkMatrix(matrix2, 0);
    SynchronizerTester::checkMatrix(matrix3, 0);
    SynchronizerTester::checkMatrix(matrix4, 22);

    // populate the remaining TLs.
    // no synch as timestamp is min (2, 3, 4) = 2 == last_timestamp
    tester.addFrameToFrameTL(frameTL4, 3);
    tester.addFrameToFrameTL(frameTL6, 4);
    tester.addMatrixToMatrixTL(matrixTL2, {0, 1, 2}, 4);
    fwTestWithFailWaitMacro(synchronizationSkippedReceived == true);
    synchronizationSkippedReceived = false;

    SynchronizerTester::checkFrame(frame1, 2);
    SynchronizerTester::checkFrame(frame4, 0);
    SynchronizerTester::checkFrame(frame6, 0);
    SynchronizerTester::checkFrame(frame11, 2);
    SynchronizerTester::checkMatrix(matrix0, 21);
    SynchronizerTester::checkMatrix(matrix1, 20);
    SynchronizerTester::checkMatrix(matrix2, 0);
    SynchronizerTester::checkMatrix(matrix3, 0);
    SynchronizerTester::checkMatrix(matrix4, 22);

    tester.addFrameToFrameTL(frameTL1, 5);
    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 5);
    fwTestWithFailWaitMacro(lastTimestampSynch == 3);

    SynchronizerTester::checkFrame(frame1, 2);
    SynchronizerTester::checkFrame(frame4, 3);
    SynchronizerTester::checkFrame(frame6, 4);
    SynchronizerTester::checkFrame(frame11, 2);
    SynchronizerTester::checkMatrix(matrix0, 21);
    SynchronizerTester::checkMatrix(matrix1, 20);
    SynchronizerTester::checkMatrix(matrix2, 40);
    SynchronizerTester::checkMatrix(matrix3, 41);
    SynchronizerTester::checkMatrix(matrix4, 22);

    tester.addFrameToFrameTL(frameTL1, 6);
    tester.addFrameToFrameTL(frameTL4, 6);
    tester.addFrameToFrameTL(frameTL6, 6);
    tester.addMatrixToMatrixTL(matrixTL1, {0, 1, 2}, 6);
    tester.addMatrixToMatrixTL(matrixTL2, {0, 1, 2}, 6);
    fwTestWithFailWaitMacro(lastTimestampSynch == 6);

    SynchronizerTester::checkFrame(frame1, 6);
    SynchronizerTester::checkFrame(frame4, 6);
    SynchronizerTester::checkFrame(frame6, 6);
    SynchronizerTester::checkFrame(frame11, 6);
    SynchronizerTester::checkMatrix(matrix0, 61);
    SynchronizerTester::checkMatrix(matrix1, 60);
    SynchronizerTester::checkMatrix(matrix2, 60);
    SynchronizerTester::checkMatrix(matrix3, 61);
    SynchronizerTester::checkMatrix(matrix4, 62);

    tester.srv->stop().wait();
}

//------------------------------------------------------------------------------

void synchronizer_test::updateConfigTest()
{
    SynchronizerTester tester;
    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    // This is done just to handle automatic synch at first data push
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.srv->slot("reset")->run();

    //time 1: synch
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    SynchronizerTester::checkFrame(tester.frame1, 1);
    SynchronizerTester::checkFrame(tester.frame2, 1);
    SynchronizerTester::checkMatrix(tester.matrix1, 1);

    //time 2: synch
    tester.addFrameToFrameTL(tester.frameTL1, 4);
    tester.addFrameToFrameTL(tester.frameTL2, 5);
    tester.addMatrixToMatrixTL(tester.matrixTL1, {0, 1, 2}, 6);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 4);
    SynchronizerTester::checkFrame(tester.frame1, 4);
    SynchronizerTester::checkFrame(tester.frame2, 5);
    SynchronizerTester::checkMatrix(tester.matrix1, 60);
    tester.srv->slot("setFrameBinding")->run(std::size_t(1), 0U, std::size_t(0));
    tester.srv->slot("setMatrixBinding")->run(std::size_t(0), 1U, std::size_t(0));

    //time 3: synch with frame TL 2 on both frame outputs and matrix on second element
    tester.addFrameToFrameTL(tester.frameTL1, 10);
    tester.addFrameToFrameTL(tester.frameTL2, 11);
    tester.addMatrixToMatrixTL(tester.matrixTL1, {0, 1, 2}, 12);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 10);
    SynchronizerTester::checkFrame(tester.frame1, 11);
    SynchronizerTester::checkFrame(tester.frame2, 11);
    SynchronizerTester::checkMatrix(tester.matrix1, 121);

    tester.srv->slot("setFrameBinding")->run(std::size_t(0), 0U, std::size_t(1));

    //time 4: synch with frame TL1 => frame2 and frameTL2 => frame1
    tester.addFrameToFrameTL(tester.frameTL1, 20);
    tester.addFrameToFrameTL(tester.frameTL2, 21);
    tester.addMatrixToMatrixTL(tester.matrixTL1, {0, 1, 2}, 22);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 20);
    SynchronizerTester::checkFrame(tester.frame1, 21);
    SynchronizerTester::checkFrame(tester.frame2, 20);
    SynchronizerTester::checkMatrix(tester.matrix1, 221);
}

//------------------------------------------------------------------------------

/// Take the classical configuration, and add send status to Frame 1 and Matrix 1
// FrameTL1  <= f1_1 <= f1_2  <= f1_50 <= f1_51 <= f1_52 <= f1_53  <= f1_101
// FrameTL2  <= f2_1 <= f2_2  <= f2_50 <= f2_51 <= f2_52 <= f2_100 <= f2_101
// MatrixTL1 <= m1_1 <= m1_2  <= m1_3  <= m1_51 <= m1_52 <= m1_100 <= m1_101
//
// at each FrameTL1 a synchronisation is done.
// expected signal:
// ref tsp  :  1   ,  2   ,  50   ,  51   ,   52 ,  100 , 101
//  Frame 1 : sync , none , none  , none  , none ,  out , sync
//  Frame 2 : none , none , none  , none  , none , none , none
// Matrix 1 : sync , none ,  out  , sync  , none , none , none
void synchronizer_test::sendStatusTest()
{
    std::stringstream config_string;
    config_string
    << "<in group=\"frameTL\">"
       "    <key uid=\"frameTL1\"  />"
       "    <key uid=\"frameTL2\" />"
       "</in>"
       "<inout group=\"frames\">"
       "    <key uid=\"frame1\" sendStatus=\"true\" />"
       "    <key uid=\"frame2\" tl=\"1\" />"
       "</inout>"
       "<in group=\"matrixTL\">"
       "    <key uid=\"matrixTL\" />"
       "</in>"
       "<inout group=\"matrix\">"
       "    <key uid=\"matrix0\" sendStatus=\"true\" />"
       "</inout>"
       "<tolerance>5</tolerance>";

    SynchronizerTester tester(config_string);
    tester.initStandardInOut();
    tester.srv->start().wait();
    tester.srv->update().wait();

    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    std::vector<bool> frameVarIndexSynchSignal = {false, false};
    auto slotFrameSynchronized                 =
        sight::core::com::new_slot(
            [&frameVarIndexSynchSignal](int _frameVarIndex)
        {
            frameVarIndexSynchSignal[static_cast<std::size_t>(_frameVarIndex)] = true;
        });
    slotFrameSynchronized->set_worker(sight::core::thread::get_default_worker());
    auto frameSynchronizedConnection =
        tester.srv->signal("frameSynchronized")->connect(slotFrameSynchronized);

    std::vector<bool> frameVarIndexUnSynchSignal = {false, false};
    auto slotFrameUnsynchronized                 =
        sight::core::com::new_slot(
            [&frameVarIndexUnSynchSignal](int _frameVarIndex)
        {
            frameVarIndexUnSynchSignal[static_cast<std::size_t>(_frameVarIndex)] = true;
        });
    slotFrameUnsynchronized->set_worker(sight::core::thread::get_default_worker());
    auto frameUnsynchronizedConnection =
        tester.srv->signal("frameUnsynchronized")->connect(slotFrameUnsynchronized);

    int matrixVarIndexSynchSignal = -1;
    auto slotMatrixSynchronized   =
        sight::core::com::new_slot(
            [&matrixVarIndexSynchSignal](int _matrixVarIndex)
        {
            matrixVarIndexSynchSignal = _matrixVarIndex;
        });
    slotMatrixSynchronized->set_worker(sight::core::thread::get_default_worker());
    auto matrixSynchronizedConnection =
        tester.srv->signal("matrixSynchronized")->connect(slotMatrixSynchronized);

    int matrixVarIndexUnSynchSignal = -1;
    auto slotMatrixUnsynchronized   =
        sight::core::com::new_slot(
            [&matrixVarIndexUnSynchSignal](int _matrixVarIndex)
        {
            matrixVarIndexUnSynchSignal = _matrixVarIndex;
        });
    slotMatrixUnsynchronized->set_worker(sight::core::thread::get_default_worker());
    auto matrixUnsynchronizedConnection =
        tester.srv->signal("matrixUnsynchronized")->connect(slotMatrixUnsynchronized);

    // This is done just to handle automatic synch at first data push
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.srv->slot("reset")->run();

    //time 1: synch
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    fwTestWithFailWaitMacro(matrixVarIndexSynchSignal == 0);
    fwTestWithFailWaitMacro(frameVarIndexSynchSignal[0]);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[1] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[1] == false);
    CPPUNIT_ASSERT(matrixVarIndexUnSynchSignal == -1);
    frameVarIndexSynchSignal[0] = false;
    matrixVarIndexSynchSignal   = -1;

    //time 2
    tester.addFrameToFrameTL(tester.frameTL1, 2);
    tester.addFrameToFrameTL(tester.frameTL2, 2);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 2);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[1] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[1] == false);
    CPPUNIT_ASSERT(matrixVarIndexSynchSignal == -1);
    CPPUNIT_ASSERT(matrixVarIndexUnSynchSignal == -1);

    //time 3
    tester.addFrameToFrameTL(tester.frameTL1, 50);
    tester.addFrameToFrameTL(tester.frameTL2, 50);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 3);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 50);
    fwTestWithFailWaitMacro(matrixVarIndexUnSynchSignal == 0);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[1] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[1] == false);
    CPPUNIT_ASSERT(matrixVarIndexSynchSignal == -1);
    CPPUNIT_ASSERT(matrixVarIndexUnSynchSignal == 0);
    matrixVarIndexUnSynchSignal = -1;

    //time 4
    tester.addFrameToFrameTL(tester.frameTL1, 51);
    tester.addFrameToFrameTL(tester.frameTL2, 51);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 51);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 51);
    fwTestWithFailWaitMacro(matrixVarIndexSynchSignal == 0);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[1] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[1] == false);
    CPPUNIT_ASSERT(matrixVarIndexSynchSignal == 0);
    CPPUNIT_ASSERT(matrixVarIndexUnSynchSignal == -1);
    matrixVarIndexSynchSignal = -1;

    //time 5
    tester.addFrameToFrameTL(tester.frameTL1, 53);
    tester.addFrameToFrameTL(tester.frameTL2, 100);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 100);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 100);
    fwTestWithFailWaitMacro(frameVarIndexUnSynchSignal[0]);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[1] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[1] == false);
    CPPUNIT_ASSERT(matrixVarIndexSynchSignal == -1);
    CPPUNIT_ASSERT(matrixVarIndexUnSynchSignal == -1);
    frameVarIndexUnSynchSignal[0] = false;

    //time 6
    tester.addFrameToFrameTL(tester.frameTL1, 101);
    tester.addFrameToFrameTL(tester.frameTL2, 101);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 101);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 101);
    fwTestWithFailWaitMacro(frameVarIndexSynchSignal[0]);
    CPPUNIT_ASSERT(frameVarIndexSynchSignal[1] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[0] == false);
    CPPUNIT_ASSERT(frameVarIndexUnSynchSignal[1] == false);
    CPPUNIT_ASSERT(matrixVarIndexSynchSignal == -1);
    CPPUNIT_ASSERT(matrixVarIndexUnSynchSignal == -1);
    frameVarIndexSynchSignal[0] = false;
}

//------------------------------------------------------------------------------

void synchronizer_test::delayTest()
{
    std::stringstream config_string;
    config_string
    << "<in group=\"frameTL\">"
       "    <key uid=\"frameTL1\" delay=\"2\" />"
       "    <key uid=\"frameTL2\" />"
       "</in>"
       "<inout group=\"frames\">"
       "    <key uid=\"frame1\" />"
       "    <key uid=\"frame2\" tl=\"1\" />"
       "</inout>"
       "<in group=\"matrixTL\">"
       "    <key uid=\"matrixTL\" delay=\"3\"/>"
       "</in>"
       "<inout group=\"matrix\">"
       "    <key uid=\"matrix0\" />"
       "</inout>"
       "<tolerance>5</tolerance>"
       "<legacyAutoSync>true</legacyAutoSync>";

    SynchronizerTester tester(config_string);
    tester.initStandardInOut();
    tester.srv->start().wait();
    tester.srv->update().wait();

    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    //time 1: synch on timestamp 4, expected (2,4,1)
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.addFrameToFrameTL(tester.frameTL1, 2);
    tester.addFrameToFrameTL(tester.frameTL2, 2);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 2);
    tester.addFrameToFrameTL(tester.frameTL1, 3);
    tester.addFrameToFrameTL(tester.frameTL2, 3);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 3);
    tester.addFrameToFrameTL(tester.frameTL1, 4);
    tester.addFrameToFrameTL(tester.frameTL2, 4);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 4);
    fwTestWithFailWaitMacro(lastTimestampSynch == 4);
    SynchronizerTester::checkFrame(tester.frame1, 2);
    SynchronizerTester::checkFrame(tester.frame2, 4);
    SynchronizerTester::checkMatrix(tester.matrix1, 1);

    //time 2: synch on timestamp 5, expected (3,5,2)
    tester.addFrameToFrameTL(tester.frameTL1, 5);
    tester.addFrameToFrameTL(tester.frameTL2, 5);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 5);
    fwTestWithFailWaitMacro(lastTimestampSynch == 5);
    SynchronizerTester::checkFrame(tester.frame1, 3);
    SynchronizerTester::checkFrame(tester.frame2, 5);
    SynchronizerTester::checkMatrix(tester.matrix1, 2);

    tester.srv->slot("setDelay")->run(1, std::string("frameDelay_0"));
    tester.srv->slot("setDelay")->run(-1, std::string("frameDelay_1"));
    tester.srv->slot("setDelay")->run(-1, std::string("frameDelay_2"));
    tester.srv->slot("setDelay")->run(-1, std::string("matrixDelay_0"));
    tester.srv->slot("setDelay")->run(2, std::string("matrixDelay_0"));
    tester.srv->slot("setDelay")->run(1, std::string("matrixDelay_1"));

    //time 3: synch on timestamp 6, expected (5,6,4)
    tester.addFrameToFrameTL(tester.frameTL1, 6);
    tester.addFrameToFrameTL(tester.frameTL2, 6);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 6);
    fwTestWithFailWaitMacro(lastTimestampSynch == 6);
    SynchronizerTester::checkFrame(tester.frame1, 5);
    SynchronizerTester::checkFrame(tester.frame2, 6);
    SynchronizerTester::checkMatrix(tester.matrix1, 4);

    tester.srv->slot("setDelay")->run(0, std::string("frameDelay_0"));
    tester.srv->slot("setDelay")->run(0, std::string("matrixDelay_0"));

    //time 4: synch on timestamp 7, expected (7,7,7)
    tester.addFrameToFrameTL(tester.frameTL1, 7);
    tester.addFrameToFrameTL(tester.frameTL2, 7);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 7);
    fwTestWithFailWaitMacro(lastTimestampSynch == 7);
    SynchronizerTester::checkFrame(tester.frame1, 7);
    SynchronizerTester::checkFrame(tester.frame2, 7);
    SynchronizerTester::checkMatrix(tester.matrix1, 7);
}

//------------------------------------------------------------------------------

/// The classical tolerance is 5. Make one TL or the other out of range
// FrameTL1  <= f1_1 <= f1_5 <= f1_50 <= f1_53 <= f1_100 <= f1_110
// FrameTL2  <= f2_1 <= f2_5          <= f2_54 <= f2_100 <= f2_110
// MatrixTL1 <= m1_1 <= m1_4 <= m1_50 <= m1_53 <= m1_55  <= m1_111
//
// expected result:
// ref tsp  :  1   ,  4   ,  50   ,  53   ,   100  ,   110
//  Frame 1 : f1_1 , f1_5 , f1_50 , f1_53 , f1_100 , f1_110
//  Frame 2 : f2_1 , f2_5 , f2_5  , f2_54 , f2_100 , f2_110
// Matrix 1 : m1_1 , m1_4 , m1_50 , m1_53 , m1_53  , m1_111
void synchronizer_test::toleranceTest()
{
    SynchronizerTester tester;
    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    // This is done just to handle automatic synch at first data push
    tester.addFrameToFrameTL(tester.frameTL1, 1);
    tester.srv->slot("reset")->run();

    //time 1: synch
    tester.addFrameToFrameTL(tester.frameTL2, 1);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 1);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 1);
    SynchronizerTester::checkFrame(tester.frame1, 1);
    SynchronizerTester::checkFrame(tester.frame2, 1);
    SynchronizerTester::checkMatrix(tester.matrix1, 1);

    //time 2: synch, all good, diff is bellow tolerance which is 5
    tester.addFrameToFrameTL(tester.frameTL1, 5);
    tester.addFrameToFrameTL(tester.frameTL2, 5);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 4);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 4);
    SynchronizerTester::checkFrame(tester.frame1, 5);
    SynchronizerTester::checkFrame(tester.frame2, 5);
    SynchronizerTester::checkMatrix(tester.matrix1, 4);

    //time 3: frame 2 is stuck. Tolerance is out
    tester.addFrameToFrameTL(tester.frameTL1, 50);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 50);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 50);
    SynchronizerTester::checkFrame(tester.frame1, 50);
    SynchronizerTester::checkFrame(tester.frame2, 5);
    SynchronizerTester::checkMatrix(tester.matrix1, 50);

    //time 3: frame 2 caches up
    tester.addFrameToFrameTL(tester.frameTL1, 53);
    tester.addFrameToFrameTL(tester.frameTL2, 54);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 53);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 53);
    SynchronizerTester::checkFrame(tester.frame1, 53);
    SynchronizerTester::checkFrame(tester.frame2, 54);
    SynchronizerTester::checkMatrix(tester.matrix1, 53);

    //time 4: matrix is stuck
    tester.addFrameToFrameTL(tester.frameTL1, 100);
    tester.addFrameToFrameTL(tester.frameTL2, 100);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 55);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 100);
    SynchronizerTester::checkFrame(tester.frame1, 100);
    SynchronizerTester::checkFrame(tester.frame2, 100);
    SynchronizerTester::checkMatrix(tester.matrix1, 53);

    //time 5: matrix catches up
    tester.addFrameToFrameTL(tester.frameTL1, 110);
    tester.addFrameToFrameTL(tester.frameTL2, 110);
    tester.addMatrixToMatrixTL(tester.matrixTL1, 111);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 110);
    SynchronizerTester::checkFrame(tester.frame1, 110);
    SynchronizerTester::checkFrame(tester.frame2, 110);
    SynchronizerTester::checkMatrix(tester.matrix1, 111);
}

//------------------------------------------------------------------------------

void synchronizer_test::imageSeriesTimeTaggingTest()
{
    const data::image::Size frameSize {2, 2, 1};

    std::stringstream config_string;
    config_string
    << "<in group=\"frameTL\">"
       "    <key uid=\"frameTL1\" />"
       "</in>"
       "<inout group=\"frames\">"
       "    <key uid=\"frame1\" />"
       "</inout>";
    // service::config_t config;
    // boost::property_tree::read_xml(config_string, config);

    SynchronizerTester tester(config_string);

    tester.srv->start().wait();
    tester.srv->update().wait();

    // create and set the input TL
    data::frame_tl::sptr frameTL1 = std::make_shared<data::frame_tl>();
    frameTL1->initPoolSize(
        frameSize[0],
        frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    tester.srv->set_input(frameTL1, "frameTL", true, false, 0);

    // create and set the inout which will be filled in the synchronization process
    sight::data::image_series::sptr frame1 = std::make_shared<sight::data::image_series>();

    frame1->setSOPKeyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    frame1->resize(
        frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame1 = frame1->dump_lock();
        std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
    }

    tester.srv->set_inout(frame1, "frames", false, false, 0);

    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection =
        tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    // This is done just to handle automatic synch at first data push
    const std::uint64_t timestamp = 13;

    const SPTR(data::frame_tl::BufferType) data = frameTL1->createBuffer(timestamp);
    std::uint8_t* eltBuffer = data->addElement(0);
    memset(eltBuffer, timestamp, frameSize[0] * frameSize[1]);

    frameTL1->pushObject(data);

    tester.srv->update().wait();

    fwTestWithFailWaitMacro(lastTimestampSynch == timestamp);

    const auto dt = frame1->getFrameAcquisitionDateTime();
    CPPUNIT_ASSERT(dt.has_value());

    const auto timePoint = frame1->getFrameAcquisitionTimePoint();
    CPPUNIT_ASSERT(timePoint.has_value());

    std::int64_t ts = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint->time_since_epoch()).count();
    CPPUNIT_ASSERT(ts == timestamp);

    tester.srv->stop().wait();
}

//------------------------------------------------------------------------------

void synchronizer_test::singleImageSeriesTLPopulation()
{
    const data::image::Size frameSize {2, 2, 1};

    /// Service setup
    std::stringstream config_string;
    config_string
    << "<in group=\"frameTL\">"
       "    <key uid=\"frameTL1\" />"
       "    <key uid=\"frameTL2\" />"
       "</in>"
       "<inout group=\"frames\">"
       "    <key uid=\"frame1\" tl=\"0\"/>"
       "    <key uid=\"frame2\" tl=\"1\" />"
       "</inout>"
       "<in group=\"matrixTL\">"
       "    <key uid=\"matrixTL1\" />"
       "</in>"
       "<inout group=\"matrix\">"
       "    <key uid=\"matrix1\" tl=\"0\"/>"
       "</inout>"
       "<tolerance>5</tolerance>";

    SynchronizerTester tester(config_string);

    tester.srv->start().wait();
    tester.srv->update().wait();

    /// Input/output setup
    // create and set the input TL
    data::frame_tl::sptr frameTL1 = std::make_shared<data::frame_tl>();
    frameTL1->initPoolSize(
        frameSize[0],
        frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );
    data::frame_tl::sptr frameTL2 = std::make_shared<data::frame_tl>();
    frameTL2->initPoolSize(
        frameSize[0],
        frameSize[1],
        core::type::UINT8,
        sight::data::frame_tl::PixelFormat::GRAY_SCALE
    );

    data::matrix_tl::sptr matrixTL1 = std::make_shared<data::matrix_tl>();
    matrixTL1->initPoolSize(4);

    tester.srv->set_input(frameTL1, "frameTL", true, false, 0);
    tester.srv->set_input(frameTL2, "frameTL", true, false, 1);
    tester.srv->set_input(matrixTL1, "matrixTL", true, false, 0);

    // create and set the inout which will be filled in the synchronization process
    sight::data::image_series::sptr frame1 = std::make_shared<sight::data::image_series>();

    frame1->setSOPKeyword(data::dicom::sop::Keyword::EnhancedUSVolumeStorage);
    frame1->resize(
        frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame1 = frame1->dump_lock();
        std::fill(frame1->begin<std::uint8_t>(), frame1->end<std::uint8_t>(), std::uint8_t(0));
    }

    data::image::sptr frame2 = std::make_shared<data::image>();
    frame2->resize(
        frameSize,
        core::type::UINT8,
        data::image::PixelFormat::GRAY_SCALE
    );
    {
        auto dump_lock_frame2 = frame2->dump_lock();
        std::fill(frame2->begin<std::uint8_t>(), frame2->end<std::uint8_t>(), std::uint8_t(0));
    }

    /// Processing
    data::matrix4::sptr matrix1 = std::make_shared<data::matrix4>();
    (*matrix1)(0, 0) = 0.; // init the first value a 0. This will be filled with the timestamp in the tests.

    tester.srv->set_inout(frame1, "frames", false, false, 0);
    tester.srv->set_inout(frame2, "frames", false, false, 1);
    tester.srv->set_inout(matrix1, "matrix", false, false, 0);

    core::hires_clock::type lastTimestampSynch = 0;
    auto slotSynchronizationDone               =
        sight::core::com::new_slot(
            [&lastTimestampSynch](core::hires_clock::type timestamp)
        {
            lastTimestampSynch = timestamp;
        });
    slotSynchronizationDone->set_worker(sight::core::thread::get_default_worker());
    auto synchDoneConnection = tester.srv->signal("synchronizationDone")->connect(slotSynchronizationDone);

    // populate the TLs
    tester.addFrameToFrameTL(frameTL1, 1);
    tester.addFrameToFrameTL(frameTL1, 2);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 2);
    //test the output
    SynchronizerTester::checkFrame(frame1, 2);
    SynchronizerTester::checkFrame(frame2, 0);
    SynchronizerTester::checkMatrix(matrix1, 0);

    std::optional<std::string> dt;
    std::optional<std::chrono::system_clock::time_point> timePoint;
    std::int64_t ts = 0;

    // Check that the ImageSeries timestamp was written
    dt = frame1->getFrameAcquisitionDateTime();
    CPPUNIT_ASSERT(dt.has_value());
    timePoint = frame1->getFrameAcquisitionTimePoint();
    CPPUNIT_ASSERT(timePoint.has_value());
    ts = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint->time_since_epoch()).count();
    CPPUNIT_ASSERT(ts == 2);

    // populate the TL
    tester.addFrameToFrameTL(frameTL1, 3);
    tester.srv->slot("synchronize")->run();
    fwTestWithFailWaitMacro(lastTimestampSynch == 3);
    //test the output
    SynchronizerTester::checkFrame(frame1, 3);
    SynchronizerTester::checkFrame(frame2, 0);
    SynchronizerTester::checkMatrix(matrix1, 0);

    // Check that the ImageSeries timestamp was written
    dt = frame1->getFrameAcquisitionDateTime();
    CPPUNIT_ASSERT(dt.has_value());
    timePoint = frame1->getFrameAcquisitionTimePoint();
    CPPUNIT_ASSERT(timePoint.has_value());
    ts = std::chrono::duration_cast<std::chrono::milliseconds>(timePoint->time_since_epoch()).count();
    CPPUNIT_ASSERT(ts == 3);

    tester.srv->stop().wait();
}

//------------------------------------------------------------------------------

} // namespace sight::module::sync::ut
