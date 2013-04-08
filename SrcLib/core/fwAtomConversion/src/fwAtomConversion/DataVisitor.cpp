/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include <boost/shared_ptr.hpp>
#include <boost/utility/enable_if.hpp>

#include <camp/class.hpp>

#include <fwCamp/factory/new.hpp>
#include <fwCamp/Mapper/ValueMapper.hpp>

#include <fwData/camp/mapper.hpp>
#include <fwData/Array.hpp>

#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/String.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Base.hpp>

#include <fwTools/UUID.hpp>
#include <fwTools/BufferObject.hpp>

#include <fwDataCamp/Version.hpp>

#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/mapper/factory/new.hpp"
#include "fwAtomConversion/convert.hpp"


namespace fwAtomConversion
{

static int dataCampVersion = ::fwDataCamp::Version::s_CURRENT_VERSION; // Hack to force link with fwDataCamp

const std::string DataVisitor::CLASSNAME_METAINFO = "CLASSNAME_METAINFO";

//-----------------------------------------------------------------------------

struct DataConversionValueVisitor : public ::camp::ValueVisitor< ::fwAtoms::Base::sptr >
{
    DataVisitor::AtomCacheType & m_cache;

    DataConversionValueVisitor( DataVisitor::AtomCacheType & cache ) : m_cache(cache)
    {}

    ::fwAtoms::Base::sptr operator()(camp::NoType value)
    {
        SLM_FATAL("Enter in void GetCampValueVisitor()(camp::NoType value) : case not managed");
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

            if( classname == "::fwTools::BufferObject" )
            {
                ::fwTools::BufferObject * ptr = value.get< ::fwTools::BufferObject * >();
                baseObj = ::fwAtoms::Blob::New( ptr->getSptr() );
            }
            else
            {
                // get fwData object
                ::fwData::Object * ptr = value.get< ::fwData::Object * >();
                ::fwData::Object::sptr dataObj = ptr->getSptr();

                baseObj = ::fwAtomConversion::convert( dataObj, m_cache );
            }
        }

        return baseObj;
    }
 };

//-----------------------------------------------------------------------------

DataVisitor::DataVisitor( ::fwData::Object::sptr dataObj, AtomCacheType & cache )
: m_campDataObj( dataObj.get() ), m_cache( cache )
{

    // Create atom object
    m_atomObj = ::fwAtoms::Object::New();
    ClassnameType classname = m_campDataObj.call("classname").to<std::string>();
    m_atomObj->setMetaInfo( DataVisitor::CLASSNAME_METAINFO, classname );
    ::fwTools::UUID::UUIDType uuid = ::fwTools::UUID::get(dataObj);
    m_atomObj->setId( uuid );
    m_cache[uuid] = m_atomObj;

    // Fill atom object with tag
    const camp::Class& metaclass = ::camp::classByName(classname);
    std::size_t tagCount = metaclass.tagCount();
    for ( std::size_t i = 0; i < tagCount; ++i )
    {
        const ::camp::Value & tag = metaclass.tagId(i);
        const ::camp::Value & val = metaclass.tag(tag);
        m_atomObj->setMetaInfo( tag.to< std::string >() , val.to< std::string >() );
    }

}

//-----------------------------------------------------------------------------

DataVisitor::~DataVisitor()
{}

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

        first = value.first;
        second = value.second;

        DataConversionValueVisitor valVisitor(m_cache);
        valAtom = second.visit( valVisitor );

        SLM_ASSERT("Not managed type for map key.",
                   first.type() == ::camp::stringType ||
                   first.type() == ::camp::intType ||
                   first.type() == ::camp::realType);
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

void DataVisitor::visit(const camp::Function& function)
{
    SLM_DEBUG("function visitation not implemented");
}

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr DataVisitor::getAtomObject() const
{
    return this->m_atomObj;
}

} // fwAtomConversion
