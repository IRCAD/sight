/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include <fwMedData/ActivitySeries.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwMedData
{

namespace ut
{

class ActivitySeriesTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( ActivitySeriesTest );

CPPUNIT_TEST(activityConfigIdTest);
CPPUNIT_TEST(dataTest);

CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void activityConfigIdTest();
    void dataTest();

protected:

    SPTR(::fwMedData::ActivitySeries) m_series;

private:

    /// Dummy variable members used to force link with fwDataCamp, fwMedDataCamp and arDataCamp bundles
    /// Recent GCC versions (>= 5.4) strip local variables with -O2
    int m_fwDataVersion;
    int m_fwMedDataVersion;

};

} //namespace ut
} //namespace fwMedData
