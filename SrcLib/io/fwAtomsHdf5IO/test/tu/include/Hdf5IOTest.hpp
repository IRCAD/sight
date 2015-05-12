/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSHDF5IO_UT_HDF5IOTEST_HPP__
#define __FWATOMSHDF5IO_UT_HDF5IOTEST_HPP__

#include <cppunit/extensions/HelperMacros.h>

#include <fwAtomsHdf5IO/Writer.hpp>

namespace fwAtomsHdf5IO
{
namespace ut
{

class Hdf5IOTest :  public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( Hdf5IOTest );
CPPUNIT_TEST( readWriteTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void readWriteTest();

protected:
    void readProcess(::boost::filesystem::path path);

    void writeProcess(::boost::filesystem::path path);
};


}  // namespace ut
}  // namespace fwAtomsHdf5IO



#endif // __FWATOMSHDF5IO_UT_HDF5IOTEST_HPP__


