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

#include "data/helper/Composite.hpp"

#include <data/Composite.hpp>

#include <core/com/Signal.hxx>

namespace sight::data
{

namespace helper
{

//-----------------------------------------------------------------------------

Composite::Composite(data::Composite::wptr _composite) :
    m_composite(_composite)
{
}

//-----------------------------------------------------------------------------

Composite::~Composite()
{
    if(!m_addedObjects.empty() || !m_removedObjects.empty() || !m_newChangedObjects.empty()
       || !m_oldChangedObjects.empty())
    {
        notify();
    }
}

//-----------------------------------------------------------------------------

void Composite::add(std::string _compositeKey, data::Object::sptr _newObject)
{
    SIGHT_FATAL_IF(
        "The composite key " << _compositeKey << " does not exist in the composite, this is the key of the"
                                                 "object to be added.",
        m_composite.lock()->find(_compositeKey) != m_composite.lock()->end()
    );

    // Modify composite
    m_composite.lock()->getContainer()[_compositeKey] = _newObject;

    m_addedObjects[_compositeKey] = _newObject;
}

//-----------------------------------------------------------------------------

void Composite::remove(std::string _compositeKey)
{
    SIGHT_FATAL_IF(
        "The composite key " << _compositeKey << " does not exist in the composite, this is the key of the"
                                                 "object to be removed.",
        m_composite.lock()->find(_compositeKey) == m_composite.lock()->end()
    );

    // Get old object
    data::Object::sptr objBackup = m_composite.lock()->getContainer()[_compositeKey];

    // Modify composite
    m_composite.lock()->getContainer().erase(_compositeKey);

    m_removedObjects[_compositeKey] = objBackup;
}

//-----------------------------------------------------------------------------

void Composite::clear()
{
    data::Composite::sptr composite = m_composite.lock();
    std::vector<std::string> vectKey;
    std::transform(
        composite->begin(),
        composite->end(),
        std::back_inserter(vectKey),
        std::bind(&sight::data::Composite::value_type::first, std::placeholders::_1)
    );

    for(std::string key : vectKey)
    {
        this->remove(key);
    }
}

//-----------------------------------------------------------------------------

void Composite::swap(std::string _compositeKey, data::Object::sptr _newObject)
{
    SIGHT_FATAL_IF(
        "The composite key " << _compositeKey << " does not exist in the composite, this is the key of the"
                                                 "object to be swapped.",
        m_composite.lock()->find(_compositeKey) == m_composite.lock()->end()
    );

    // Get old object
    data::Object::sptr objBackup = m_composite.lock()->getContainer()[_compositeKey];

    if(objBackup != _newObject)
    {
        // Modify composite
        m_composite.lock()->getContainer()[_compositeKey] = _newObject;

        m_newChangedObjects[_compositeKey] = _newObject;
        m_oldChangedObjects[_compositeKey] = objBackup;
    }
    else
    {
        SIGHT_INFO(
            "Cannot swap this object ( " << _compositeKey
            << " ) in composite because it is the same object. Do nothing (not notification)"
        );
    }
}

//-----------------------------------------------------------------------------

void Composite::notify()
{
    if(!m_removedObjects.empty())
    {
        auto sig = m_composite.lock()->signal<data::Composite::RemovedObjectsSignalType>(
            data::Composite::s_REMOVED_OBJECTS_SIG
        );

        sig->asyncEmit(m_removedObjects);
    }

    if(!m_newChangedObjects.empty() && !m_oldChangedObjects.empty())
    {
        auto sig = m_composite.lock()->signal<data::Composite::ChangedObjectsSignalType>(
            data::Composite::s_CHANGED_OBJECTS_SIG
        );

        sig->asyncEmit(m_newChangedObjects, m_oldChangedObjects);
    }

    if(!m_addedObjects.empty())
    {
        auto sig = m_composite.lock()->signal<data::Composite::AddedObjectsSignalType>(
            data::Composite::s_ADDED_OBJECTS_SIG
        );

        sig->asyncEmit(m_addedObjects);
    }

    SIGHT_INFO_IF(
        "No changes were found on the composite '" + m_composite.lock()->getID() + "', nothing to notify.",
        m_addedObjects.empty() && m_newChangedObjects.empty() && m_removedObjects.empty()
    );

    m_removedObjects.clear();
    m_newChangedObjects.clear();
    m_oldChangedObjects.clear();
    m_addedObjects.clear();
}

//-----------------------------------------------------------------------------

} // namespace helper

} // namespace sight::data
