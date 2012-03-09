/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <vector>
#include <sstream>
#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>
#include <boost/function_types/result_type.hpp>
#include <boost/function_types/function_type.hpp>
#include <boost/function_types/member_function_pointer.hpp>

#include <boost/typeof/typeof.hpp>

#include <fwTools/random/Generator.hpp>
#include <fwData/Object.hpp>
#include <fwData/macros.hpp>

#include "MacrosTest.hpp"

#define __FWDATA_UT_TEST_RETURN_TYPE(Func, ExpectedType)                                            \
{                                                                                                   \
    typedef BOOST_TYPEOF(&Func) F;                                                                  \
    typedef ::boost::function_types::result_type< F >::type R;                                      \
    typedef ::boost::is_same<ExpectedType, R >::type IsFuncOfExpectedType;                          \
    std::stringstream ss;                                                                           \
    ss << "Expected type : " << #ExpectedType << " (typeid: "                                       \
            << typeid(ExpectedType).name() << "), got typeid: "                                     \
            << ( ( ::boost::is_const< ::boost::remove_reference<R>::type >::value) ? " const " : "")\
            << typeid(R).name()  << ( ( ::boost::is_reference<R>::value) ? " &" : "")               \
            ;                                                                                       \
    CPPUNIT_ASSERT_MESSAGE( ss.str(), IsFuncOfExpectedType::value );                                \
}



// Registers the fixture into the 'registry'
CPPUNIT_TEST_SUITE_REGISTRATION( ::fwData::ut::MacrosTest );

namespace fwData
{
namespace ut
{

class MyObjectTest
{
public:

    typedef std::vector< float > VectFloatType;

    fwDataGetSetMacro(ValInt, int)

    fwDataGetSetCRefMacro(VectFloat, VectFloatType)

    fwDataGetSetSptrMacro(Object, ::fwData::Object::sptr)

    int m_attrValInt;
    VectFloatType m_attrVectFloat;
    ::fwData::Object::sptr m_attrObject;
};

void MacrosTest::setUp()
{
    // Set up context before running a test.

}
void MacrosTest::tearDown()
{
    // Clean up after the test run.
}

void MacrosTest::getterSetterTest()
{
    __FWDATA_UT_TEST_RETURN_TYPE(MyObjectTest::getValInt, int);
    __FWDATA_UT_TEST_RETURN_TYPE(MyObjectTest::getVectFloat,  const MyObjectTest::VectFloatType &);
    __FWDATA_UT_TEST_RETURN_TYPE(MyObjectTest::getObject, ::fwData::Object::sptr);

    int valInt = ::fwTools::random::getValue(-100, 100);
    MyObjectTest::VectFloatType vectFloat(100, 0);
    ::fwTools::random::fillContainer(0.f, 100.f, vectFloat);
    ::fwData::Object::sptr object = ::fwData::Object::New();

    MyObjectTest objTest;
    objTest.setValInt(valInt);
    objTest.setVectFloat(vectFloat);
    objTest.setObject(object);

    CPPUNIT_ASSERT_EQUAL(valInt, objTest.getValInt() );
    CPPUNIT_ASSERT(vectFloat == objTest.getVectFloat() );
    CPPUNIT_ASSERT_EQUAL(object, objTest.getObject() );
}

} //namespace ut
} //namespace fwData

