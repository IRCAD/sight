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

#include "atoms/conversion/AtomVisitor.hpp"

#include "atoms/conversion/AtomToDataMappingVisitor.hpp"
#include "atoms/conversion/DataVisitor.hpp"
#include "atoms/conversion/exception/ClassnameMismatch.hpp"
#include "atoms/conversion/exception/DataFactoryNotFound.hpp"
#include "atoms/conversion/exception/DuplicatedDataUUID.hpp"

#include <data/Object.hpp>

namespace sight::atoms::conversion
{

//------------------------------------------------------------------------------

data::Object::sptr AtomVisitor::ReusePolicy::operator()(const std::string& uuid, const std::string& classname) const
{
    data::Object::sptr obj = data::Object::dynamicCast(core::tools::Object::fromUUID(uuid));

    SIGHT_THROW_EXCEPTION_IF(
        exception::ClassnameMismatch(
            "Loaded object classname (" + classname
            + ") for UUID '" + uuid
            + "' does not match existing classname (" + obj->classname() + ")"
        ),
        classname != obj->getClassname()
    );

    if(!obj)
    {
        try
        {
            obj = data::factory::New(classname);
        }
        catch(const core::Exception& e)
        {
            SIGHT_ERROR(e.what());
        }
        SIGHT_THROW_EXCEPTION_IF(
            exception::DataFactoryNotFound(
                std::string("Unable to build '") + classname + "': the data factory may be missing."
            )
            ,
            !obj
        );

        obj->setUUID(uuid);
    }

    return obj;
}

//------------------------------------------------------------------------------

data::Object::sptr AtomVisitor::ChangePolicy::operator()(
    const std::string& uuid,
    const std::string& classname
) const
{
    data::Object::sptr obj;
    try
    {
        obj = data::factory::New(classname);

        // uuid is set only if the given uuid is available
    }
    catch(const core::Exception& e)
    {
        SIGHT_ERROR(e.what());
    }
    SIGHT_THROW_EXCEPTION_IF(
        exception::DataFactoryNotFound(
            std::string("Unable to build '") + classname + "': the data factory may be missing."
        )
        ,
        !obj
    );

    if(!data::Object::fromUUID(uuid))
    {
        obj->setUUID(uuid);
    }

    return obj;
}

//------------------------------------------------------------------------------

data::Object::sptr AtomVisitor::StrictPolicy::operator()(
    const std::string& uuid,
    const std::string& classname
) const
{
    data::Object::sptr obj;
    try
    {
        obj = data::factory::New(classname);
    }
    catch(const core::Exception& e)
    {
        SIGHT_ERROR(e.what());
    }
    SIGHT_THROW_EXCEPTION_IF(
        exception::DataFactoryNotFound(
            std::string("Unable to build '") + classname + "': the data factory may be missing."
        )
        ,
        !obj
    );

    SIGHT_THROW_EXCEPTION_IF(
        exception::DuplicatedDataUUID(
            std::string("Try to create new data object '") + classname + "' with uuid '"
            + uuid + "' but this uuid is already used."
        ),
        data::Object::fromUUID(uuid)
    );

    obj->setUUID(uuid);

    return obj;
}

AtomVisitor::AtomVisitor(const atoms::Object::sptr& atomObj, DataCacheType& cache, const IReadPolicy& uuidPolicy) :
    m_atomObj(atomObj),
    m_cache(cache),
    m_uuidPolicy(uuidPolicy)
{
}

AtomVisitor::~AtomVisitor()
{
}

//------------------------------------------------------------------------------

void AtomVisitor::visit()
{
    this->processMetaInfos(m_atomObj->getMetaInfos());
    this->processAttributes(m_atomObj->getAttributes());
}

//------------------------------------------------------------------------------

void AtomVisitor::processMetaInfos(const atoms::Object::MetaInfosType& metaInfos)
{
    const DataVisitor::ClassnameType& classname = metaInfos.find(DataVisitor::CLASSNAME_METAINFO)->second;
    const std::string& uuid                     = metaInfos.find(DataVisitor::ID_METAINFO)->second;

    m_dataObj     = m_uuidPolicy(uuid, classname);
    m_cache[uuid] = m_dataObj;
}

//------------------------------------------------------------------------------

void AtomVisitor::processAttributes(const atoms::Object::AttributesType& attributes)
{
    const camp::Class& metaclass = ::camp::classByName(m_dataObj->getClassname());
    atoms::conversion::AtomToDataMappingVisitor visitor(m_dataObj, m_atomObj, m_cache, m_uuidPolicy);
    metaclass.visit(visitor);
}

//------------------------------------------------------------------------------

data::Object::sptr AtomVisitor::getDataObject() const
{
    return m_dataObj;
}

} // end namespace sight::atoms::conversion
