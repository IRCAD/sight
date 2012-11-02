/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>
#include <limits>

#include <fwAtoms/Numeric.hpp>
#include "NumericTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtoms::ut::NumericTest );

namespace fwAtoms
{
namespace ut
{

void NumericTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void NumericTest::tearDown()
{
    // Clean up after the test run.
}


void NumericTest::intConversion()
{
    const int VALUES[] = {
            std::numeric_limits< int >::min(), -1234, 0, 1234, std::numeric_limits<int>::max()};
    fwAtoms::Numeric::sptr metaInt;

    BOOST_FOREACH ( int INT, VALUES )
    {
        std::string res = boost::lexical_cast< std::string>( INT);

        metaInt = ::fwAtoms::Numeric::New(INT);

        CPPUNIT_ASSERT_EQUAL(res, metaInt->getString());
    }
}

void NumericTest::uintConversion()
{
    const unsigned int VALUES[] = {
            std::numeric_limits< unsigned int >::min(), 0, 1234, std::numeric_limits<unsigned int>::max()};
    fwAtoms::Numeric::sptr metaInt;

    BOOST_FOREACH ( unsigned int INT, VALUES )
    {
        std::string res = boost::lexical_cast< std::string>( INT);

        metaInt = ::fwAtoms::Numeric::New(INT);

        CPPUNIT_ASSERT_EQUAL(res, metaInt->getString());
    }
}

void NumericTest::int64Conversion()
{
    const ::boost::int64_t VALUES[] = {
            std::numeric_limits<  ::boost::int64_t >::min(),-1234, 0, 1234,
            std::numeric_limits<  ::boost::int64_t >::max()};

    fwAtoms::Numeric::sptr metaInt;

    BOOST_FOREACH ( boost::int64_t INT, VALUES )
    {
        std::string res = boost::lexical_cast< std::string>( INT);

        metaInt = ::fwAtoms::Numeric::New(INT);

        CPPUNIT_ASSERT_EQUAL(res, metaInt->getString());
    }
}

void NumericTest::uint64Conversion()
{
    const ::boost::uint64_t VALUES[] = {
            std::numeric_limits<  ::boost::uint64_t >::min(),1234, 0, 12345678,
            std::numeric_limits<  ::boost::uint64_t >::max()};

    fwAtoms::Numeric::sptr metaInt;

    BOOST_FOREACH ( boost::uint64_t INT, VALUES )
    {
        std::string res = boost::lexical_cast< std::string>( INT);

        metaInt = ::fwAtoms::Numeric::New(INT);

        CPPUNIT_ASSERT_EQUAL(res, metaInt->getString());
    }

}

void NumericTest::floatConversion()
{
    const float VALUES[] = {
            std::numeric_limits< float >::min(), -1234.1234, 0, 1234.1234, std::numeric_limits<float>::max()};
    fwAtoms::Numeric::sptr metaFloat;

    BOOST_FOREACH ( float FLOAT, VALUES )
    {
        std::string res = boost::lexical_cast< std::string>( FLOAT);

        metaFloat = ::fwAtoms::Numeric::New(FLOAT);

        CPPUNIT_ASSERT_EQUAL(res, metaFloat->getString());
    }
}

void NumericTest::doubleConversion()
{
    const double VALUES[] = {
            std::numeric_limits< double >::min(), -1234.1234, 0, 1234.1234, std::numeric_limits<double>::max()};
    fwAtoms::Numeric::sptr metaFloat;

    BOOST_FOREACH ( double FLOAT, VALUES )
    {
        std::string res = boost::lexical_cast< std::string>( FLOAT);

        metaFloat = ::fwAtoms::Numeric::New(FLOAT);

        CPPUNIT_ASSERT_EQUAL(res, metaFloat->getString());
    }
}

}  // namespace ut
}  // namespace fwAtoms
