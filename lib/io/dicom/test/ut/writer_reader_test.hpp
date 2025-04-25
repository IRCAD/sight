/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include <data/series_set.hpp>

#include <service/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::dicom::ut
{

class writer_reader_test : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(writer_reader_test);
CPPUNIT_TEST(write_read_image_series_test);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp() override;
    void tearDown() override;

    static void write_read_image_series_test();

private:

    /**
     * Create a series set containing an ImageSeries with landmarks and distance
     * and a ModelSeries with one reconstruction.
     */
    static data::series_set::sptr create_series_set();
};

} // namespace sight::io::dicom::ut
