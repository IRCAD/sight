/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Exception.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Object.hpp>
#include <fwAtoms/Object.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwMemory/BufferObject.hpp>

#include <limits>

CPPUNIT_TEST_SUITE_REGISTRATION( ::fwAtoms::ut::AtomTest );

namespace fwAtoms
{
namespace ut
{



void AtomTest::setUp()
{
}

void AtomTest::tearDown()
{
}


void AtomTest::blobTest()
{
    ::fwAtoms::Blob::sptr blob = ::fwAtoms::Blob::New();

    CPPUNIT_ASSERT_THROW(blob->getString(), ::fwAtoms::Exception);

    ::fwMemory::BufferObject::sptr bo = ::fwMemory::BufferObject::New();
    blob->setBufferObject(bo);
    CPPUNIT_ASSERT_EQUAL(bo, blob->getBufferObject());
}


void AtomTest::mapTest()
{
    ::fwAtoms::Map::sptr map = ::fwAtoms::Map::New();
    CPPUNIT_ASSERT( map->empty() );

    CPPUNIT_ASSERT_THROW(map->getString(), ::fwAtoms::Exception);

    ::fwAtoms::Blob::sptr blob    = ::fwAtoms::Blob::New();
    ::fwAtoms::Object::sptr obj   = ::fwAtoms::Object::New();
    ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();
    map->insert("blob", blob);
    map->insert("object", obj);
    map->insert("sequence", seq);

    CPPUNIT_ASSERT_EQUAL( size_t(3), map->size() );

    ::fwAtoms::Map::sptr clone = ::fwAtoms::Map::dynamicCast(map->clone());
    CPPUNIT_ASSERT_EQUAL( size_t(3), clone->size() );

    map->clear();
    CPPUNIT_ASSERT_EQUAL( size_t(0), map->size() );
    CPPUNIT_ASSERT( map->empty() );

    CPPUNIT_ASSERT_EQUAL( size_t(3), clone->size() );

}


void AtomTest::objectTest()
{
    ::fwAtoms::Object::sptr obj = ::fwAtoms::Object::New();

    CPPUNIT_ASSERT_THROW(obj->getString(), ::fwAtoms::Exception);

    ::fwAtoms::Map::sptr map      = ::fwAtoms::Map::New();
    ::fwAtoms::Blob::sptr blob    = ::fwAtoms::Blob::New();
    ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();

    obj->setAttribute("blob", blob);
    obj->setAttribute("object", obj);
    obj->setAttribute("map", map);
    obj->setAttribute("sequence", seq);

    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(blob), obj->getAttribute("blob") );
    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(obj), obj->getAttribute("object") );
    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(map), obj->getAttribute("map") );
    CPPUNIT_ASSERT_EQUAL( ::fwAtoms::Base::sptr(seq), obj->getAttribute("sequence") );
    CPPUNIT_ASSERT_EQUAL( blob, obj->getAttribute< ::fwAtoms::Blob >("blob") );
    CPPUNIT_ASSERT_EQUAL( obj, obj->getAttribute< ::fwAtoms::Object >("object") );
    CPPUNIT_ASSERT_EQUAL( map, obj->getAttribute< ::fwAtoms::Map >("map") );
    CPPUNIT_ASSERT_EQUAL( seq, obj->getAttribute< ::fwAtoms::Sequence >("sequence") );

    CPPUNIT_ASSERT_EQUAL( size_t(4), obj->getAttributes().size() );
    CPPUNIT_ASSERT_EQUAL( size_t(1), obj->eraseAttribute("object") );
    CPPUNIT_ASSERT_EQUAL( size_t(3), obj->getAttributes().size() );

    obj->setMetaInfo("info 0", "A");
    obj->setMetaInfo("info 1", "B");
    obj->setMetaInfo("info 2", "C");
    obj->setMetaInfo("info 3", "D");

    CPPUNIT_ASSERT_EQUAL( std::string("A"), obj->getMetaInfo("info 0") );
    CPPUNIT_ASSERT_EQUAL( std::string("B"), obj->getMetaInfo("info 1") );
    CPPUNIT_ASSERT_EQUAL( std::string("C"), obj->getMetaInfo("info 2") );
    CPPUNIT_ASSERT_EQUAL( std::string("D"), obj->getMetaInfo("info 3") );

    CPPUNIT_ASSERT_EQUAL( size_t(4), obj->getMetaInfos().size() );
    CPPUNIT_ASSERT_EQUAL( size_t(1), obj->eraseMetaInfo("info 1") );
    CPPUNIT_ASSERT_EQUAL( size_t(3), obj->getMetaInfos().size() );

    ::fwAtoms::Object::sptr clone = ::fwAtoms::Object::dynamicCast(obj->clone());

    CPPUNIT_ASSERT( ::fwAtoms::Base::sptr(blob) != clone->getAttribute("blob") );
    CPPUNIT_ASSERT( ::fwAtoms::Base::sptr(map) != clone->getAttribute("map") );
    CPPUNIT_ASSERT( ::fwAtoms::Base::sptr(seq) != clone->getAttribute("sequence") );
    CPPUNIT_ASSERT_EQUAL( size_t(3), clone->getAttributes().size() );
    CPPUNIT_ASSERT_EQUAL( std::string("A"), clone->getMetaInfo("info 0") );
    CPPUNIT_ASSERT_EQUAL( std::string("C"), clone->getMetaInfo("info 2") );
    CPPUNIT_ASSERT_EQUAL( std::string("D"), clone->getMetaInfo("info 3") );
    CPPUNIT_ASSERT_EQUAL( size_t(3), clone->getMetaInfos().size() );



    obj->clearAttribute();
    CPPUNIT_ASSERT_EQUAL( size_t(0), obj->getAttributes().size() );
    obj->clearMetaInfo();
    CPPUNIT_ASSERT_EQUAL( size_t(0), obj->getMetaInfos().size() );

    CPPUNIT_ASSERT_EQUAL( size_t(3), clone->getAttributes().size() );
    CPPUNIT_ASSERT_EQUAL( size_t(3), clone->getMetaInfos().size() );
}


