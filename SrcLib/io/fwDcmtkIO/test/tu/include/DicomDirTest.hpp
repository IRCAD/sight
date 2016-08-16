/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDCMTKIO_UT_DICOMDIRTEST_HPP__
#define __FWDCMTKIO_UT_DICOMDIRTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <boost/filesystem.hpp>

namespace fwDcmtkIO
{
namespace ut
{

class DicomDirTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( DicomDirTest );
CPPUNIT_TEST( readDicomDir );
CPPUNIT_TEST_SUITE_END();

public:

    // Interface
    void setUp();
    void tearDown();

    // Test functions
    void readDicomDir();

};

} // namespace ut
} // namespace fwDcmtkIO

#endif // __FWDCMTKIO_UT_DICOMDIRTEST_HPP__

