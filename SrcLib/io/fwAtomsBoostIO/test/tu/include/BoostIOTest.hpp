/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include <fwAtomsBoostIO/types.hpp>
#include <fwAtomsBoostIO/Writer.hpp>

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwZip/IReadArchive.hpp>
#include <fwZip/IWriteArchive.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace fwAtomsBoostIO
{
namespace ut
{

class BoostIOTest :  public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE( BoostIOTest );
CPPUNIT_TEST( readWriteZipTest );
CPPUNIT_TEST( readWriteDirTest );
CPPUNIT_TEST_SUITE_END();

public:
    // interface
    void setUp();
    void tearDown();

    void readWriteZipTest();
    void readWriteDirTest();

    static void cleanSequence(::fwAtoms::Sequence& seq);

protected:
    struct SequenceGenerator
    {
        std::string META_KEY_0;
        std::string META_KEY_1;
        std::string META_KEY_2;
        std::string META_KEY_3;
        std::string META_VALUE_0;
        std::string META_VALUE_1;
        std::string META_VALUE_2;
        std::string META_VALUE_3;

        std::string ATTR_NAME_0;
        std::string ATTR_NAME_1;
        std::string ATTR_NAME_2;
        std::string ATTR_NAME_3;
        std::string ATTR_NAME_4;

        ::fwAtoms::Sequence::sptr m_seq;

        SequenceGenerator()
        {
            META_KEY_0   = "info.a<b>.c.0";
            META_KEY_1   = "info.a<b>.c.{1:d}";
            META_KEY_2   = "info.a<b>.c.2";
            META_KEY_3   = "info.a<b>.c.3";
            META_VALUE_0 = "A";
            META_VALUE_1 = "B";
            META_VALUE_2 = "C";
            META_VALUE_3 = "D";

            ATTR_NAME_0 = "m_blob";
            ATTR_NAME_1 = "m_map";
            ATTR_NAME_2 = "m_str";
            ATTR_NAME_3 = "m_num";
            ATTR_NAME_4 = "NULL";

            ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::New();
            ::fwAtoms::Map::sptr map    = ::fwAtoms::Map::New();
            ::fwAtoms::Blob::sptr blob  = ::fwAtoms::Blob::New();
            m_seq = ::fwAtoms::Sequence::New();

            ::fwMemory::BufferObject::sptr bo                  = ::fwMemory::BufferObject::New();
            const ::fwMemory::BufferObject::SizeType BUFF_SIZE = 1*1024*1024; // 1Mo
            bo->allocate(BUFF_SIZE);
            blob->setBufferObject(bo);

            ::fwMemory::BufferObject::Lock lock(bo->lock());
            void* v            = lock.getBuffer();
            std::uint8_t* buff = static_cast<std::uint8_t*>(v);
            for (size_t i = 0; i < BUFF_SIZE; ++i)
            {
                buff[i] = static_cast<std::uint8_t>(i%256);
            }

            ::fwAtoms::Boolean::sptr boolFalse = ::fwAtoms::Boolean::New(false);
            ::fwAtoms::Boolean::sptr boolTrue  = ::fwAtoms::Boolean::New(true);
            ::fwAtoms::Numeric::sptr num       = ::fwAtoms::Numeric::New(42);
            ::fwAtoms::Numeric::sptr num2      = ::fwAtoms::Numeric::New(16.64);
            ::fwAtoms::String::sptr str        = ::fwAtoms::String::New("MyStr");

            obj->setMetaInfo(META_KEY_0, META_VALUE_0);
            obj->setMetaInfo(META_KEY_1, META_VALUE_1);
            obj->setMetaInfo(META_KEY_2, META_VALUE_2);
            obj->setMetaInfo(META_KEY_3, META_VALUE_3);

            obj->setAttribute(ATTR_NAME_0, blob);
            obj->setAttribute(ATTR_NAME_1, map);
            obj->setAttribute(ATTR_NAME_2, str);
            obj->setAttribute(ATTR_NAME_3, num);
            obj->setAttribute(ATTR_NAME_4, ::fwAtoms::String::sptr());

            m_seq->push_back(obj);
            m_seq->push_back(obj);
            m_seq->push_back(blob);
            m_seq->push_back(map);
            m_seq->push_back(m_seq);
            m_seq->push_back(boolFalse);
            m_seq->push_back(num);
            m_seq->push_back(str);
            m_seq->push_back(::fwAtoms::String::sptr());
            m_seq->push_back(num2);
            m_seq->push_back(boolTrue);

            // tests the case of a a cache path hiting an object stored in a map
            ::fwAtoms::Map::sptr map2    = ::fwAtoms::Map::New();
            ::fwAtoms::Object::sptr obj2 = ::fwAtoms::Object::New();
            map2->insert("obj2 ref1", obj2);
            map2->insert("obj2 ref2", obj2);

            map->insert("sequence loop key", m_seq);
            map->insert("object one more ref", obj);
            map->insert("map2", map2);
            map->insert("blob key", ::fwAtoms::Blob::New());
            map->insert("object key", ::fwAtoms::Object::New());
            map->insert("sequence key", ::fwAtoms::String::New("map-str"));
        }

        virtual ~SequenceGenerator()
        {
            BoostIOTest::cleanSequence(*m_seq);
        }

        //------------------------------------------------------------------------------

        ::fwAtoms::Sequence::sptr getSequence()
        {
            return m_seq;
        }

        //------------------------------------------------------------------------------

        void compare(::fwAtoms::Sequence::sptr readSeq) const
        {
            ::fwAtoms::Object::sptr obj        = ::fwAtoms::Object::dynamicCast((*m_seq)[0]);
            ::fwAtoms::Blob::sptr blob         = ::fwAtoms::Blob::dynamicCast((*m_seq)[2]);
            ::fwAtoms::Boolean::sptr boolFalse = ::fwAtoms::Boolean::dynamicCast((*m_seq)[5]);
            ::fwAtoms::Numeric::sptr num       = ::fwAtoms::Numeric::dynamicCast((*m_seq)[6]);
            ::fwAtoms::String::sptr str        = ::fwAtoms::String::dynamicCast((*m_seq)[7]);
            ::fwAtoms::Numeric::sptr num2      = ::fwAtoms::Numeric::dynamicCast((*m_seq)[9]);
            ::fwAtoms::Boolean::sptr boolTrue  = ::fwAtoms::Boolean::dynamicCast((*m_seq)[10]);

            CPPUNIT_ASSERT_EQUAL(m_seq->size(), readSeq->size());

            ::fwAtoms::Object::sptr readObj0   = ::fwAtoms::Object::dynamicCast((*readSeq)[0]);
            ::fwAtoms::Object::sptr readObj1   = ::fwAtoms::Object::dynamicCast((*readSeq)[1]);
            ::fwAtoms::Blob::sptr readBlob     = ::fwAtoms::Blob::dynamicCast((*readSeq)[2]);
            ::fwAtoms::Map::sptr readMap       = ::fwAtoms::Map::dynamicCast((*readSeq)[3]);
            ::fwAtoms::Sequence::sptr readSeq2 = ::fwAtoms::Sequence::dynamicCast((*readSeq)[4]);
            ::fwAtoms::Boolean::sptr readBoolF = ::fwAtoms::Boolean::dynamicCast((*readSeq)[5]);
            ::fwAtoms::Numeric::sptr readNum   = ::fwAtoms::Numeric::dynamicCast((*readSeq)[6]);
            ::fwAtoms::String::sptr readStr    = ::fwAtoms::String::dynamicCast((*readSeq)[7]);
            ::fwAtoms::Base::sptr readNull     = ::fwAtoms::Base::dynamicCast((*readSeq)[8]);
            ::fwAtoms::Numeric::sptr readNum2  = ::fwAtoms::Numeric::dynamicCast((*readSeq)[9]);
            ::fwAtoms::Boolean::sptr readBoolT = ::fwAtoms::Boolean::dynamicCast((*readSeq)[10]);

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

            ::fwMemory::BufferObject::sptr bo     = blob->getBufferObject();
            ::fwMemory::BufferObject::sptr readBo = readBlob->getBufferObject();

            CPPUNIT_ASSERT( readBo );

            CPPUNIT_ASSERT_EQUAL( bo->getSize(),  readBo->getSize() );

            ::fwMemory::BufferObject::Lock lock(bo->lock());
            ::fwMemory::BufferObject::Lock readLock(readBo->lock());

            void* v        = lock.getBuffer();
            void* readV    = readLock.getBuffer();
            char* buff     = static_cast<char*>(v);
            char* readBuff = static_cast<char*>(readV);

            CPPUNIT_ASSERT(readBlob);
            for (size_t i = 0; i < bo->getSize(); ++i)
            {
                CPPUNIT_ASSERT_EQUAL( buff[i], readBuff[i] );
            }

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
    };

    void readProcess(SequenceGenerator& gen,
                     ::fwZip::IReadArchive::sptr readArchive,
                     const std::filesystem::path& rootFilename,
                     const ::fwAtomsBoostIO::FormatType& formatType = ::fwAtomsBoostIO::JSON);

    void writeProcess(SequenceGenerator& gen,
                      ::fwZip::IWriteArchive::sptr writeArchive,
                      const std::filesystem::path& rootFilename,
                      ::fwAtomsBoostIO::FormatType format );
};

}  // namespace ut
}  // namespace fwAtomsBoostIO
