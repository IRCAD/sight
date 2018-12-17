/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
