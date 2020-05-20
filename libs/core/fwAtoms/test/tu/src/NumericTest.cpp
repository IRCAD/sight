/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "NumericTest.hpp"

#include <fwAtoms/Exception.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>

#include <boost/lexical_cast.hpp>

#include <limits>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtoms::ut::NumericTest );

namespace fwAtoms
{
namespace ut
{

//------------------------------------------------------------------------------

void NumericTest::setUp()
{
    // Set up context before running a test.
}

//-----------------------------------------------------------------------------

void NumericTest::tearDown()
{
    // Clean up after the test run.
}

//-----------------------------------------------------------------------------

void NumericTest::emptyTest()
{
    fwAtoms::Numeric::sptr numeric = fwAtoms::Numeric::New();
    CPPUNIT_ASSERT_THROW(numeric->getString(), ::fwAtoms::Exception);
}

//-----------------------------------------------------------------------------

void NumericTest::intTest()
{
    fwAtoms::Numeric::sptr numeric;

    numeric = fwAtoms::Numeric::New(42);
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(42), numeric->getString());

    numeric = fwAtoms::Numeric::New(1ULL<<63);
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(1ULL<<63), numeric->getString());
    CPPUNIT_ASSERT_THROW( numeric->getValue<int>(), boost::numeric::positive_overflow );

    numeric = fwAtoms::Numeric::New(-22);
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(-22), numeric->getString());
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(-22), numeric->getValue<int>());
    CPPUNIT_ASSERT_THROW( numeric->getValue<unsigned int>(), boost::numeric::negative_overflow );

    numeric = fwAtoms::Numeric::New(true);
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(true), numeric->getString());

    numeric = fwAtoms::Numeric::New('a');
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(static_cast<int>('a')), numeric->getString());

    numeric = fwAtoms::Numeric::New(std::string("42"));
    CPPUNIT_ASSERT_EQUAL(std::string("42"), numeric->getString());
    CPPUNIT_ASSERT_EQUAL(static_cast<char>(42), numeric->getValue<char>());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned char>(42), numeric->getValue<unsigned char>());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned short>(42), numeric->getValue<unsigned short>());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(42), numeric->getValue<unsigned int>());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long int>(42), numeric->getValue<unsigned long int>());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned long long int>(42), numeric->getValue<unsigned long long int>());
    CPPUNIT_ASSERT_EQUAL(static_cast<char>(42), numeric->getValue<char>());
    CPPUNIT_ASSERT_EQUAL(static_cast<short>(42), numeric->getValue<short>());
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(42), numeric->getValue<int>());
    CPPUNIT_ASSERT_EQUAL(static_cast<long int>(42), numeric->getValue<long int>());
    CPPUNIT_ASSERT_EQUAL(static_cast<long long int>(42), numeric->getValue<long long int>());
    CPPUNIT_ASSERT_EQUAL(static_cast<float>(42), numeric->getValue<float>());
    CPPUNIT_ASSERT_EQUAL(static_cast<double>(42), numeric->getValue<double>());

    numeric = fwAtoms::Numeric::New(std::string("422"));
    CPPUNIT_ASSERT_EQUAL(422, numeric->getValue<int>());
    CPPUNIT_ASSERT_THROW( numeric->getValue<char>(), boost::numeric::positive_overflow );

}

//-----------------------------------------------------------------------------

void NumericTest::floatingTest()
{
    fwAtoms::Numeric::sptr numeric;

    typedef boost::variant<float, double> VariantType;

    double d1 = 0.234;
    double d2 = std::numeric_limits< double >::max();
    double d3 = -42.4;
    float f1  = 4327.452359;

    numeric = fwAtoms::Numeric::New(d1);
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(VariantType(d1)), numeric->getString());
    CPPUNIT_ASSERT_EQUAL(d1, numeric->getValue<double>());
    CPPUNIT_ASSERT_EQUAL((float)d1, numeric->getValue<float>());

    numeric = fwAtoms::Numeric::New(d2);
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(VariantType(d2)), numeric->getString());
    CPPUNIT_ASSERT_EQUAL(d2, numeric->getValue<double>());
    CPPUNIT_ASSERT_THROW( numeric->getValue< float >(), boost::numeric::positive_overflow );
    CPPUNIT_ASSERT_THROW( numeric->getValue< std::uint64_t >(), boost::numeric::positive_overflow );

    numeric = fwAtoms::Numeric::New(d3);
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(VariantType(d3)), numeric->getString());
    CPPUNIT_ASSERT_EQUAL(d3, numeric->getValue<double>());
    CPPUNIT_ASSERT_EQUAL(static_cast<int>(d3), numeric->getValue<int>());
    CPPUNIT_ASSERT_THROW( numeric->getValue< unsigned int >(), boost::numeric::negative_overflow );

    numeric = fwAtoms::Numeric::New(f1);
    CPPUNIT_ASSERT_EQUAL(::boost::lexical_cast<std::string>(VariantType(f1)), numeric->getString());
    CPPUNIT_ASSERT_EQUAL(f1, numeric->getValue<float>());
    CPPUNIT_ASSERT_EQUAL((double)f1, numeric->getValue<double>());
    CPPUNIT_ASSERT_THROW( numeric->getValue< char >(), boost::numeric::positive_overflow );
}

//-----------------------------------------------------------------------------

class variant_visitor : public boost::static_visitor<>
{
public:

    //------------------------------------------------------------------------------

    void operator()( ::boost::blank& ) const
    {
        CPPUNIT_ASSERT_MESSAGE("This shall not happen", 0);
    }
    //------------------------------------------------------------------------------

    template <typename U>
    void operator()( U& value ) const
    {
        fwAtoms::Numeric::sptr numeric = ::fwAtoms::Numeric::New(value);
        CPPUNIT_ASSERT_EQUAL(U(value), numeric->getValue<U>());
        CPPUNIT_ASSERT_EQUAL(
            ::boost::lexical_cast<std::string>(fwAtoms::Numeric::ValueType(value)),
            numeric->getString()
            );

        CPPUNIT_ASSERT_EQUAL(typeid(U).name(), numeric->getVariant().type().name());
    }
};

//------------------------------------------------------------------------------

void NumericTest::mixedTest()
{
    fwAtoms::Numeric::ValueType values[] = {
        std::numeric_limits<  std::int64_t >::min(),
        std::int64_t(-1234),
        std::int64_t(0),
        std::int64_t(1234),
        std::numeric_limits<  std::int64_t >::max(),

        std::numeric_limits<  std::uint64_t >::min(),
        std::uint64_t(-1234),
        std::uint64_t(0),
        std::uint64_t(1234),
        std::numeric_limits<  std::uint64_t >::max(),

        -std::numeric_limits<  float >::max(),
        float(-1234),
        float(0),
        std::numeric_limits<  float >::min(),
        float(1234),
        std::numeric_limits<  float >::max(),

        -std::numeric_limits<  double >::max(),
        double(-1234),
        double(0),
        std::numeric_limits<  double >::min(),
        double(1234),
        std::numeric_limits<  double >::max()

    };

    for( fwAtoms::Numeric::ValueType& v : values )
    {
        boost::apply_visitor( variant_visitor(), v );
    }

}

}  // namespace ut
}  // namespace fwAtoms

