/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_UT_DICOMSERIESDBREADERTEST_HPP__
#define __FWDCMTKIO_UT_DICOMSERIESDBREADERTEST_HPP__

#include <fwServices/macros.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwDcmtkIO
{
namespace ut
{

class DicomSeriesDBReaderTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DicomSeriesDBReaderTest );
CPPUNIT_TEST( readDicomSeriesDBTest );
CPPUNIT_TEST_SUITE_END();

public:

    void setUp();
    void tearDown();

    void readDicomSeriesDBTest();

};

} // namespace ut
} // namespace fwDcmtkIO

#endif // __FWDCMTKIO_UT_DICOMSERIESDBREADERTEST_HPP__

