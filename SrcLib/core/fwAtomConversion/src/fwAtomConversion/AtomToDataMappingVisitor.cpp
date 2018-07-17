/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtomConversion/AtomToDataMappingVisitor.hpp"

#include "fwAtomConversion/camp_ext/ValueMapper.hpp"
#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/exception/ConversionNotManaged.hpp"
#include "fwAtomConversion/mapper/Base.hpp"

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Boolean.hpp>
#include <fwAtoms/Map.hpp>
#include <fwAtoms/Numeric.hpp>
#include <fwAtoms/Numeric.hxx>
#include <fwAtoms/Sequence.hpp>

#include <fwCamp/factory/new.hpp>

#include <fwData/Object.hpp>

#include <fwMemory/BufferObject.hpp>

namespace fwAtomConversion
{

//-----------------------------------------------------------------------------

class NumericSimplePropertyVisitor : public boost::static_visitor<void>
{
public:

    ::fwAtoms::Numeric::sptr m_typedAtom;
    ::camp::UserObject& m_campDataObj;
    const camp::SimpleProperty& m_property;

    NumericSimplePropertyVisitor( const ::fwAtoms::Numeric::sptr& typedAtom,
                                  ::camp::UserObject& campDataObj,
                                  const camp::SimpleProperty& property ) :
        m_typedAtom(typedAtom),
        m_campDataObj(campDataObj),
        m_property(property)
    {
    }

    //------------------------------------------------------------------------------

    void operator()( ::boost::blank& ) const
    {
        m_property.set( m_campDataObj, m_typedAtom->getString() );
    }

    //------------------------------------------------------------------------------

    void operator()( const ::boost::blank& ) const
    {
        m_property.set( m_campDataObj, m_typedAtom->getString() );
    }

    //------------------------------------------------------------------------------

    template <typename U>
    void operator()( U& value ) const
    {
        m_property.set( m_campDataObj, value );
    }
};

//-----------------------------------------------------------------------------

class NumericArrayPropertyVisitor : public boost::static_visitor<void>
{
public:

    ::fwAtoms::Numeric::sptr m_typedAtom;
    ::camp::UserObject& m_campDataObj;
    const camp::ArrayProperty& m_property;
    unsigned int m_index;

    NumericArrayPropertyVisitor( const ::fwAtoms::Numeric::sptr& typedAtom,
                                 ::camp::UserObject& campDataObj,
                                 const camp::ArrayProperty& property,
                                 unsigned int index ) :
        m_typedAtom(typedAtom),
        m_campDataObj(campDataObj),
        m_property(property),
        m_index(index)
    {
    }

    //------------------------------------------------------------------------------

    void operator()( ::boost::blank& ) const
    {
        if( m_property.dynamic() )
        {
            m_property.insert( m_campDataObj, m_index, m_typedAtom->getString() );
        }
        else
        {
            m_property.set( m_campDataObj, m_index, m_typedAtom->getString() );
        }
    }

    //------------------------------------------------------------------------------

    void operator()( const ::boost::blank& ) const
    {
        if( m_property.dynamic() )
        {
            m_property.insert( m_campDataObj, m_index, m_typedAtom->getString() );
        }
        else
        {
            m_property.set( m_campDataObj, m_index, m_typedAtom->getString() );
        }
    }

    //------------------------------------------------------------------------------

    template <typename U>
    void operator()( U& value ) const
    {
        if( m_property.dynamic() )
        {
            m_property.insert( m_campDataObj, m_index, value );
        }
        else
        {
            m_property.set( m_campDataObj, m_index, value );
        }
    }
};

//-----------------------------------------------------------------------------
class NumericMapPropertyVisitor : public boost::static_visitor<void>
{
public:

    ::fwAtoms::Numeric::sptr m_typedAtom;
    ::camp::UserObject& m_campDataObj;
    const camp::MapProperty& m_property;
    std::string m_key;

    NumericMapPropertyVisitor( const ::fwAtoms::Numeric::sptr& typedAtom,
                               ::camp::UserObject& campDataObj,
                               const camp::MapProperty& property,
                               const std::string& key ) :
        m_typedAtom(typedAtom),
        m_campDataObj(campDataObj),
        m_property(property),
        m_key(key)
    {
    }

    //------------------------------------------------------------------------------

    void operator()( ::boost::blank& ) const
    {
        m_property.set( m_campDataObj, m_key, m_typedAtom->getString() );
    }

    //------------------------------------------------------------------------------

    void operator()( const ::boost::blank& ) const
    {
        m_property.set( m_campDataObj, m_key, m_typedAtom->getString() );
    }

