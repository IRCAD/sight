/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCOM_SIGNALS_HPP__
#define __FWCOM_SIGNALS_HPP__

#include "fwCom/SignalBase.hpp"
#include "fwCom/config.hpp"

namespace fwCom
{

/**
 * @class   Signals.
 * @brief   This class proposes a mapping between a SignalKeyType and a SignalBase.
 *
 * @author IRCAD (Research and Development Team).
 * @date   2012.
 */
class FWCOM_CLASS_API Signals
{
public:

    typedef std::string SignalKeyType;
    typedef std::map< SignalKeyType, SignalBase::sptr > SignalMapType;
    typedef std::vector < SignalKeyType > SignalKeyContainerType;

    /// Constructor, does nothing
    FWCOM_API Signals();

    /// Constructor, check if all signals are disconnected
    FWCOM_API virtual ~Signals();

    /// Registers SignalBase in m_signals
    FWCOM_API Signals& operator()( const SignalKeyType &key, const SignalBase::sptr &Signal );

    /// Returns the SignalBase associated to the key, if key does not exist, the ptr is null
    FWCOM_API SignalBase::sptr operator[]( const SignalKeyType &key ) const;

    /// Returns all SignalKeyType registered in m_signals
    FWCOM_API SignalKeyContainerType getSignalKeys() const;

#ifdef COM_LOG
    /// Set new id for all signals using their key in m_signals, a prefix can be added
    FWCOM_API void setID( const std::string prefix = "" );
#endif

protected:

    /// Copy constructor forbidden
    Signals( const Signals& );

    /// Copy constructor forbidden
    Signals& operator=( const Signals& );

    /// Association < key , SignalBase::sptr >
    SignalMapType m_signals;
};

} // namespace fwCom

#endif //__FWCOM_SIGNALS_HPP__
