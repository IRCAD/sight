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


#include "fwAtomsPatch/infos/log.hpp"
#include "fwAtomsPatch/IStructuralCreator.hpp"
#include "fwAtomsPatch/StructuralCreatorDB.hpp"

namespace fwAtomsPatch
{

StructuralCreatorDB::sptr StructuralCreatorDB::s_default = std::make_shared<StructuralCreatorDB>();

void StructuralCreatorDB::registerCreator(::fwAtomsPatch::IStructuralCreator::sptr creator)
{
    VersionIDType key = std::make_pair(creator->getObjectClassname(), creator->getObjectVersion());
    ::fwCore::mt::WriteLock lock(m_mutex);
    m_creators[key] = creator;
}

// ----------------------------------------------------------------------------

::fwAtomsPatch::IStructuralCreator::sptr StructuralCreatorDB::getCreator(const std::string& classname,
                                                                         const std::string& version)
{
    VersionIDType key = std::make_pair(classname, version);

    ::fwCore::mt::ReadLock lock(m_mutex);
    CreatorsType::const_iterator it = m_creators.find(key);
    if(it != m_creators.end())
    {
        return it->second;
    }
    else
    {
        return ::fwAtomsPatch::IStructuralCreator::sptr();
    }
}

// ----------------------------------------------------------------------------

::fwAtoms::Object::sptr StructuralCreatorDB::create(const std::string& classname, const std::string& version)
{
    ::fwAtomsPatch::IStructuralCreator::sptr creator = this->getCreator( classname, version );
    OSLM_ASSERT( "object creator of type '"<< classname <<"' and version '"<< version <<"' not found", creator );
    fwAtomsPatchInfoLogMacro("Create '" + classname + "|" + version + "'");
    ::fwAtoms::Object::sptr obj = creator->create();
    return obj;
}

// ----------------------------------------------------------------------------

size_t StructuralCreatorDB::size() const
{
    ::fwCore::mt::ReadLock lock(m_mutex);
    return m_creators.size();
}



} //fwAtomsPatch