    //------------------------------------------------------------------------------

    template <typename U>
    void operator()( U& value ) const
    {
        m_property.set( m_campDataObj, m_key, value );
    }
};

//-----------------------------------------------------------------------------

AtomToDataMappingVisitor::AtomToDataMappingVisitor(
    ::fwData::Object::sptr dataObj,
    ::fwAtoms::Object::sptr atomObj,
    AtomVisitor::DataCacheType& cache,
    const AtomVisitor::IReadPolicy& uuidPolicy) :
    m_dataObj(dataObj),
    m_campDataObj( m_dataObj.get() ),
    m_atomObj(atomObj),
    m_cache(cache),
    m_uuidPolicy(uuidPolicy)
{
}

AtomToDataMappingVisitor::~AtomToDataMappingVisitor()
{
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::SimpleProperty& property)
{
    const std::string& name( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( msg.str() ), !atom );

    switch( atom->type() )
    {
        case ::fwAtoms::Base::NUMERIC:
        {
            ::fwAtoms::Numeric::sptr typedAtom = ::fwAtoms::Numeric::dynamicCast( atom );
            ::boost::apply_visitor( NumericSimplePropertyVisitor(typedAtom, m_campDataObj, property),
                                    typedAtom->getVariant() );
            break;
        }
        default:
        {
            property.set( m_campDataObj, atom->getString() );
            break;
        }
    }

}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::EnumProperty& property)
{
    const std::string& name( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( msg.str() ), !atom );

    property.set( m_campDataObj, atom->getString() );
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::UserProperty& property)
{
    const std::string& name( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );
    if ( atom ) // attribute not exist if was a null object sptr
    {
        switch( atom->type() )
        {
            case ::fwAtoms::Base::BLOB:
            {
                ::fwAtoms::Blob::sptr blobAtom = ::fwAtoms::Blob::dynamicCast(atom);
                ::fwMemory::BufferObject::sptr buffer;
                buffer = ::camp_ext::ValueMapper< ::fwAtoms::Blob::sptr >::to(blobAtom);
                FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                           "A blob cannot contain a null buffer pointer"),
                                       !buffer );
                property.set( m_campDataObj, ::camp::UserObject(buffer) );
                break;
            }
            case ::fwAtoms::Base::OBJECT:
            {
                ::fwAtoms::Object::sptr objectAtom = ::fwAtoms::Object::dynamicCast(atom);
                ::fwData::Object::sptr objectData  = ::fwAtomConversion::convert( objectAtom, m_cache, m_uuidPolicy );
                property.set( m_campDataObj, objectData );
                break;
            }
            default:
            {
                std::stringstream msg;
                msg << "Attribute of type '";
                switch( atom->type() )
                {
                    case ::fwAtoms::Base::BOOLEAN:
                        msg << "BOOLEAN";
                        break;
                    case ::fwAtoms::Base::STRING:
                        msg << "STRING";
                        break;
                    case ::fwAtoms::Base::NUMERIC:
                        msg << "NUMERIC";
                        break;
                    case ::fwAtoms::Base::MAP:
                        msg << "MAP";
                        break;
                    case ::fwAtoms::Base::SEQUENCE:
                        msg << "SEQUENCE";
                        break;
                    default:
                        break;
                }
                msg <<"', are not supported in the data conversion process.";
                FW_RAISE_EXCEPTION( exception::ConversionNotManaged(msg.str()) );
                break;
            }
        }
    }
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::ArrayProperty& property)
{
    const std::string& name( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( msg.str() ), !atom );

    ::fwAtoms::Sequence::sptr seqAtom = ::fwAtoms::Sequence::dynamicCast(atom);
    unsigned int index = 0;
    for( ::fwAtoms::Base::sptr elemAtom :  seqAtom->getValue() )
    {
        if (!elemAtom)
        {
            FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( "Not supported null element in Atom sequence." ),
                                   property.elementType() != ::camp::userType );

            ::fwData::Object::sptr objectData;
            if( property.dynamic() )
            {
                property.insert( m_campDataObj, index, objectData );
            }
            else
            {
                property.set( m_campDataObj, index, objectData );
            }
        }
        else
        {
            switch( elemAtom->type() )
            {
                case ::fwAtoms::Base::BOOLEAN:
                case ::fwAtoms::Base::STRING:
                {
                    std::string value = elemAtom->getString();

                    if( property.dynamic() )
                    {
                        property.insert( m_campDataObj, index, value );
                    }
                    else
                    {
                        property.set( m_campDataObj, index, value );
                    }
                    break;
                }
                case ::fwAtoms::Base::NUMERIC:
                {
                    ::fwAtoms::Numeric::sptr typedAtom = ::fwAtoms::Numeric::dynamicCast( elemAtom );
                    ::boost::apply_visitor( NumericArrayPropertyVisitor(typedAtom, m_campDataObj, property, index),
                                            typedAtom->getVariant() );
                    break;
                }

                case ::fwAtoms::Base::OBJECT:
                {
                    ::fwAtoms::Object::sptr objectAtom = ::fwAtoms::Object::dynamicCast(elemAtom);
                    ::fwData::Object::sptr objectData  =
                        ::fwAtomConversion::convert( objectAtom, m_cache, m_uuidPolicy);

                    if( property.dynamic() )
                    {
                        property.insert( m_campDataObj, index, objectData );
                    }
                    else
                    {
                        property.set( m_campDataObj, index, objectData );
                    }
                    break;
                }
                default:
                {
                    std::stringstream msg;
                    msg << "fwAtoms::Sequence elements of type '";
                    switch( elemAtom->type() )
                    {
                        case ::fwAtoms::Base::BLOB:
                            msg << "BLOB";
                            break;
                        case ::fwAtoms::Base::MAP:
                            msg << "MAP";
                            break;
                        case ::fwAtoms::Base::SEQUENCE:
                            msg << "SEQUENCE";
                            break;
                        default:
                            break;
                    }
                    msg <<"', are not supported in the data conversion process.";
                    FW_RAISE_EXCEPTION( exception::ConversionNotManaged(msg.str()) );
                    break;
                }
            }
        }
        ++index;
    }
}

