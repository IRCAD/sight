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

#include <fwAtoms/Object.hpp>

#include <fwData/Object.hpp>

#include "fwAtomConversion/convert.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/AtomVisitor.hpp"
#include "fwAtomConversion/mapper/Base.hpp"

namespace fwAtomConversion
{

::fwAtoms::Object::sptr convert(const ::fwData::Object::sptr &data )
{
    DataVisitor::AtomCacheType cache;
    return convert( data, cache );
}

//-----------------------------------------------------------------------------

::fwAtoms::Object::sptr convert(const ::fwData::Object::sptr &dataObj, DataVisitor::AtomCacheType & cache )
{
    ::fwAtoms::Object::sptr atom;

    DataVisitor::AtomCacheType::iterator elem = cache.find( ::fwTools::UUID::get( dataObj ) );

    if ( elem == cache.end() )
    {
        SPTR(mapper::Base) mapper = mapper::factory::New( dataObj->getClassname() );
        if ( mapper )
        {
            atom = mapper->convert( dataObj, cache );
        }
        else
        {
            const camp::Class& metaclass = ::camp::classByName( dataObj->getClassname() );
            ::fwAtomConversion::DataVisitor visitor ( dataObj, cache );
            metaclass.visit(visitor);
            atom = visitor.getAtomObject();
        }
    }
    else // already analysed
    {
        atom = elem->second;
    }

    return atom;
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr convert( const ::fwAtoms::Object::sptr &atom,
                                const AtomVisitor::IReadPolicy &uuidPolicy
                                )
{
    AtomVisitor::DataCacheType cache;
    return convert( atom, cache, uuidPolicy );
}

//-----------------------------------------------------------------------------

::fwData::Object::sptr convert( const ::fwAtoms::Object::sptr &atomObj, AtomVisitor::DataCacheType & cache,
                                const AtomVisitor::IReadPolicy &uuidPolicy
                                )
{
    ::fwData::Object::sptr data;

    AtomVisitor::DataCacheType::iterator elem = cache.find( atomObj->getMetaInfo( DataVisitor::ID_METAINFO ) );

    if ( elem == cache.end() )
    {
        SPTR(mapper::Base) mapper = mapper::factory::New( atomObj->getMetaInfo( DataVisitor::CLASSNAME_METAINFO ) );
        if ( mapper )
        {
            data = mapper->convert( atomObj, cache, uuidPolicy );
        }
        else
        {
            ::fwAtomConversion::AtomVisitor visitor ( atomObj, cache, uuidPolicy );
            visitor.visit();
            data = visitor.getDataObject();
        }
    }
    else // already analyzed
    {
        data = elem->second;
    }

    return data;
}

} /// end namespace fwAtomConversion
