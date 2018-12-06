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

#include "fwAtomsPatch/helper/functions.hpp"
#include "fwAtomsPatch/types.hpp"

#include <fwTools/UUID.hpp>

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
    for( const ::fwAtoms::Object::AttributesType::value_type& elem :  obj->getAttributes() )
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
    for( ::fwAtoms::Map::ValueType elem :  map->getValue() )
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
    for( ::fwAtoms::Base::sptr elem :  seq->getValue() )
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