void AtomTest::sequenceTest()
{
    ::fwAtoms::Sequence::sptr seq = ::fwAtoms::Sequence::New();
    CPPUNIT_ASSERT( seq->empty() );

    CPPUNIT_ASSERT_THROW(seq->getString(), ::fwAtoms::Exception);

    seq->push_back(::fwAtoms::Blob::New());
    seq->push_back(::fwAtoms::Object::New());
    seq->push_back(::fwAtoms::Map::New());
    seq->push_back(::fwAtoms::Sequence::New());

    CPPUNIT_ASSERT_EQUAL( size_t(4), seq->size() );

    ::fwAtoms::Sequence::sptr clone = ::fwAtoms::Sequence::dynamicCast(seq->clone());

    CPPUNIT_ASSERT_EQUAL( size_t(4), clone->size() );


    ::fwAtoms::Sequence::ConstIteratorType iterSeq   = seq->begin();
    ::fwAtoms::Sequence::ConstIteratorType iterClone = clone->begin();

    CPPUNIT_ASSERT_EQUAL( (*(iterClone++))->type(), (*(iterSeq++))->type() );
    CPPUNIT_ASSERT_EQUAL( (*(iterClone++))->type(), (*(iterSeq++))->type() );
    CPPUNIT_ASSERT_EQUAL( (*(iterClone++))->type(), (*(iterSeq++))->type() );
    CPPUNIT_ASSERT_EQUAL( (*(iterClone++))->type(), (*(iterSeq++))->type() );

    CPPUNIT_ASSERT( seq->end() == iterSeq );
    CPPUNIT_ASSERT( clone->end() == iterClone );

    seq->clear();
    CPPUNIT_ASSERT_EQUAL( size_t(0), seq->size() );
    CPPUNIT_ASSERT( seq->empty() );

    CPPUNIT_ASSERT_EQUAL( size_t(4), clone->size() );
}



}  // namespace ut
}  // namespace fwAtoms

