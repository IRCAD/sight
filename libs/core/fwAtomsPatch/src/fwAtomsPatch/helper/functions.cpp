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

#include "fwAtomsPatch/helper/functions.hpp"

#include "fwAtomsPatch/types.hpp"

#include <core/tools/UUID.hpp>

namespace fwAtomsPatch
{
namespace helper
{

//-----------------------------------------------------------------------------

std::string getClassname( const atoms::Object::sptr& obj )
{
    return obj->getMetaInfo( ::fwAtomsPatch::s_OBJ_CLASSNAME );
}

//-----------------------------------------------------------------------------

void setClassname( const atoms::Object::sptr& obj, const std::string& newClassname )
{
    obj->setMetaInfo( ::fwAtomsPatch::s_OBJ_CLASSNAME, newClassname );
}

//-----------------------------------------------------------------------------

std::string getVersion( const atoms::Object::sptr& obj )
{
    return obj->getMetaInfo( ::fwAtomsPatch::s_OBJ_VERSION );
}

//-----------------------------------------------------------------------------

void setVersion( const atoms::Object::sptr& obj, const std::string& newVersion )
{
    obj->setMetaInfo( ::fwAtomsPatch::s_OBJ_VERSION, newVersion );
}

//-----------------------------------------------------------------------------

void generateID( const atoms::Object::sptr& obj )
{
    obj->setMetaInfo( fwAtomsPatch::s_OBJ_ID, core::tools::UUID::generateUUID() );
}

//-----------------------------------------------------------------------------

void cleanFields( const atoms::Object::sptr& obj )
{
    obj->setAttribute("fields", atoms::Map::New());
}

//-----------------------------------------------------------------------------

void changeUID( const atoms::Object::sptr& obj )
{
    for( const atoms::Object::AttributesType::value_type& elem :  obj->getAttributes() )
    {
        if ( elem.second )
        {
            switch ( elem.second->type() )
            {
                case atoms::Base::SEQUENCE:
                {
                    changeSeqUID( atoms::Sequence::dynamicCast( elem.second ) );
                    break;
                }
                case atoms::Base::MAP:
                {
                    changeMapUID( atoms::Map::dynamicCast( elem.second ) );
                    break;
                }
                case atoms::Base::OBJECT:
                {
                    changeUID( atoms::Object::dynamicCast( elem.second ) );
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

void changeMapUID( const atoms::Map::sptr& map )
{
    for( atoms::Map::ValueType elem :  map->getValue() )
    {
        if ( elem.second )
        {
            switch ( elem.second->type() )
            {
                case atoms::Base::SEQUENCE:
                {
                    changeSeqUID( atoms::Sequence::dynamicCast( elem.second ) );
                    break;
                }
                case atoms::Base::MAP:
                {
                    changeMapUID( atoms::Map::dynamicCast( elem.second ) );
                    break;
                }
                case atoms::Base::OBJECT:
                {
                    changeUID( atoms::Object::dynamicCast( elem.second ) );
                    break;
                }
                default:
                    break;
            }
        }
    }
}

//-----------------------------------------------------------------------------

void changeSeqUID( const atoms::Sequence::sptr& seq )
{
    for( atoms::Base::sptr elem :  seq->getValue() )
    {
        if ( elem )
        {
            switch ( elem->type() )
            {
                case atoms::Base::SEQUENCE:
                {
                    changeSeqUID( atoms::Sequence::dynamicCast( elem ) );
                    break;
                }
                case atoms::Base::MAP:
                {
                    changeMapUID( atoms::Map::dynamicCast( elem ) );
                    break;
                }
                case atoms::Base::OBJECT:
                {
                    changeUID( atoms::Object::dynamicCast( elem ) );
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
