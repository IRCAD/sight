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

#include "fwCom/Slot.hpp"
#include "fwCom/Slot.hxx"

#include "fwCom/Slots.hpp"

#include <fwThread/Worker.hpp>

namespace fwCom
{

Slots::Slots()
{
}

//-----------------------------------------------------------------------------

Slots::~Slots()
{
#ifdef DEBUG
    for( SlotMapType::value_type elem :  m_slots )
    {
        SLM_ASSERT( "Slot '"<< elem.first <<"' has connected signals", elem.second->getNumberOfConnections() == 0 );
    }
#endif
}

//-----------------------------------------------------------------------------

void Slots::setWorker( const ::fwThread::Worker::sptr &worker )
{
    for( SlotMapType::value_type elem :  m_slots )
    {
        elem.second->setWorker(worker);
    }
}

//-----------------------------------------------------------------------------

Slots& Slots::operator()( const SlotKeyType &key, const SlotBase::sptr &slot )
{
    m_slots.insert( SlotMapType::value_type(key, slot) );
    return *this;
}

//-----------------------------------------------------------------------------

SlotBase::sptr Slots::operator[]( const SlotKeyType &key ) const
{
    SlotMapType::const_iterator it = m_slots.find(key);

    if(it != m_slots.end())
    {
        return it->second;
    }

    return SlotBase::sptr();
}

//-----------------------------------------------------------------------------

Slots::SlotKeyContainerType Slots::getSlotKeys() const
{
    Slots::SlotKeyContainerType slotKeys;
    for( SlotMapType::value_type elem :  m_slots )
    {
        slotKeys.push_back(elem.first);
    }
    return slotKeys;
}

//-----------------------------------------------------------------------------

Slots::Slots( const Slots& )
{
}

//-----------------------------------------------------------------------------

Slots& Slots::operator=( const Slots& )
{
    return *this;
}

//-----------------------------------------------------------------------------



} // namespace fwCom

