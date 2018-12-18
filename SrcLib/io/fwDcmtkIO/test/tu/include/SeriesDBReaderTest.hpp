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

#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDcmtkIO
{
namespace ut
{

class SeriesDBReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( SeriesDBReaderTest );
CPPUNIT_TEST( readJMSSeriesDBTest );
CPPUNIT_TEST( readJMSLazySeriesDBTest );
CPPUNIT_TEST( readCTSeriesDBTest );
CPPUNIT_TEST( readCTLazySeriesDBTest );
CPPUNIT_TEST( readMRSeriesDBTest );
CPPUNIT_TEST( readMRLazySeriesDBTest );
CPPUNIT_TEST( readOTSeriesDBTest );
CPPUNIT_TEST( readOTLazySeriesDBTest );
CPPUNIT_TEST( readUnsupportedSeriesDBTest );
CPPUNIT_TEST( readUnsupportedLazySeriesDBTest );
CPPUNIT_TEST( readDisabledSeriesDBTest );
CPPUNIT_TEST( readDisabledLazySeriesDBTest );
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    /// Read JMS Series
    void readJMSSeriesDBTest();
    void readJMSLazySeriesDBTest();

    /// Read CT Series (01-CT-DICOM_LIVER)
    void readCTSeriesDBTest();
    void readCTLazySeriesDBTest();

    /// Read MR Series (46-MR-BARRE-MONO2-12-shoulder)
    void readMRSeriesDBTest();
    void readMRLazySeriesDBTest();

    /// Read OT Series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    void readOTSeriesDBTest();
    void readOTLazySeriesDBTest();

    /// Read unsupported Series (71-CT-DICOM_SEG)
    void readUnsupportedSeriesDBTest();
    void readUnsupportedLazySeriesDBTest();

    /// Read disabled Series (Try to read MR when only CT is enabled)
    void readDisabledSeriesDBTest();
    void readDisabledLazySeriesDBTest();

protected:
    /// Read and check JMS series
    void readJMSSeries();

    /// Read and check CT series (01-CT-DICOM_LIVER)
    void readCTSeries();

    /// Read and check MR series (46-MR-BARRE-MONO2-12-shoulder)
    void readMRSeries();

    /// Read and check OT series (42-OT-BARRE-MONO2-8-colon) (Secondary capture)
    void readOTSeries();

    /// Read unsupported series (71-CT-DICOM_SEG)
    void readUnsupportedSeries();

    /// Read disabled series (Try to read MR when only CT is enabled)
    void readDisabledSeries();

};

} // namespace ut
} // namespace fwDcmtkIO
