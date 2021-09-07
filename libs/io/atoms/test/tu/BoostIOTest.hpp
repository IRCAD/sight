/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Object.hpp>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <io/atoms/types.hpp>
#include <io/atoms/Writer.hpp>
#include <io/zip/IReadArchive.hpp>
#include <io/zip/IWriteArchive.hpp>

#include <cppunit/extensions/HelperMacros.h>

namespace sight::io::atoms
{

namespace ut
{

class BoostIOTest : public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(BoostIOTest);
CPPUNIT_TEST(readWriteZipTest);
CPPUNIT_TEST(readWriteDirTest);
CPPUNIT_TEST_SUITE_END();

public:

    // interface
    void setUp();
    void tearDown();

    void readWriteZipTest();
    void readWriteDirTest();

    static void cleanSequence(sight::atoms::Sequence& seq);

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

        sight::atoms::Sequence::sptr m_seq;

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

            sight::atoms::Object::sptr obj = sight::atoms::Object::New();
            sight::atoms::Map::sptr map    = sight::atoms::Map::New();
            sight::atoms::Blob::sptr blob  = sight::atoms::Blob::New();
            m_seq = sight::atoms::Sequence::New();

            core::memory::BufferObject::sptr bo                  = core::memory::BufferObject::New();
            const core::memory::BufferObject::SizeType BUFF_SIZE = 1 * 1024 * 1024; // 1Mo
            bo->allocate(BUFF_SIZE);
            blob->setBufferObject(bo);

            core::memory::BufferObject::Lock lock(bo->lock());
            void* v            = lock.getBuffer();
            std::uint8_t* buff = static_cast<std::uint8_t*>(v);
            for(size_t i = 0 ; i < BUFF_SIZE ; ++i)
            {
                buff[i] = static_cast<std::uint8_t>(i % 256);
            }

            sight::atoms::Boolean::sptr boolFalse = sight::atoms::Boolean::New(false);
            sight::atoms::Boolean::sptr boolTrue  = sight::atoms::Boolean::New(true);
            sight::atoms::Numeric::sptr num       = sight::atoms::Numeric::New(42);
            sight::atoms::Numeric::sptr num2      = sight::atoms::Numeric::New(16.64);
            sight::atoms::String::sptr str        = sight::atoms::String::New("MyStr");

            obj->setMetaInfo(META_KEY_0, META_VALUE_0);
            obj->setMetaInfo(META_KEY_1, META_VALUE_1);
            obj->setMetaInfo(META_KEY_2, META_VALUE_2);
            obj->setMetaInfo(META_KEY_3, META_VALUE_3);

            obj->setAttribute(ATTR_NAME_0, blob);
            obj->setAttribute(ATTR_NAME_1, map);
            obj->setAttribute(ATTR_NAME_2, str);
            obj->setAttribute(ATTR_NAME_3, num);
            obj->setAttribute(ATTR_NAME_4, sight::atoms::String::sptr());

            m_seq->push_back(obj);
            m_seq->push_back(obj);
            m_seq->push_back(blob);
            m_seq->push_back(map);
            m_seq->push_back(m_seq);
            m_seq->push_back(boolFalse);
            m_seq->push_back(num);
            m_seq->push_back(str);
            m_seq->push_back(sight::atoms::String::sptr());
            m_seq->push_back(num2);
            m_seq->push_back(boolTrue);

            // tests the case of a a cache path hiting an object stored in a map
            sight::atoms::Map::sptr map2    = sight::atoms::Map::New();
            sight::atoms::Object::sptr obj2 = sight::atoms::Object::New();
            map2->insert("obj2 ref1", obj2);
            map2->insert("obj2 ref2", obj2);

            map->insert("sequence loop key", m_seq);
            map->insert("object one more ref", obj);
            map->insert("map2", map2);
            map->insert("blob key", sight::atoms::Blob::New());
            map->insert("object key", sight::atoms::Object::New());
            map->insert("sequence key", sight::atoms::String::New("map-str"));
        }

        virtual ~SequenceGenerator()
        {
            BoostIOTest::cleanSequence(*m_seq);
        }

        //------------------------------------------------------------------------------

        sight::atoms::Sequence::sptr getSequence()
        {
            return m_seq;
        }

        //------------------------------------------------------------------------------

