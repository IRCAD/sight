/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "atoms/conversion/DataVisitor.hpp"

#include "atoms/conversion/convert.hpp"
#include "atoms/conversion/exception/ConversionNotManaged.hpp"
#include "atoms/conversion/mapper/factory/new.hpp"

#include <atoms/Base.hpp>
#include <atoms/Blob.hpp>
#include <atoms/Boolean.hpp>
#include <atoms/Map.hpp>
#include <atoms/Numeric.hpp>
#include <atoms/Numeric.hxx>
#include <atoms/Sequence.hpp>
#include <atoms/String.hpp>

#include <core/memory/BufferObject.hpp>
#include <core/reflection/factory/new.hpp>
#include <core/reflection/Mapper/ValueMapper.hpp>
#include <core/tools/UUID.hpp>

#include <data/Array.hpp>
#include <data/reflection/mapper.hpp>

#include <boost/utility/enable_if.hpp>

#include <camp/class.hpp>

#include <string>

namespace sight::atoms::conversion
{

const std::string DataVisitor::CLASSNAME_METAINFO = "CLASSNAME_METAINFO";
const std::string DataVisitor::ID_METAINFO        = "ID_METAINFO";

//-----------------------------------------------------------------------------

struct DataConversionValueVisitor : public ::camp::ValueVisitor< atoms::Base::sptr >
{
    DataVisitor::AtomCacheType& m_cache;

    DataConversionValueVisitor( DataVisitor::AtomCacheType& cache ) :
        m_cache(cache)
    {
    }

    //------------------------------------------------------------------------------

    atoms::Base::sptr operator()(camp::NoType value)
    {
        FW_RAISE_EXCEPTION( exception::ConversionNotManaged(
                                "Enter in void GetCampValueVisitor()(camp::NoType value) : case not managed" ) );
        atoms::Base::sptr val;
        return val;
    }

    //------------------------------------------------------------------------------

    atoms::Base::sptr operator()(bool value)
    {
        return atoms::Boolean::New( value );
    }

    //------------------------------------------------------------------------------

    atoms::Base::sptr operator()(long value)
    {
        return atoms::Numeric::New( value );
    }

    //------------------------------------------------------------------------------

    atoms::Base::sptr operator()(double value)
    {
        return atoms::Numeric::New( value );
    }

    //------------------------------------------------------------------------------

    atoms::Base::sptr operator()(std::string value)
    {
        return atoms::String::New( value );
    }

    //------------------------------------------------------------------------------

    atoms::Base::sptr operator()(const camp::EnumObject& value)
    {
        return atoms::String::New( value.name() );
    }

    //------------------------------------------------------------------------------

    atoms::Base::sptr operator()(const camp::UserObject& value)
    {
        atoms::Base::sptr baseObj;

        if ( value.pointer() )
        {
            DataVisitor::ClassnameType classname = value.call("classname").to<std::string>();

            if( classname == "core::memory::BufferObject" )
            {
                core::memory::BufferObject* ptr = value.get< core::memory::BufferObject* >();
                baseObj = atoms::Blob::New( ptr->getSptr() );
            }
            else
            {
                // get fwData object
                data::Object* ptr          = value.get< data::Object* >();
                data::Object::sptr dataObj = ptr->getSptr();

                baseObj = atoms::conversion::convert( dataObj, m_cache );
            }
        }

        return baseObj;
    }
};

//-----------------------------------------------------------------------------

DataVisitor::DataVisitor( data::Object::sptr dataObj, AtomCacheType& cache ) :
    m_campDataObj( dataObj.get() ),
    m_cache( cache )
{

    // Create atom object
    m_atomObj = atoms::Object::New();
    ClassnameType classname = m_campDataObj.call("classname").to<std::string>();
    m_atomObj->setMetaInfo( DataVisitor::CLASSNAME_METAINFO, classname );
    core::tools::UUID::UUIDType uuid = core::tools::UUID::get(dataObj);
    m_atomObj->setMetaInfo( DataVisitor::ID_METAINFO, uuid );
    m_cache[uuid] = m_atomObj;

    // Fill atom object with tag
    const camp::Class& metaclass = ::camp::classByName(classname);
    std::size_t tagCount         = metaclass.tagCount();
    for ( std::size_t i = 0; i < tagCount; ++i )
    {
        const ::camp::Value& tag = metaclass.tagId(i);
        const ::camp::Value& val = metaclass.tag(tag);
        m_atomObj->setMetaInfo( tag.to< std::string >(), val.to< std::string >() );
    }

}

//-----------------------------------------------------------------------------

DataVisitor::~DataVisitor()
{
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::SimpleProperty& property)
{
    const std::string& name( property.name() );
    const ::camp::Value& val( property.get( m_campDataObj ) );

    /// ACH Hack !
    if( val.type() != camp::intType )
    {
        DataConversionValueVisitor visitor(m_cache);
        atoms::Base::sptr atom = val.visit( visitor );
        m_atomObj->setAttribute( name, atom );
    }
    else
    {
        atoms::Base::sptr atom = atoms::Numeric::New( val.to<long>() );
        m_atomObj->setAttribute( name, atom );
    }
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::EnumProperty& property)
{
    const std::string& name( property.name() );
    const ::camp::Value& val( property.get( m_campDataObj ) );

    DataConversionValueVisitor visitor(m_cache);
    atoms::Base::sptr atom = val.visit( visitor );
    m_atomObj->setAttribute( name, atom );
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::MapProperty& property)
{
    const std::string name( property.name() );
    atoms::Map::sptr atom = atoms::Map::New();

    std::pair< ::camp::Value, ::camp::Value > value;
    atoms::Base::sptr valAtom;
    ::camp::Value first;
    ::camp::Value second;

    const size_t size = property.getSize( m_campDataObj );
    for (size_t index = 0; index < size; ++index)
    {
        value = property.getElement(m_campDataObj, index);

        first  = value.first;
        second = value.second;

        DataConversionValueVisitor valVisitor(m_cache);
        valAtom = second.visit( valVisitor );

        FW_RAISE_EXCEPTION_IF(
            exception::ConversionNotManaged("Not managed type for map key (only support string, int and real)"),
            first.type() != ::camp::stringType &&
            first.type() != ::camp::intType &&
            first.type() != ::camp::realType );
        atom->insert( first.to< std::string >(), valAtom );
    }

    m_atomObj->setAttribute( name, atom );
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::ArrayProperty& property)
{
    const std::string name( property.name() );
    atoms::Sequence::sptr atom = atoms::Sequence::New();

    ::camp::Value val;
    atoms::Base::sptr valAtom;
    const size_t size = property.size( m_campDataObj );
    for (size_t index = 0; index < size; ++index)
    {
        val = property.get( m_campDataObj, index );

        DataConversionValueVisitor visitor(m_cache);
        valAtom = val.visit( visitor );
        atom->push_back( valAtom );
    }

    m_atomObj->setAttribute( name, atom );
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::UserProperty& property)
{
    const std::string& name( property.name() );
    const ::camp::Value& val( property.get( m_campDataObj ) );

    DataConversionValueVisitor visitor(m_cache);
    atoms::Base::sptr atom = val.visit( visitor );
    m_atomObj->setAttribute( name, atom );

}

//-----------------------------------------------------------------------------

atoms::Object::sptr DataVisitor::getAtomObject() const
{
    return this->m_atomObj;
}

} // fwAtomConversion
