/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_HASIGNALS_HPP__
#define __FWCOM_HASIGNALS_HPP__

#include "fwCom/config.hpp"
#include "fwCom/Signals.hpp"

namespace fwCom
{

struct SignalBase;

/**
 * @class   HasSignals
 * @brief   This class proposes a mapping between a SignalKeyType and a SignalBase.
 * @date   2012.
 */
class HasSignals
{

public:

    typedef ::boost::shared_ptr< HasSignals > sptr;

    HasSignals(){}

    SPTR( SignalBase ) signal( const Signals::SignalKeyType & key ) const
    {
        return m_signals[key];
    }

    template< typename SignalType >
    SPTR( SignalType ) signal( const Signals::SignalKeyType & key ) const
    {
        SPTR( SignalType ) Signal = ::boost::dynamic_pointer_cast< SignalType >( this->signal(key) );
        return Signal;
    }


protected:

    /// Copy constructor forbidden
    HasSignals( const HasSignals& );

    /// Copy operator forbidden
    HasSignals& operator=( const HasSignals& );

    Signals m_signals;
};

} // namespace fwCom

#endif // __FWCOM_HASIGNALS_HPP__
