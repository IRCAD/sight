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

#include "io/atoms/patch/helper/functions.hpp"

#include "io/atoms/patch/types.hpp"

#include <core/tools/UUID.hpp>

namespace sight::io::atoms::patch
{
namespace helper
{

//-----------------------------------------------------------------------------

std::string getClassname( const sight::atoms::Object::sptr& obj )
{
    return obj->getMetaInfo( io::atoms::patch::s_OBJ_CLASSNAME );
}

//-----------------------------------------------------------------------------

void setClassname( const sight::atoms::Object::sptr& obj, const std::string& newClassname )
{
    obj->setMetaInfo( io::atoms::patch::s_OBJ_CLASSNAME, newClassname );
}

//-----------------------------------------------------------------------------

std::string getVersion( const sight::atoms::Object::sptr& obj )
{
    return obj->getMetaInfo( io::atoms::patch::s_OBJ_VERSION );
}

//-----------------------------------------------------------------------------

void setVersion( const sight::atoms::Object::sptr& obj, const std::string& newVersion )
{
    obj->setMetaInfo( io::atoms::patch::s_OBJ_VERSION, newVersion );
}

//-----------------------------------------------------------------------------

void generateID( const sight::atoms::Object::sptr& obj )
{
    obj->setMetaInfo( io::atoms::patch::s_OBJ_ID, core::tools::UUID::generateUUID() );
}

//-----------------------------------------------------------------------------

void cleanFields( const sight::atoms::Object::sptr& obj )
{
    obj->setAttribute("fields", sight::atoms::Map::New());
}

//-----------------------------------------------------------------------------

void changeUID( const sight::atoms::Object::sptr& obj )
{
    for( const sight::atoms::Object::AttributesType::value_type& elem :  obj->getAttributes() )
    {
        if ( elem.second )
        {
            switch ( elem.second->type() )
            {
                case sight::atoms::Base::SEQUENCE:
                {
                    changeSeqUID( sight::atoms::Sequence::dynamicCast( elem.second ) );
                    break;
                }
                case sight::atoms::Base::MAP:
                {
                    changeMapUID( sight::atoms::Map::dynamicCast( elem.second ) );
                    break;
                }
                case sight::atoms::Base::OBJECT:
                {
                    changeUID( sight::atoms::Object::dynamicCast( elem.second ) );
                    break;
                }
                default:
                    break;
            }
        }
    }

    io::atoms::patch::helper::generateID( obj );
}

//-----------------------------------------------------------------------------

void changeMapUID( const sight::atoms::Map::sptr& map )
{
    for( sight::atoms::Map::ValueType elem :  map->getValue() )
    {
        if ( elem.second )
        {
            switch ( elem.second->type() )
            {
                case sight::atoms::Base::SEQUENCE:
                {
                    changeSeqUID( sight::atoms::Sequence::dynamicCast( elem.second ) );
                    break;
                }
                case sight::atoms::Base::MAP:
                {
                    changeMapUID( sight::atoms::Map::dynamicCast( elem.second ) );
                    break;
                }
                case sight::atoms::Base::OBJECT:
                {
                    changeUID( sight::atoms::Object::dynamicCast( elem.second ) );
                    break;
                }
                default:
                    break;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void changeSeqUID( const sight::atoms::Sequence::sptr& seq )
{
    for( sight::atoms::Base::sptr elem :  seq->getValue() )
    {
        if ( elem )
        {
            switch ( elem->type() )
            {
                case sight::atoms::Base::SEQUENCE:
                {
                    changeSeqUID( sight::atoms::Sequence::dynamicCast( elem ) );
                    break;
                }
                case sight::atoms::Base::MAP:
                {
                    changeMapUID( sight::atoms::Map::dynamicCast( elem ) );
                    break;
                }
                case sight::atoms::Base::OBJECT:
                {
                    changeUID( sight::atoms::Object::dynamicCast( elem ) );
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
