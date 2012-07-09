#include <boost/foreach.hpp>
#include <limits>

#include <fwMetaData/Boolean.hpp>
#include "BooleanTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwMetaData::ut::BooleanTest );

namespace fwMetaData
{
namespace ut
{

void BooleanTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void BooleanTest::tearDown()
{
    // Clean up after the test run.
}


void BooleanTest::conversion()
{
    const bool VALUES[] = {true, false};

    ::fwMetaData::Boolean::sptr meta;

    BOOST_FOREACH ( bool BOOL, VALUES )
    {
        std::string res = BOOL ? "true" : "false";

        meta = ::fwMetaData::Boolean::New(BOOL);

        CPPUNIT_ASSERT_EQUAL(res, meta->getString());
    }
}


}  // namespace ut
}  // namespace fwMetaData
