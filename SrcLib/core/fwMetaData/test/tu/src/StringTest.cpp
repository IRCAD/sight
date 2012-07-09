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
