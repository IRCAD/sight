/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

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
    BOOST_FOREACH( SignalMapType::value_type elem, m_signals )
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
    BOOST_FOREACH( SignalMapType::value_type elem, m_signals )
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
#ifdef COM_LOG
void Signals::setID( const std::string prefix )
{
    BOOST_FOREACH( SignalMapType::value_type elem, m_signals )
    {
        elem.second->setID( prefix + elem.first );
    }
}
#endif
//-----------------------------------------------------------------------------

} // namespace fwCom

