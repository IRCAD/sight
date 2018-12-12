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

#ifdef DEBUG
#include "fwCore/spyLog.hpp"
#endif

#include "fwCom/SignalBase.hpp"
#include "fwCom/Signals.hpp"

namespace fwCom
{

Signals::Signals()
{
}

//-----------------------------------------------------------------------------

Signals::~Signals()
{
#ifdef DEBUG
    for( SignalMapType::value_type elem :  m_signals )
    {
        OSLM_ASSERT( "Signal '"<< elem.first <<"' has connected slots", elem.second->getNumberOfConnections() == 0 );
    }
#endif
}

//-----------------------------------------------------------------------------

Signals& Signals::operator()( const SignalKeyType &key, const SignalBase::sptr &Signal )
{
    m_signals.insert( SignalMapType::value_type(key, Signal) );
    return *this;
}

//-----------------------------------------------------------------------------

SignalBase::sptr Signals::operator[]( const SignalKeyType &key ) const
{
    SignalMapType::const_iterator it = m_signals.find(key);

    if(it != m_signals.end())
    {
        return it->second;
    }

    return SignalBase::sptr();
}

//-----------------------------------------------------------------------------

Signals::SignalKeyContainerType Signals::getSignalKeys() const
{
    Signals::SignalKeyContainerType SignalKeys;
    for( SignalMapType::value_type elem :  m_signals )
    {
        SignalKeys.push_back(elem.first);
    }
    return SignalKeys;
}

//-----------------------------------------------------------------------------

Signals::Signals( const Signals& )
{
}

//-----------------------------------------------------------------------------

Signals& Signals::operator=( const Signals& )
{
    return *this;
}

//-----------------------------------------------------------------------------

} // namespace fwCom

