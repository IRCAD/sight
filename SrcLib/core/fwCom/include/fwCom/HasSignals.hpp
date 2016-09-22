/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_HASSIGNALS_HPP__
#define __FWCOM_HASSIGNALS_HPP__

#include "fwCom/config.hpp"
#include "fwCom/Signals.hpp"

namespace fwCom
{

struct SignalBase;

/**
 * @brief   This class proposes a mapping between a SignalKeyType and a SignalBase.
 */
class HasSignals
{

public:

    typedef std::shared_ptr< HasSignals > sptr;
    typedef std::shared_ptr< const HasSignals > csptr;

    HasSignals()
    {
    }
    virtual ~HasSignals()
    {
    }

    SPTR( SignalBase ) signal( const Signals::SignalKeyType & key ) const
    {
        return m_signals[key];
    }

    template< typename SignalType >
    SPTR( SignalType ) signal( const Signals::SignalKeyType & key ) const
    {
        SPTR( SignalType ) Signal = std::dynamic_pointer_cast< SignalType >( this->signal(key) );
        return Signal;
    }

    template<typename SignalType>
    SPTR( SignalType ) newSignal(const Signals::SignalKeyType & key)
    {
        SPTR( SignalType ) sig = std::make_shared< SignalType > ();
        m_signals(key, sig);
        return sig;
    }

protected:

    /// Copy constructor forbidden
    HasSignals( const HasSignals& );

    /// Copy operator forbidden
    HasSignals& operator=( const HasSignals& );

    Signals m_signals;
};

} // namespace fwCom

#endif // __FWCOM_HASSIGNALS_HPP__
