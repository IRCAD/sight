/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "fwAtomConversion/DataVisitor.hpp"

#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/exception/ConversionNotManaged.hpp"
#include "fwAtomConversion/mapper/factory/new.hpp"

#include <fwAtoms/Base.hpp>
#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/String.hpp>

#include <fwCamp/Mapper/ValueMapper.hpp>
#include <fwCamp/factory/new.hpp>

#include <fwData/Array.hpp>
#include <fwData/camp/mapper.hpp>

#include <fwDataCamp/Version.hpp>

#include <fwMedDataCamp/Version.hpp>

#include <fwMemory/BufferObject.hpp>

#include <fwTools/UUID.hpp>

#include <boost/utility/enable_if.hpp>

#include <camp/class.hpp>

#include <string>


namespace fwAtomConversion
{

static int dataCampVersion    = ::fwDataCamp::Version::s_CURRENT_VERSION; // Hack to force link with fwDataCamp
static int medDataCampVersion = ::fwMedDataCamp::Version::s_CURRENT_VERSION; // Hack to force link with fwMedDataCamp

const std::string DataVisitor::CLASSNAME_METAINFO = "CLASSNAME_METAINFO";
const std::string DataVisitor::ID_METAINFO        = "ID_METAINFO";

//-----------------------------------------------------------------------------

struct DataConversionValueVisitor : public ::camp::ValueVisitor< ::fwAtoms::Base::sptr >
{
    DataVisitor::AtomCacheType& m_cache;

    DataConversionValueVisitor( DataVisitor::AtomCacheType& cache ) : m_cache(cache)
    {
    }

    ::fwAtoms::Base::sptr operator()(camp::NoType value)
    {
        FW_RAISE_EXCEPTION( exception::ConversionNotManaged(
                                "Enter in void GetCampValueVisitor()(camp::NoType value) : case not managed" ) );
        ::fwAtoms::Base::sptr val;
        return val;
    }

    ::fwAtoms::Base::sptr operator()(bool value)
    {
        return ::fwAtoms::Boolean::New( value );
    }

    ::fwAtoms::Base::sptr operator()(long value)
    {
        return ::fwAtoms::Numeric::New( value );
    }

    ::fwAtoms::Base::sptr operator()(double value)
    {
        return ::fwAtoms::Numeric::New( value );
    }

    ::fwAtoms::Base::sptr operator()(std::string value)
    {
        return ::fwAtoms::String::New( value );
    }

    ::fwAtoms::Base::sptr operator()(const camp::EnumObject& value)
    {
        return ::fwAtoms::String::New( value.name() );
    }

    ::fwAtoms::Base::sptr operator()(const camp::UserObject& value)
    {
        ::fwAtoms::Base::sptr baseObj;

        if ( value.pointer() )
        {
            DataVisitor::ClassnameType classname = value.call("classname").to<std::string>();

            if( classname == "::fwMemory::BufferObject" )
            {
                ::fwMemory::BufferObject* ptr = value.get< ::fwMemory::BufferObject* >();
                baseObj                       = ::fwAtoms::Blob::New( ptr->getSptr() );
            }
            else
            {
                // get fwData object
                ::fwData::Object* ptr          = value.get< ::fwData::Object* >();
                ::fwData::Object::sptr dataObj = ptr->getSptr();

                baseObj = ::fwAtomConversion::convert( dataObj, m_cache );
            }
        }

        return baseObj;
    }
};

//-----------------------------------------------------------------------------

DataVisitor::DataVisitor( ::fwData::Object::sptr dataObj, AtomCacheType& cache )
    : m_campDataObj( dataObj.get() ), m_cache( cache )
{

    // Create atom object
    m_atomObj = ::fwAtoms::Object::New();
    ClassnameType classname = m_campDataObj.call("classname").to<std::string>();
    m_atomObj->setMetaInfo( DataVisitor::CLASSNAME_METAINFO, classname );
    ::fwTools::UUID::UUIDType uuid = ::fwTools::UUID::get(dataObj);
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
    const std::string& name ( property.name() );
    const ::camp::Value& val ( property.get( m_campDataObj ) );

    /// ACH Hack !
    if( val.type() != camp::intType )
    {
        DataConversionValueVisitor visitor(m_cache);
        ::fwAtoms::Base::sptr atom = val.visit( visitor );
        m_atomObj->setAttribute( name, atom );
    }
    else
    {
        ::fwAtoms::Base::sptr atom = ::fwAtoms::Numeric::New( val.to<long>() );
        m_atomObj->setAttribute( name, atom );
    }
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::EnumProperty& property)
{
    const std::string& name ( property.name() );
    const ::camp::Value& val ( property.get( m_campDataObj ) );

    DataConversionValueVisitor visitor(m_cache);
    ::fwAtoms::Base::sptr atom = val.visit( visitor );
    m_atomObj->setAttribute( name, atom );
}

//-----------------------------------------------------------------------------

void DataVisitor::visit(const camp::MapProperty& property)
{
    const std::string name ( property.name() );
    ::fwAtoms::Map::sptr atom = ::fwAtoms::Map::New();

    std::pair< ::camp::Value, ::camp::Value > value;
    ::fwAtoms::Base::sptr valAtom;
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
    const std::string name ( property.name() );
    ::fwAtoms::Sequence::sptr atom = ::fwAtoms::Sequence::New();

    ::camp::Value val;
    ::fwAtoms::Base::sptr valAtom;
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
    const std::string& name ( property.name() );
    const ::camp::Value& val ( property.get( m_campDataObj ) );

    DataConversionValueVisitor visitor(m_cache);
    ::fwAtoms::Base::sptr atom = val.visit( visitor );
    m_atomObj->setAttribute( name, atom );

}

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr DataVisitor::getAtomObject() const
{
    return this->m_atomObj;
}

} // fwAtomConversion
