/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
#ifndef __FWCOM_SIGNALBASE_HPP__
#define __FWCOM_SIGNALBASE_HPP__

#ifdef COM_LOG
#include <boost/lexical_cast.hpp>
#include <fwCore/mt/types.hpp>
#endif

#include <fwCore/BaseObject.hpp>

#include "fwCom/config.hpp"

#include "fwCom/Connection.hpp"

namespace fwCom
{

struct SlotBase;

/**
 * @brief Signal base class.
 */
struct SignalBase : virtual fwCore::BaseObject
{
    /**
     * @name Typedefs
     * @{ */
    typedef SPTR( SignalBase ) sptr;
    typedef WPTR( SignalBase )   wptr;
    /**  @} */

    SignalBase()
    {
#ifdef COM_LOG
        ::fwCore::mt::ScopedLock lock(s_mutexCounter);
        m_id = "Signal-" + ::boost::lexical_cast<std::string>(s_idCount++);
#endif
    };

    virtual ~SignalBase(){}

    /**
     * @brief Connects the given slot.
     * @throws BadSlot If given slot doesn't match signal type.
     */
    virtual Connection connect ( SPTR( SlotBase ) slot ) = 0;

    /**
     * @brief Disconnects the given slot.
     * @throws BadSlot If given slot is not found in current connections.
     */
    virtual void disconnect ( SPTR( SlotBase ) slot ) = 0;

    /// Returns number of connections.
    virtual size_t getNumberOfConnections() const = 0;

protected:
    /// Copy constructor forbidden
    SignalBase( const SignalBase& );

    /// Copy operator forbiden
    SignalBase& operator=( const SignalBase& );

#ifdef COM_LOG

public :

    typedef std::string IDType;

    /// Gets current m_id
    IDType getID() const
    {
        return m_id;
    }

    /// Sets new m_id
    void setID( IDType newId )
    {
        m_id = newId;
    }

private :

    /// Id of signal (not mandatory)
    IDType m_id;

    /// Id counter
    FWCOM_API static size_t s_idCount;

    /// Mutex to protect id counter
    FWCOM_API static ::fwCore::mt::Mutex s_mutexCounter;

#endif

};

} // namespace fwCom

#endif /* __FWCOM_SIGNALBASE_HPP__ */

