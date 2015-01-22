/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include <fwThread/Worker.hpp>

#include "fwCom/Slot.hpp"
#include "fwCom/Slot.hxx"

#include "fwCom/Slots.hpp"

namespace fwCom
{

Slots::Slots()
{}

//-----------------------------------------------------------------------------

Slots::~Slots()
{
#ifdef DEBUG
    BOOST_FOREACH( SlotMapType::value_type elem, m_slots )
    {
        SLM_ASSERT( "Slot '"<< elem.first <<"' has connected signals", elem.second->getNumberOfConnections() == 0 );
    }
#endif
}

//-----------------------------------------------------------------------------

void Slots::setWorker( const ::fwThread::Worker::sptr &worker )
{
    BOOST_FOREACH( SlotMapType::value_type elem, m_slots )
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
    BOOST_FOREACH( SlotMapType::value_type elem, m_slots )
    {
        slotKeys.push_back(elem.first);
    }
    return slotKeys;
}

//-----------------------------------------------------------------------------

Slots::Slots( const Slots& )
{}

//-----------------------------------------------------------------------------

Slots& Slots::operator=( const Slots& )
{
    return *this;
}

//-----------------------------------------------------------------------------

#ifdef COM_LOG
void Slots::setID( const std::string prefix )
{
    BOOST_FOREACH( SlotMapType::value_type elem, m_slots )
    {
        elem.second->setID( prefix + elem.first );
    }
}
#endif

} // namespace fwCom

