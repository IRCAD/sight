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

#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace sight::module::sync::ut
{

/**
 * @brief Test the synchronizer service.
 *
 * For reminding, the tested service configuration is as follow:
 *
   @code{.xml}
        <service type="sight::module::sync::synchronizer" auto_connect="true">
            <in group="frameTL">
                <key uid="frameTL1" />
                <key uid="frameTL4" />
                <key uid="frameTL6" />
            </in>
            <inout group="frames">
                <key uid="frame1" sendStatus="true" />
                <key uid="frame6" tl="2" />
                <key uid="frame4" tl="1" sendStatus="false"/>
                <key uid="frame11" tl="0"  sendStatus="true" />
            </inout>
            <in group="matrixTL">
                <key uid="matrixTL1" />
                <key uid="matrixTL2" />
            </in>
            <inout group="matrix">
                <key uid="matrix0" index="1" sendStatus="true" >
                <key uid="matrix1" tl="0" index="0" />
                <key uid="matrix2" tl="1" index="0" sendStatus="false"/>
                <key uid="matrix3" tl="1" index="1"/>
                <key uid="matrix4" tl="0" index="2"/>
            </inout>
            <framerate>30</framerate>
            <tolerance>500</tolerance>
       </service>
   @endcode
 *
 * The test will cover different fields.
 *   - the synchronisation process.
 * Those tests will have a fixed configuration, with always the same number of input TL, and output variables
 * They will verify that the expected frames and matrix are filled in the out variables, depending on the input
 * timestamps
 * There will be some test which confirms the behaviour when some tls are not populated.
 *
 *    -  configurations and in/output association
 * These tests are configuration dependent.
 * They will verify that the appropriate output variable is filled, and that the association input/out is well done.
 * There will be some tests on the state signal emission too
 *
 *    - other configuration options
 * The service present various configuration options: delay, etc..
 * The remaining test will cover all those additional behaviors.
 *
 * To simply the process, the frames internal values and the matrix first element are set to the timestamp value
 * This make it possible to simply test which frame/matrix has been provided to the out variable.
 *
 *
 */
class synchronizer_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(synchronizer_test);
CPPUNIT_TEST(single_frame_tl_population);
CPPUNIT_TEST(single_matrix_tl_population);
CPPUNIT_TEST(basic_synchronisation);
CPPUNIT_TEST(time_gap_synchronisation);
CPPUNIT_TEST(reset_timeline_synchronisation);
CPPUNIT_TEST(reset_and_loop_synchronisation);
CPPUNIT_TEST(single_matrix_tl_config_test);
CPPUNIT_TEST(mixt_matrix_tl_config_test);
CPPUNIT_TEST(single_frame_tl_config_test);
CPPUNIT_TEST(mixt_frame_tl_config_test);
CPPUNIT_TEST(full_config_test);
CPPUNIT_TEST(update_config_test);
CPPUNIT_TEST(send_status_test);
CPPUNIT_TEST(delay_test);
CPPUNIT_TEST(tolerance_test);
CPPUNIT_TEST(image_series_time_tagging_test);
CPPUNIT_TEST(single_image_series_tl_population);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    /// Synchronisation process
    /// For this section, the max delay for a successful synchronisation is 5.

    /// Give frames to  only one frameTL and check that the out frame is well filled
    static void single_frame_tl_population();

    /// Give matrix to  only one matrixTL and check that the out matrix is well filled
    static void single_matrix_tl_population();

    /// Classical synchronisation with variations
    static void basic_synchronisation();

    ///Classical synchronisation with time gap
    static void time_gap_synchronisation();

    /// Classical synchronisation with resets
    static void reset_timeline_synchronisation();

    /// Classical synchronisation with resets and loops
    static void reset_and_loop_synchronisation();

    ///configuration tests
    //test the following configuration, with simple frame/matrix input output

    // single matrixTL without framesTL
    static void single_matrix_tl_config_test();

    //multi matrixTL Configurations without framesTL
    static void mixt_matrix_tl_config_test();

    // single frameTL without matrixTL
    static void single_frame_tl_config_test();

    // Multi framesTL configuration without matrixTL
    static void mixt_frame_tl_config_test();

    // Multi framesTL and matrixTL configurations
    static void full_config_test();

    /// Take the classical configuration, and change the association through slot
    static void update_config_test();

    /// Take the classical configuration, and add send status to Frame 1 and Matrix 1
    static void send_status_test();

    /// other configuration options Tests

    //set a delay of 2 to frameTl and of  1 to matrixTL1
    // use the standard configuration for tests
    static void delay_test();

    /// The classical tolerance is 5. Make one TL or the other out of range
    static void tolerance_test();

    /// When the output is an ImageSeries object, instead of an image
    /// Checks that we write the acquisition time as a dicom attribute
    static void image_series_time_tagging_test();

    /// Test with an ImageSeries and matrices to ensure timestamp data is written in the ImageSeries
    /// assuming a more complex context
    static void single_image_series_tl_population();
};

} // namespace sight::module::sync::ut
