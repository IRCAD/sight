/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/System.hpp>

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwAtomsBoostIO/Reader.hpp>
#include <fwAtomsBoostIO/Writer.hpp>

#include "BoostIOTest.hpp"


CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtomsBoostIO::ut::BoostIOTest );

namespace fwAtomsBoostIO
{
namespace ut
{



void BoostIOTest::setUp()
{
}

void BoostIOTest::tearDown()
{
}


void BoostIOTest::writeTest()
{
    std::stringstream sstr;

    ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::New();
    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::New();
    ::fwAtoms::Blob::sptr blob = ::fwAtoms::Blob::New();
    ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();

    ::fwAtoms::Boolean::sptr boolFalse = ::fwAtoms::Boolean::New(false);
    ::fwAtoms::Boolean::sptr boolTrue = ::fwAtoms::Boolean::New(true);
    ::fwAtoms::Numeric::sptr num = ::fwAtoms::Numeric::New(42);
    ::fwAtoms::Numeric::sptr num2 = ::fwAtoms::Numeric::New(16.64);
    ::fwAtoms::String::sptr str = ::fwAtoms::String::New("MyStr");

    const std::string META_KEY_0 = "info.a<b>.c.0";
    const std::string META_KEY_1 = "info.a<b>.c.1";
    const std::string META_KEY_2 = "info.a<b>.c.2";
    const std::string META_KEY_3 = "info.a<b>.c.3";
    const std::string META_VALUE_0 = "A";
    const std::string META_VALUE_1 = "B";
    const std::string META_VALUE_2 = "C";
    const std::string META_VALUE_3 = "D";

    obj->setMetaInfo(META_KEY_0, META_VALUE_0);
    obj->setMetaInfo(META_KEY_1, META_VALUE_1);
    obj->setMetaInfo(META_KEY_2, META_VALUE_2);
    obj->setMetaInfo(META_KEY_3, META_VALUE_3);

    const std::string ATTR_NAME_0 = "m_blob";
    const std::string ATTR_NAME_1 = "m_map";
    const std::string ATTR_NAME_2 = "m_str";
    const std::string ATTR_NAME_3 = "m_num";
    const std::string ATTR_NAME_4 = "NULL";

    obj->setAttribute(ATTR_NAME_0, blob);
    obj->setAttribute(ATTR_NAME_1, map);
    obj->setAttribute(ATTR_NAME_2, str);
    obj->setAttribute(ATTR_NAME_3, num);
    obj->setAttribute(ATTR_NAME_4, ::fwAtoms::String::sptr());

    seq->push_back(obj);
    seq->push_back(obj);
    seq->push_back(blob);
    seq->push_back(map);
    seq->push_back(seq);
    seq->push_back(boolFalse);
    seq->push_back(num);
    seq->push_back(str);
    seq->push_back(::fwAtoms::String::sptr());
    seq->push_back(num2);
    seq->push_back(boolTrue);


    map->insert("blob key", ::fwAtoms::Blob::New());
    map->insert("object key", ::fwAtoms::Object::New());
    map->insert("sequence key", ::fwAtoms::String::New("map-str"));

    ::fwAtomsBoostIO::Writer(seq).write(sstr, ::fwAtomsBoostIO::Writer::JSON);





    ::fwAtoms::Sequence::sptr readSeq = ::fwAtoms::Sequence::dynamicCast( ::fwAtomsBoostIO::Reader().read(sstr) );

    CPPUNIT_ASSERT_EQUAL(seq->size(), readSeq->size());

    ::fwAtoms::Object::sptr   readObj0  = ::fwAtoms::Object::dynamicCast((*readSeq)[0]);
    ::fwAtoms::Object::sptr   readObj1  = ::fwAtoms::Object::dynamicCast((*readSeq)[1]);
    ::fwAtoms::Blob::sptr     readBlob  = ::fwAtoms::Blob::dynamicCast((*readSeq)[2]);
    ::fwAtoms::Map::sptr      readMap   = ::fwAtoms::Map::dynamicCast((*readSeq)[3]);
    ::fwAtoms::Sequence::sptr readSeq2  = ::fwAtoms::Sequence::dynamicCast((*readSeq)[4]);
    ::fwAtoms::Boolean::sptr  readBoolF = ::fwAtoms::Boolean::dynamicCast((*readSeq)[5]);
    ::fwAtoms::Numeric::sptr  readNum   = ::fwAtoms::Numeric::dynamicCast((*readSeq)[6]);
    ::fwAtoms::String::sptr   readStr   = ::fwAtoms::String::dynamicCast((*readSeq)[7]);
    ::fwAtoms::Base::sptr     readNull  = ::fwAtoms::Base::dynamicCast((*readSeq)[8]);
    ::fwAtoms::Numeric::sptr  readNum2  = ::fwAtoms::Numeric::dynamicCast((*readSeq)[9]);
    ::fwAtoms::Boolean::sptr  readBoolT = ::fwAtoms::Boolean::dynamicCast((*readSeq)[10]);

    CPPUNIT_ASSERT( readObj0  );
    CPPUNIT_ASSERT( readObj1  );
    CPPUNIT_ASSERT( readBlob  );
    CPPUNIT_ASSERT( readMap   );
    CPPUNIT_ASSERT( readSeq2  );
    CPPUNIT_ASSERT( readBoolF );
    CPPUNIT_ASSERT( readNum   );
    CPPUNIT_ASSERT( readStr   );
    CPPUNIT_ASSERT( !readNull );
    CPPUNIT_ASSERT( readNum2  );
    CPPUNIT_ASSERT( readBoolT );

    CPPUNIT_ASSERT_EQUAL( readObj0, readObj1 );
    CPPUNIT_ASSERT_EQUAL( readSeq, readSeq2 );

    CPPUNIT_ASSERT_EQUAL( META_VALUE_0, readObj0->getMetaInfo(META_KEY_0) );
    CPPUNIT_ASSERT_EQUAL( META_VALUE_1, readObj0->getMetaInfo(META_KEY_1) );
    CPPUNIT_ASSERT_EQUAL( META_VALUE_2, readObj0->getMetaInfo(META_KEY_2) );
    CPPUNIT_ASSERT_EQUAL( META_VALUE_3, readObj0->getMetaInfo(META_KEY_3) );

    CPPUNIT_ASSERT_EQUAL( obj->getAttributes().size(), readObj0->getAttributes().size() );

    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(readBlob), readObj0->getAttribute(ATTR_NAME_0) );
    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(readMap ), readObj0->getAttribute(ATTR_NAME_1) );
    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(readStr ), readObj0->getAttribute(ATTR_NAME_2) );
    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(readNum ), readObj0->getAttribute(ATTR_NAME_3) );
    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(), readObj0->getAttribute(ATTR_NAME_4) );

    CPPUNIT_ASSERT_EQUAL( boolFalse->getValue(), readBoolF->getValue() );
    CPPUNIT_ASSERT_EQUAL( boolTrue->getValue(), readBoolT->getValue() );
    CPPUNIT_ASSERT_EQUAL( num->getString(), readNum->getString() );
    CPPUNIT_ASSERT_EQUAL( num2->getString(), readNum2->getString() );
    CPPUNIT_ASSERT_EQUAL( str->getValue(), readStr->getValue() );
}



}  // namespace ut
}  // namespace fwAtomsBoostIO


