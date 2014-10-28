/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/make_shared.hpp>

#include "fwAtomsPatch/infos/log.hpp"
#include "fwAtomsPatch/IStructuralCreator.hpp"
#include "fwAtomsPatch/StructuralCreatorDB.hpp"

namespace fwAtomsPatch
{

StructuralCreatorDB::sptr StructuralCreatorDB::s_default = ::boost::make_shared<StructuralCreatorDB>();

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
