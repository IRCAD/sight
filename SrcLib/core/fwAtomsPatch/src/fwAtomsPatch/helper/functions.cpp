/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwTools/UUID.hpp>

#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/types.hpp"

namespace fwAtomsPatch
{
namespace helper
{

//-----------------------------------------------------------------------------

std::string getClassname( const ::fwAtoms::Object::sptr & obj )
{
    return obj->getMetaInfo( ::fwAtomsPatch::s_OBJ_CLASSNAME );
}

//-----------------------------------------------------------------------------

void setClassname( const ::fwAtoms::Object::sptr & obj, const std::string & newClassname )
{
    obj->setMetaInfo( ::fwAtomsPatch::s_OBJ_CLASSNAME, newClassname );
}

//-----------------------------------------------------------------------------

std::string getVersion( const ::fwAtoms::Object::sptr & obj )
{
    return obj->getMetaInfo( ::fwAtomsPatch::s_OBJ_VERSION );
}

//-----------------------------------------------------------------------------

void setVersion( const ::fwAtoms::Object::sptr & obj, const std::string & newVersion )
{
    obj->setMetaInfo( ::fwAtomsPatch::s_OBJ_VERSION, newVersion );
}

//-----------------------------------------------------------------------------

void generateID( const ::fwAtoms::Object::sptr & obj )
{
    obj->setMetaInfo( fwAtomsPatch::s_OBJ_ID, ::fwTools::UUID::generateUUID() );
}

//-----------------------------------------------------------------------------

void cleanFields( const ::fwAtoms::Object::sptr & obj )
{
    obj->setAttribute("fields", ::fwAtoms::Map::New());
}

//-----------------------------------------------------------------------------

void changeUID( const ::fwAtoms::Object::sptr & obj )
{
    BOOST_FOREACH( const ::fwAtoms::Object::AttributesType::value_type& elem, obj->getAttributes() )
    {
        if ( elem.second )
        {
            switch ( elem.second->type() )
            {
                case ::fwAtoms::Base::SEQUENCE:
                {
                    changeSeqUID( ::fwAtoms::Sequence::dynamicCast( elem.second ) );
                    break;
                }
                case ::fwAtoms::Base::MAP:
                {
                    changeMapUID( ::fwAtoms::Map::dynamicCast( elem.second ) );
                    break;
                }
                case ::fwAtoms::Base::OBJECT:
                {
                    changeUID( ::fwAtoms::Object::dynamicCast( elem.second ) );
                    break;
                }
                default:
                    break;
            }
        }
    }

    ::fwAtomsPatch::helper::generateID( obj );
}

//-----------------------------------------------------------------------------

void changeMapUID( const ::fwAtoms::Map::sptr & map )
{
    BOOST_FOREACH( ::fwAtoms::Map::ValueType elem, map->getValue() )
    {
        if ( elem.second )
        {
            switch ( elem.second->type() )
            {
                case ::fwAtoms::Base::SEQUENCE:
                {
                    changeSeqUID( ::fwAtoms::Sequence::dynamicCast( elem.second ) );
                    break;
                }
                case ::fwAtoms::Base::MAP:
                {
                    changeMapUID( ::fwAtoms::Map::dynamicCast( elem.second ) );
                    break;
                }
                case ::fwAtoms::Base::OBJECT:
                {
                    changeUID( ::fwAtoms::Object::dynamicCast( elem.second ) );
                    break;
                }
                default:
                    break;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void changeSeqUID( const ::fwAtoms::Sequence::sptr & seq )
{
    BOOST_FOREACH( ::fwAtoms::Base::sptr elem, seq->getValue() )
    {
        if ( elem )
        {
            switch ( elem->type() )
            {
                case ::fwAtoms::Base::SEQUENCE:
                {
                    changeSeqUID( ::fwAtoms::Sequence::dynamicCast( elem ) );
                    break;
                }
                case ::fwAtoms::Base::MAP:
                {
                    changeMapUID( ::fwAtoms::Map::dynamicCast( elem ) );
                    break;
                }
                case ::fwAtoms::Base::OBJECT:
                {
                    changeUID( ::fwAtoms::Object::dynamicCast( elem ) );
                    break;
                }
                default:
                    break;
            }
        }
    }
}

//-----------------------------------------------------------------------------

} //helper
} //fwAtomHelper

