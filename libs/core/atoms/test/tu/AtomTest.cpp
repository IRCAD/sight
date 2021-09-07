/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include "AtomTest.hpp"

#include <atoms/Blob.hpp>
#include <atoms/Exception.hpp>
#include <atoms/Map.hpp>
#include <atoms/Object.hpp>
#include <atoms/Object.hxx>
#include <atoms/Sequence.hpp>

#include <core/memory/BufferObject.hpp>

#include <limits>

CPPUNIT_TEST_SUITE_REGISTRATION(sight::atoms::ut::AtomTest);

namespace sight::atoms
{

namespace ut
{

//------------------------------------------------------------------------------

void AtomTest::setUp()
{
}

//------------------------------------------------------------------------------

void AtomTest::tearDown()
{
}

//------------------------------------------------------------------------------

void AtomTest::blobTest()
{
    atoms::Blob::sptr blob = atoms::Blob::New();

    CPPUNIT_ASSERT_THROW(blob->getString(), atoms::Exception);

    core::memory::BufferObject::sptr bo = core::memory::BufferObject::New();
    blob->setBufferObject(bo);
    CPPUNIT_ASSERT_EQUAL(bo, blob->getBufferObject());
}

//------------------------------------------------------------------------------

void AtomTest::mapTest()
{
    atoms::Map::sptr map = atoms::Map::New();
    CPPUNIT_ASSERT(map->empty());

    CPPUNIT_ASSERT_THROW(map->getString(), atoms::Exception);

    atoms::Blob::sptr blob    = atoms::Blob::New();
    atoms::Object::sptr obj   = atoms::Object::New();
    atoms::Sequence::sptr seq = atoms::Sequence::New();
    map->insert("blob", blob);
    map->insert("object", obj);
    map->insert("sequence", seq);

    CPPUNIT_ASSERT_EQUAL(size_t(3), map->size());

    atoms::Map::sptr clone = atoms::Map::dynamicCast(map->clone());
    CPPUNIT_ASSERT_EQUAL(size_t(3), clone->size());

    map->clear();
    CPPUNIT_ASSERT_EQUAL(size_t(0), map->size());
    CPPUNIT_ASSERT(map->empty());

    CPPUNIT_ASSERT_EQUAL(size_t(3), clone->size());
}

//------------------------------------------------------------------------------

void AtomTest::objectTest()
{
    atoms::Object::sptr obj = atoms::Object::New();

    CPPUNIT_ASSERT_THROW(obj->getString(), atoms::Exception);

    atoms::Map::sptr map      = atoms::Map::New();
    atoms::Blob::sptr blob    = atoms::Blob::New();
    atoms::Sequence::sptr seq = atoms::Sequence::New();

    obj->setAttribute("blob", blob);
    obj->setAttribute("object", obj);
    obj->setAttribute("map", map);
    obj->setAttribute("sequence", seq);

    CPPUNIT_ASSERT_EQUAL(atoms::Base::sptr(blob), obj->getAttribute("blob"));
    CPPUNIT_ASSERT_EQUAL(atoms::Base::sptr(obj), obj->getAttribute("object"));
    CPPUNIT_ASSERT_EQUAL(atoms::Base::sptr(map), obj->getAttribute("map"));
    CPPUNIT_ASSERT_EQUAL(atoms::Base::sptr(seq), obj->getAttribute("sequence"));
    CPPUNIT_ASSERT_EQUAL(blob, obj->getAttribute<atoms::Blob>("blob"));
    CPPUNIT_ASSERT_EQUAL(obj, obj->getAttribute<atoms::Object>("object"));
    CPPUNIT_ASSERT_EQUAL(map, obj->getAttribute<atoms::Map>("map"));
    CPPUNIT_ASSERT_EQUAL(seq, obj->getAttribute<atoms::Sequence>("sequence"));

    CPPUNIT_ASSERT_EQUAL(size_t(4), obj->getAttributes().size());
    CPPUNIT_ASSERT_EQUAL(size_t(1), obj->eraseAttribute("object"));
    CPPUNIT_ASSERT_EQUAL(size_t(3), obj->getAttributes().size());

    obj->setMetaInfo("info 0", "A");
    obj->setMetaInfo("info 1", "B");
    obj->setMetaInfo("info 2", "C");
    obj->setMetaInfo("info 3", "D");

    CPPUNIT_ASSERT_EQUAL(std::string("A"), obj->getMetaInfo("info 0"));
    CPPUNIT_ASSERT_EQUAL(std::string("B"), obj->getMetaInfo("info 1"));
    CPPUNIT_ASSERT_EQUAL(std::string("C"), obj->getMetaInfo("info 2"));
    CPPUNIT_ASSERT_EQUAL(std::string("D"), obj->getMetaInfo("info 3"));

    CPPUNIT_ASSERT_EQUAL(size_t(4), obj->getMetaInfos().size());
    CPPUNIT_ASSERT_EQUAL(size_t(1), obj->eraseMetaInfo("info 1"));
    CPPUNIT_ASSERT_EQUAL(size_t(3), obj->getMetaInfos().size());

    atoms::Object::sptr clone = atoms::Object::dynamicCast(obj->clone());

    CPPUNIT_ASSERT(atoms::Base::sptr(blob) != clone->getAttribute("blob"));
    CPPUNIT_ASSERT(atoms::Base::sptr(map) != clone->getAttribute("map"));
    CPPUNIT_ASSERT(atoms::Base::sptr(seq) != clone->getAttribute("sequence"));
    CPPUNIT_ASSERT_EQUAL(size_t(3), clone->getAttributes().size());
    CPPUNIT_ASSERT_EQUAL(std::string("A"), clone->getMetaInfo("info 0"));
    CPPUNIT_ASSERT_EQUAL(std::string("C"), clone->getMetaInfo("info 2"));
    CPPUNIT_ASSERT_EQUAL(std::string("D"), clone->getMetaInfo("info 3"));
    CPPUNIT_ASSERT_EQUAL(size_t(3), clone->getMetaInfos().size());

    obj->clearAttribute();
    CPPUNIT_ASSERT_EQUAL(size_t(0), obj->getAttributes().size());
    obj->clearMetaInfo();
    CPPUNIT_ASSERT_EQUAL(size_t(0), obj->getMetaInfos().size());

    CPPUNIT_ASSERT_EQUAL(size_t(3), clone->getAttributes().size());
    CPPUNIT_ASSERT_EQUAL(size_t(3), clone->getMetaInfos().size());
}

//------------------------------------------------------------------------------

void AtomTest::sequenceTest()
{
    atoms::Sequence::sptr seq = atoms::Sequence::New();
    CPPUNIT_ASSERT(seq->empty());

    CPPUNIT_ASSERT_THROW(seq->getString(), atoms::Exception);

    seq->push_back(atoms::Blob::New());
    seq->push_back(atoms::Object::New());
    seq->push_back(atoms::Map::New());
    seq->push_back(atoms::Sequence::New());

    CPPUNIT_ASSERT_EQUAL(size_t(4), seq->size());

    atoms::Sequence::sptr clone = atoms::Sequence::dynamicCast(seq->clone());

    CPPUNIT_ASSERT_EQUAL(size_t(4), clone->size());

    atoms::Sequence::ConstIteratorType iterSeq   = seq->begin();
    atoms::Sequence::ConstIteratorType iterClone = clone->begin();

    CPPUNIT_ASSERT_EQUAL((*(iterClone++))->type(), (*(iterSeq++))->type());
    CPPUNIT_ASSERT_EQUAL((*(iterClone++))->type(), (*(iterSeq++))->type());
    CPPUNIT_ASSERT_EQUAL((*(iterClone++))->type(), (*(iterSeq++))->type());
    CPPUNIT_ASSERT_EQUAL((*(iterClone++))->type(), (*(iterSeq++))->type());

    CPPUNIT_ASSERT(seq->end() == iterSeq);
    CPPUNIT_ASSERT(clone->end() == iterClone);

    seq->clear();
    CPPUNIT_ASSERT_EQUAL(size_t(0), seq->size());
    CPPUNIT_ASSERT(seq->empty());

    CPPUNIT_ASSERT_EQUAL(size_t(4), clone->size());
}

} // namespace ut

} // namespace sight::atoms
