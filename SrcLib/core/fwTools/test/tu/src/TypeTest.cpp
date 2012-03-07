/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/Type.hpp>

#include "TypeTest.hpp"

// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwTools::ut::TypeTest );

namespace fwTools
{
namespace ut
{

void TypeTest::setUp()
{
    // Set up context before running a test.
}

void TypeTest::tearDown()
{
    // Clean up after the test run.
}

void TypeTest::typeTest()
{
    ::fwTools::Type INT8   ( ::fwTools::Type::s_INT8_TYPENAME   );
    ::fwTools::Type INT16  ( ::fwTools::Type::s_INT16_TYPENAME  );
    ::fwTools::Type INT32  ( ::fwTools::Type::s_INT32_TYPENAME  );
    ::fwTools::Type INT64  ( ::fwTools::Type::s_INT64_TYPENAME  );

    ::fwTools::Type UINT8  ( ::fwTools::Type::s_UINT8_TYPENAME  );
    ::fwTools::Type UINT16 ( ::fwTools::Type::s_UINT16_TYPENAME );
    ::fwTools::Type UINT32 ( ::fwTools::Type::s_UINT32_TYPENAME );
    ::fwTools::Type UINT64 ( ::fwTools::Type::s_UINT64_TYPENAME );

    ::fwTools::Type FLOAT  ( ::fwTools::Type::s_FLOAT_TYPENAME  );
    ::fwTools::Type DOUBLE ( ::fwTools::Type::s_DOUBLE_TYPENAME );

    ::fwTools::Type t;

    CPPUNIT_ASSERT_EQUAL( ::fwTools::Type::s_UNSPECIFIED_TYPE,  ::fwTools::Type() );

    CPPUNIT_ASSERT_EQUAL( INT8 , ::fwTools::Type::create< signed char >() );
    CPPUNIT_ASSERT_EQUAL( INT8 , ::fwTools::Type::create< char >() );
    CPPUNIT_ASSERT_EQUAL( INT16, ::fwTools::Type::create< short >() );
    CPPUNIT_ASSERT_EQUAL( INT32, ::fwTools::Type::create< int >() );
    CPPUNIT_ASSERT_EQUAL( INT64, ::fwTools::Type::create< long long >() );

    CPPUNIT_ASSERT_EQUAL( UINT8 , ::fwTools::Type::create< unsigned char >() );
    CPPUNIT_ASSERT_EQUAL( UINT16, ::fwTools::Type::create< unsigned short >() );
    CPPUNIT_ASSERT_EQUAL( UINT32, ::fwTools::Type::create< unsigned int >() );
    CPPUNIT_ASSERT_EQUAL( UINT64, ::fwTools::Type::create< unsigned long long >() );

    CPPUNIT_ASSERT_EQUAL( FLOAT , ::fwTools::Type::create< float  >() );
    CPPUNIT_ASSERT_EQUAL( DOUBLE, ::fwTools::Type::create< double >() );

    CPPUNIT_ASSERT_EQUAL( ::fwTools::Type::s_UNSPECIFIED_TYPE,  t );


    t.setType<void       >();

    t.setType<signed char       >();
    CPPUNIT_ASSERT_EQUAL( INT8   , t ); 
    t.setType<char              >();
    CPPUNIT_ASSERT_EQUAL( INT8   , t ); 
    t.setType<short             >();
    CPPUNIT_ASSERT_EQUAL( INT16  , t ); 
    t.setType<int               >();
    CPPUNIT_ASSERT_EQUAL( INT32  , t ); 
    t.setType<long long         >();
    CPPUNIT_ASSERT_EQUAL( INT64  , t ); 
    t.setType<unsigned char     >();
    CPPUNIT_ASSERT_EQUAL( UINT8  , t ); 
    t.setType<unsigned short    >();
    CPPUNIT_ASSERT_EQUAL( UINT16 , t ); 
    t.setType<unsigned int      >();
    CPPUNIT_ASSERT_EQUAL( UINT32 , t ); 
    t.setType<unsigned long long>();
    CPPUNIT_ASSERT_EQUAL( UINT64 , t ); 
    t.setType<float             >();
    CPPUNIT_ASSERT_EQUAL( FLOAT  , t ); 
    t.setType<double            >();
    CPPUNIT_ASSERT_EQUAL( DOUBLE , t ); 

    CPPUNIT_ASSERT( typeid(void) == ::fwTools::Type::s_UNSPECIFIED_TYPE.typeId());

    CPPUNIT_ASSERT( typeid(signed char) == INT8.typeId() );
    CPPUNIT_ASSERT( typeid(short      ) == INT16.typeId() );
    CPPUNIT_ASSERT( typeid(int        ) == INT32.typeId() );
    CPPUNIT_ASSERT( typeid(long long  ) == INT64.typeId() );

    CPPUNIT_ASSERT( typeid(unsigned char     ) == UINT8.typeId()  );
    CPPUNIT_ASSERT( typeid(unsigned short    ) == UINT16.typeId()  );
    CPPUNIT_ASSERT( typeid(unsigned int      ) == UINT32.typeId()  );
    CPPUNIT_ASSERT( typeid(unsigned long long) == UINT64.typeId()  );

    CPPUNIT_ASSERT( typeid(float ) == FLOAT.typeId()  );
    CPPUNIT_ASSERT( typeid(double) == DOUBLE.typeId()  );
}

} // namespace ut
} // namespace fwTools

