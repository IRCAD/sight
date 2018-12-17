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

#include <fwData/Object.hpp>

#include "fwAtomConversion/AtomVisitor.hpp"
#include "fwAtomConversion/DataVisitor.hpp"
#include "fwAtomConversion/AtomToDataMappingVisitor.hpp"
#include "fwAtomConversion/exception/DataFactoryNotFound.hpp"
#include "fwAtomConversion/exception/DuplicatedDataUUID.hpp"
#include "fwAtomConversion/exception/ClassnameMismatch.hpp"

namespace fwAtomConversion
{

::fwData::Object::sptr AtomVisitor::ReusePolicy::operator()(const std::string &uuid, const std::string &classname) const
{
    ::fwData::Object::sptr obj = ::fwData::Object::dynamicCast(::fwTools::UUID::get(uuid));

    FW_RAISE_EXCEPTION_IF(
        exception::ClassnameMismatch("Loaded object classname (" + classname
                                     + ") for UUID '" + uuid
                                     + "' does not match existing classname (" + obj->classname() + ")"  ),
        classname != obj->getClassname());

    if (!obj)
    {
        ::fwData::Object::sptr obj = ::fwData::factory::New(classname);


        FW_RAISE_EXCEPTION_IF(
            exception::DataFactoryNotFound(
                std::string("Unable to build '") + classname + "': the data factory may be missing.")
            , !obj
            );
        bool uuidIsSet = ::fwTools::UUID::set(obj, uuid);

        OSLM_ASSERT( "UUID '" << uuid << "' should not exist", uuidIsSet );
    }

    return obj;
}

::fwData::Object::sptr AtomVisitor::ChangePolicy::operator()(const std::string &uuid,
                                                             const std::string &classname) const
{
    ::fwData::Object::sptr obj = ::fwData::factory::New(classname);
    // uuid is set only if the given uuid is available

    FW_RAISE_EXCEPTION_IF(
        exception::DataFactoryNotFound(
            std::string("Unable to build '") + classname + "': the data factory may be missing.")
        , !obj
        );

    ::fwTools::UUID::set(obj, uuid);
    return obj;
}

::fwData::Object::sptr AtomVisitor::StrictPolicy::operator()(const std::string &uuid,
                                                             const std::string &classname) const
{

    ::fwData::Object::sptr obj = ::fwData::factory::New(classname);

    FW_RAISE_EXCEPTION_IF(
        exception::DataFactoryNotFound(
            std::string("Unable to build '") + classname + "': the data factory may be missing.")
        , !obj
        );

    bool uuidIsSet = ::fwTools::UUID::set(obj, uuid);

    FW_RAISE_EXCEPTION_IF(
        exception::DuplicatedDataUUID(
            std::string( "Try to create new data object '") + classname + "' with uuid '"
            + uuid + "' but this uuid is already used."
            ), !uuidIsSet );

    return obj;

}



AtomVisitor::AtomVisitor(const ::fwAtoms::Object::sptr &atomObj, DataCacheType & cache, const IReadPolicy &uuidPolicy)
    : m_atomObj ( atomObj ),
      m_cache ( cache ),
      m_uuidPolicy(uuidPolicy)
{
}

AtomVisitor::~AtomVisitor()
{
}

void AtomVisitor::visit()
{
    this->processMetaInfos( m_atomObj->getMetaInfos() );
    this->processAttributes( m_atomObj->getAttributes() );
}

void AtomVisitor::processMetaInfos( const ::fwAtoms::Object::MetaInfosType & metaInfos )
{
    const DataVisitor::ClassnameType& classname = metaInfos.find( DataVisitor::CLASSNAME_METAINFO )->second;
    const ::fwTools::UUID::UUIDType& uuid       = metaInfos.find( DataVisitor::ID_METAINFO )->second;

    m_dataObj     = m_uuidPolicy(uuid, classname);
    m_cache[uuid] = m_dataObj;
}

void AtomVisitor::processAttributes( const ::fwAtoms::Object::AttributesType & attributes )
{
    const camp::Class& metaclass = ::camp::classByName( m_dataObj->getClassname() );
    ::fwAtomConversion::AtomToDataMappingVisitor visitor ( m_dataObj, m_atomObj, m_cache, m_uuidPolicy );
    metaclass.visit(visitor);
}

::fwData::Object::sptr AtomVisitor::getDataObject() const
{
    return m_dataObj;
}

} // end namespace fwAtomConversion
