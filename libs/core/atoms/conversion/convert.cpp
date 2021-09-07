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

#include "atoms/conversion/convert.hpp"

#include "atoms/conversion/AtomVisitor.hpp"
#include "atoms/conversion/DataVisitor.hpp"
#include "atoms/conversion/mapper/Base.hpp"
#include <atoms/Object.hpp>

#include <data/Object.hpp>

namespace sight::atoms::conversion
{

//------------------------------------------------------------------------------

atoms::Object::sptr convert(const data::Object::sptr& data)
{
    DataVisitor::AtomCacheType cache;
    return convert(data, cache);
}

//-----------------------------------------------------------------------------

atoms::Object::sptr convert(const data::Object::sptr& dataObj, DataVisitor::AtomCacheType& cache)
{
    atoms::Object::sptr atom;

    DataVisitor::AtomCacheType::iterator elem = cache.find(dataObj->getUUID());

    if(elem == cache.end())
    {
        SPTR(mapper::Base) mapper = mapper::factory::New(dataObj->getClassname());
        if(mapper)
        {
            atom = mapper->convert(dataObj, cache);
        }
        else
        {
            const camp::Class& metaclass = ::camp::classByName(dataObj->getClassname());
            atoms::conversion::DataVisitor visitor(dataObj, cache);
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

data::Object::sptr convert(
    const atoms::Object::sptr& atom,
    const AtomVisitor::IReadPolicy& uuidPolicy
)
{
    AtomVisitor::DataCacheType cache;
    return convert(atom, cache, uuidPolicy);
}

//-----------------------------------------------------------------------------

data::Object::sptr convert(
    const atoms::Object::sptr& atomObj,
    AtomVisitor::DataCacheType& cache,
    const AtomVisitor::IReadPolicy& uuidPolicy
)
{
    data::Object::sptr data;

    AtomVisitor::DataCacheType::iterator elem = cache.find(atomObj->getMetaInfo(DataVisitor::ID_METAINFO));

    if(elem == cache.end())
    {
        SPTR(mapper::Base) mapper = mapper::factory::New(atomObj->getMetaInfo(DataVisitor::CLASSNAME_METAINFO));
        if(mapper)
        {
            data = mapper->convert(atomObj, cache, uuidPolicy);
        }
        else
        {
            atoms::conversion::AtomVisitor visitor(atomObj, cache, uuidPolicy);
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

} /// end namespace sight::atoms::conversion