//------------------------------------------------------------------------------

void AtomToDataMappingVisitor::visit(const camp::MapProperty& property)
{
    const std::string& name( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( msg.str() ), !atom );

    ::fwAtoms::Map::sptr mapAtom = ::fwAtoms::Map::dynamicCast(atom);

    for( ::fwAtoms::Map::ValueType elemAtom :  mapAtom->getValue() )
    {
        if (!elemAtom.second)
        {
            FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( "Not supported null element in Atom map." ),
                                   property.elementType() != ::camp::userType );

            ::fwData::Object::sptr objectData;
            property.set( m_campDataObj, elemAtom.first, objectData );
        }
        else
        {
            switch( elemAtom.second->type() )
            {
                case ::fwAtoms::Base::BOOLEAN:
                case ::fwAtoms::Base::STRING:
                {
                    std::string value = elemAtom.second->getString();
                    property.set( m_campDataObj, elemAtom.first, value );
                    break;
                }
                case ::fwAtoms::Base::NUMERIC:
                {
                    ::fwAtoms::Numeric::sptr typedAtom = ::fwAtoms::Numeric::dynamicCast( elemAtom.second );
                    ::boost::apply_visitor( NumericMapPropertyVisitor(typedAtom, m_campDataObj, property,
                                                                      elemAtom.first),
                                            typedAtom->getVariant() );
                    break;
                }
                case ::fwAtoms::Base::OBJECT:
                {
                    ::fwAtoms::Object::sptr objectAtom = ::fwAtoms::Object::dynamicCast(elemAtom.second);
                    ::fwData::Object::sptr objectData  =
                        ::fwAtomConversion::convert( objectAtom, m_cache, m_uuidPolicy);
                    property.set( m_campDataObj, elemAtom.first, objectData );
                    break;
                }
                case ::fwAtoms::Base::BLOB:
                {
                    ::fwAtoms::Blob::sptr blobAtom = ::fwAtoms::Blob::dynamicCast(elemAtom.second);
                    ::fwMemory::BufferObject::sptr buffer;
                    buffer = ::camp_ext::ValueMapper< ::fwAtoms::Blob::sptr >::to(blobAtom);
                    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged(
                                               "A blob cannot contain a null buffer pointer"),
                                           !buffer );
                    property.set( m_campDataObj, elemAtom.first, ::camp::UserObject(buffer) );
                    break;
                }
                default:
                {
                    std::stringstream msg;
                    msg << "fwAtoms::Map value elements of type '";
                    switch( elemAtom.second->type() )
                    {
                        case ::fwAtoms::Base::MAP:
                            msg << "MAP";
                            break;
                        case ::fwAtoms::Base::SEQUENCE:
                            msg << "SEQUENCE";
                            break;
                        default:
                            break;
                    }
                    msg <<"', are not supported in the data conversion process.";
                    FW_RAISE_EXCEPTION( exception::ConversionNotManaged(msg.str()) );
                    break;
                }
            }
        }
    }
}

} // end namespace fwAtomConversion