        void compare(sight::atoms::Sequence::sptr readSeq) const
        {
            sight::atoms::Object::sptr obj        = sight::atoms::Object::dynamicCast((*m_seq)[0]);
            sight::atoms::Blob::sptr blob         = sight::atoms::Blob::dynamicCast((*m_seq)[2]);
            sight::atoms::Boolean::sptr boolFalse = sight::atoms::Boolean::dynamicCast((*m_seq)[5]);
            sight::atoms::Numeric::sptr num       = sight::atoms::Numeric::dynamicCast((*m_seq)[6]);
            sight::atoms::String::sptr str        = sight::atoms::String::dynamicCast((*m_seq)[7]);
            sight::atoms::Numeric::sptr num2      = sight::atoms::Numeric::dynamicCast((*m_seq)[9]);
            sight::atoms::Boolean::sptr boolTrue  = sight::atoms::Boolean::dynamicCast((*m_seq)[10]);

            CPPUNIT_ASSERT_EQUAL(m_seq->size(), readSeq->size());

            sight::atoms::Object::sptr readObj0   = sight::atoms::Object::dynamicCast((*readSeq)[0]);
            sight::atoms::Object::sptr readObj1   = sight::atoms::Object::dynamicCast((*readSeq)[1]);
            sight::atoms::Blob::sptr readBlob     = sight::atoms::Blob::dynamicCast((*readSeq)[2]);
            sight::atoms::Map::sptr readMap       = sight::atoms::Map::dynamicCast((*readSeq)[3]);
            sight::atoms::Sequence::sptr readSeq2 = sight::atoms::Sequence::dynamicCast((*readSeq)[4]);
            sight::atoms::Boolean::sptr readBoolF = sight::atoms::Boolean::dynamicCast((*readSeq)[5]);
            sight::atoms::Numeric::sptr readNum   = sight::atoms::Numeric::dynamicCast((*readSeq)[6]);
            sight::atoms::String::sptr readStr    = sight::atoms::String::dynamicCast((*readSeq)[7]);
            sight::atoms::Base::sptr readNull     = sight::atoms::Base::dynamicCast((*readSeq)[8]);
            sight::atoms::Numeric::sptr readNum2  = sight::atoms::Numeric::dynamicCast((*readSeq)[9]);
            sight::atoms::Boolean::sptr readBoolT = sight::atoms::Boolean::dynamicCast((*readSeq)[10]);

            CPPUNIT_ASSERT(readObj0);
            CPPUNIT_ASSERT(readObj1);
            CPPUNIT_ASSERT(readBlob);
            CPPUNIT_ASSERT(readMap);
            CPPUNIT_ASSERT(readSeq2);
            CPPUNIT_ASSERT(readBoolF);
            CPPUNIT_ASSERT(readNum);
            CPPUNIT_ASSERT(readStr);
            CPPUNIT_ASSERT(!readNull);
            CPPUNIT_ASSERT(readNum2);
            CPPUNIT_ASSERT(readBoolT);

            core::memory::BufferObject::sptr bo     = blob->getBufferObject();
            core::memory::BufferObject::sptr readBo = readBlob->getBufferObject();

            CPPUNIT_ASSERT(readBo);

            CPPUNIT_ASSERT_EQUAL(bo->getSize(), readBo->getSize());

            core::memory::BufferObject::Lock lock(bo->lock());
            core::memory::BufferObject::Lock readLock(readBo->lock());

            void* v        = lock.getBuffer();
            void* readV    = readLock.getBuffer();
            char* buff     = static_cast<char*>(v);
            char* readBuff = static_cast<char*>(readV);

            CPPUNIT_ASSERT(readBlob);
            for(size_t i = 0 ; i < bo->getSize() ; ++i)
            {
                CPPUNIT_ASSERT_EQUAL(buff[i], readBuff[i]);
            }

            CPPUNIT_ASSERT_EQUAL(readObj0, readObj1);
            CPPUNIT_ASSERT_EQUAL(readSeq, readSeq2);

            CPPUNIT_ASSERT_EQUAL(META_VALUE_0, readObj0->getMetaInfo(META_KEY_0));
            CPPUNIT_ASSERT_EQUAL(META_VALUE_1, readObj0->getMetaInfo(META_KEY_1));
            CPPUNIT_ASSERT_EQUAL(META_VALUE_2, readObj0->getMetaInfo(META_KEY_2));
            CPPUNIT_ASSERT_EQUAL(META_VALUE_3, readObj0->getMetaInfo(META_KEY_3));

            CPPUNIT_ASSERT_EQUAL(obj->getAttributes().size(), readObj0->getAttributes().size());

            CPPUNIT_ASSERT_EQUAL(sight::atoms::Base::sptr(readBlob), readObj0->getAttribute(ATTR_NAME_0));
            CPPUNIT_ASSERT_EQUAL(sight::atoms::Base::sptr(readMap), readObj0->getAttribute(ATTR_NAME_1));
            CPPUNIT_ASSERT_EQUAL(sight::atoms::Base::sptr(readStr), readObj0->getAttribute(ATTR_NAME_2));
            CPPUNIT_ASSERT_EQUAL(sight::atoms::Base::sptr(readNum), readObj0->getAttribute(ATTR_NAME_3));
            CPPUNIT_ASSERT_EQUAL(sight::atoms::Base::sptr(), readObj0->getAttribute(ATTR_NAME_4));

            CPPUNIT_ASSERT_EQUAL(boolFalse->getValue(), readBoolF->getValue());
            CPPUNIT_ASSERT_EQUAL(boolTrue->getValue(), readBoolT->getValue());
            CPPUNIT_ASSERT_EQUAL(num->getString(), readNum->getString());
            CPPUNIT_ASSERT_EQUAL(num2->getString(), readNum2->getString());
            CPPUNIT_ASSERT_EQUAL(str->getValue(), readStr->getValue());
        }
    };

    void readProcess(
        SequenceGenerator& gen,
        io::zip::IReadArchive::sptr readArchive,
        const std::filesystem::path& rootFilename,
        const io::atoms::FormatType& formatType = io::atoms::JSON
    );

    void writeProcess(
        SequenceGenerator& gen,
        io::zip::IWriteArchive::sptr writeArchive,
        const std::filesystem::path& rootFilename,
        io::atoms::FormatType format
    );
};

} // namespace ut

} // namespace sight::io::atoms
