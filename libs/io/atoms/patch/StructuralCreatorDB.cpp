/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "io/atoms/patch/StructuralCreatorDB.hpp"

#include "io/atoms/patch/infos/log.hpp"
#include "io/atoms/patch/IStructuralCreator.hpp"

namespace sight::io::atoms::patch
{

StructuralCreatorDB::sptr StructuralCreatorDB::s_default = std::make_shared<StructuralCreatorDB>();

//------------------------------------------------------------------------------

void StructuralCreatorDB::registerCreator(io::atoms::patch::IStructuralCreator::sptr creator)
{
    VersionIDType key = std::make_pair(creator->getObjectClassname(), creator->getObjectVersion());
    core::mt::WriteLock lock(m_mutex);
    m_creators[key] = creator;
}

// ----------------------------------------------------------------------------

io::atoms::patch::IStructuralCreator::sptr StructuralCreatorDB::getCreator(const std::string& classname,
                                                                           const std::string& version)
{
    VersionIDType key = std::make_pair(classname, version);

    core::mt::ReadLock lock(m_mutex);
    CreatorsType::const_iterator it = m_creators.find(key);
    if(it != m_creators.end())
    {
        return it->second;
    }
    else
    {
        return io::atoms::patch::IStructuralCreator::sptr();
    }
}

// ----------------------------------------------------------------------------

sight::atoms::Object::sptr StructuralCreatorDB::create(const std::string& classname, const std::string& version)
{
    io::atoms::patch::IStructuralCreator::sptr creator = this->getCreator( classname, version );
    SLM_ASSERT( "object creator of type '"<< classname <<"' and version '"<< version <<"' not found", creator );
    fwAtomsPatchInfoLogMacro("Create '" + classname + "|" + version + "'");
    sight::atoms::Object::sptr obj = creator->create();
    return obj;
}

// ----------------------------------------------------------------------------

size_t StructuralCreatorDB::size() const
{
    core::mt::ReadLock lock(m_mutex);
    return m_creators.size();
}

} //fwAtomsPatch
