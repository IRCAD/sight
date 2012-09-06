/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <limits>

#include <fwMetaData/String.hpp>
#include "StringTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMetaData::ut::StringTest );

namespace fwMetaData
{
namespace ut
{

void StringTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void StringTest::tearDown()
{
    // Clean up after the test run.
}


void StringTest::conversion()
{
    const std::string VALUES[] = {"", "azerty"};
    fwMetaData::String::sptr metaString;

    BOOST_FOREACH ( std::string str, VALUES )
    {
        metaString = ::fwMetaData::String::New(str);

        CPPUNIT_ASSERT_EQUAL(str, metaString->getString());
    }
}

}  // namespace ut
}  // namespace fwMetaData
