/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/BufferObject.hpp>

#include <fwCamp/factory/new.hpp>

#include <fwAtoms/Blob.hpp>
#include <fwAtoms/Sequence.hpp>
#include <fwAtoms/Map.hpp>
#include "fwAtomConversion/camp/ValueMapper.hpp"

#include "fwAtomConversion/AtomToDataMappingVisitor.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/mapper/Base.hpp"
#include "fwAtomConversion/camp/ValueMapper.hpp"
#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/exception/ConversionNotManaged.hpp"

namespace fwAtomConversion
{

AtomToDataMappingVisitor::AtomToDataMappingVisitor(
        ::fwData::Object::sptr dataObj,
         ::fwAtoms::Object::sptr atomObj,
          AtomVisitor::DataCacheType & cache )
 : m_dataObj(dataObj),
   m_campDataObj( m_dataObj.get() ),
   m_atomObj(atomObj),
   m_cache(cache)
{}

AtomToDataMappingVisitor::~AtomToDataMappingVisitor(){}


void AtomToDataMappingVisitor::visit(const camp::SimpleProperty& property)
{
    const std::string& name ( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( msg.str() ), ! atom );

    property.set( m_campDataObj, atom->getString() );
}

void AtomToDataMappingVisitor::visit(const camp::EnumProperty& property)
{
    const std::string& name ( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( msg.str() ), ! atom );

    property.set( m_campDataObj, atom->getString() );
}


void AtomToDataMappingVisitor::visit(const camp::UserProperty& property)
{
    const std::string& name ( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );
    if ( atom ) // attribute not exist if was a null object sptr
    {
        switch( atom->type() )
        {
        case ::fwAtoms::Base::BLOB :
        {
            ::fwAtoms::Blob::sptr blobAtom = ::fwAtoms::Blob::dynamicCast(atom);
            ::fwTools::BufferObject::sptr buffer;
            buffer = ::camp_ext::ValueMapper< ::fwAtoms::Blob::sptr >::to(blobAtom);
            property.set( m_campDataObj, ::camp::UserObject(buffer) );
            break;
        }
        case ::fwAtoms::Base::OBJECT :
        {
            ::fwAtoms::Object::sptr objectAtom = ::fwAtoms::Object::dynamicCast(atom);
            ::fwData::Object::sptr objectData = ::fwAtomConversion::convert( objectAtom, m_cache );
            property.set( m_campDataObj, objectData );
            break;
        }
        default :
        {
            std::stringstream msg;
            msg << "Attribute of type '";
            switch( atom->type() )
            {
            case ::fwAtoms::Base::BOOLEAN :
                msg << "BOOLEAN";
                break;
            case ::fwAtoms::Base::STRING :
                msg << "STRING";
                break;
            case ::fwAtoms::Base::NUMERIC :
                msg << "NUMERIC";
                break;
            case ::fwAtoms::Base::MAP :
                msg << "MAP";
                break;
            case ::fwAtoms::Base::SEQUENCE :
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


void AtomToDataMappingVisitor::visit(const camp::ArrayProperty& property)
{
    const std::string& name ( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( msg.str() ), ! atom );

    ::fwAtoms::Sequence::sptr seqAtom = ::fwAtoms::Sequence::dynamicCast(atom);
    unsigned int index = 0;
    BOOST_FOREACH( ::fwAtoms::Base::sptr elemAtom, seqAtom->getValue() )
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
            case ::fwAtoms::Base::BOOLEAN :
            case ::fwAtoms::Base::NUMERIC :
            case ::fwAtoms::Base::STRING :
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
            case ::fwAtoms::Base::OBJECT :
            {
                ::fwAtoms::Object::sptr objectAtom = ::fwAtoms::Object::dynamicCast(elemAtom);
                ::fwData::Object::sptr objectData = ::fwAtomConversion::convert( objectAtom, m_cache );

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
            default :
            {
                std::stringstream msg;
                msg << "fwAtoms::Sequence elements of type '";
                switch( elemAtom->type() )
                {
                case ::fwAtoms::Base::BLOB :
                    msg << "BLOB";
                    break;
                case ::fwAtoms::Base::MAP :
                    msg << "MAP";
                    break;
                case ::fwAtoms::Base::SEQUENCE :
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


void AtomToDataMappingVisitor::visit(const camp::MapProperty& property)
{
    const std::string& name ( property.name() );
    ::fwAtoms::Base::sptr atom = m_atomObj->getAttribute( name );

    std::stringstream msg;
    msg << "Atom attribute is not well formed. Attribute '" << name << "' missing for data conversion";
    FW_RAISE_EXCEPTION_IF( exception::ConversionNotManaged( msg.str() ), ! atom );

    ::fwAtoms::Map::sptr mapAtom = ::fwAtoms::Map::dynamicCast(atom);

    BOOST_FOREACH( ::fwAtoms::Map::ValueType elemAtom, mapAtom->getValue() )
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
            case ::fwAtoms::Base::BOOLEAN :
            case ::fwAtoms::Base::NUMERIC :
            case ::fwAtoms::Base::STRING :
            {
                std::string value = elemAtom.second->getString();
                property.set( m_campDataObj, elemAtom.first, value );
                break;
            }
            case ::fwAtoms::Base::OBJECT :
            {
                ::fwAtoms::Object::sptr objectAtom = ::fwAtoms::Object::dynamicCast(elemAtom.second);
                ::fwData::Object::sptr objectData = ::fwAtomConversion::convert( objectAtom, m_cache );
                property.set( m_campDataObj, elemAtom.first, objectData );
                break;
            }
            default :
            {
                std::stringstream msg;
                msg << "fwAtoms::Map value elements of type '";
                switch( elemAtom.second->type() )
                {
                case ::fwAtoms::Base::BLOB :
                    msg << "BLOB";
                    break;
                case ::fwAtoms::Base::MAP :
                    msg << "MAP";
                    break;
                case ::fwAtoms::Base::SEQUENCE :
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
