/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_OBJECT_HPP_
#define _FWTOOLS_OBJECT_HPP_

#include <string>
#include <vector>

#include <fwCore/base.hpp>
#include <fwCore/TimeStamp.hpp>
#include <fwCore/LogicStamp.hpp>

#include "fwTools/config.hpp"
#include "fwTools/fwID.hpp"

namespace fwTools
{

class UUID;

/**
 * @brief   Define Base class for FW4SPL objects and services
 * @class   Object
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWTOOLS_CLASS_API Object  : public ::fwCore::BaseObject , protected ::fwTools::fwID
{
public:
    fwCoreClassDefinitionsWithFactoryMacro((Object), (()), new Object );
    fwCoreAllowSharedFromThis();

    friend class ::fwTools::UUID;

    // expose API for ID management
    FWTOOLS_API using  ::fwTools::fwID::hasID;
    FWTOOLS_API using  ::fwTools::fwID::getID;
    FWTOOLS_API using  ::fwTools::fwID::setID;
    FWTOOLS_API using  ::fwTools::fwID::resetID;
    FWTOOLS_API using  ::fwTools::fwID::swapID;


    /// return the sub class classname : an alias of this->getClassname
    FWTOOLS_API std::string className() const;

    FWTOOLS_API Object();

    FWTOOLS_API virtual ~Object();

    /**
     * @name    ***DEPRECATED*** All concerning universal unique identifier (UUID) ***DEPRECATED***
     */
    //@{
    /**
     * @brief Set a name for an object
     * @deprecated (should use UUID)
     */
    FWTOOLS_API void setName ( const std::string _strName ) { m_strName = _strName; }

    /**
     * @brief Retrieve the name object
     * @deprecated (should use UUID)
     */
    FWTOOLS_API const std::string getName() const { return m_strName; }

    //@}

    FWTOOLS_API void modified()
    {
        m_logicStamp->modified();
    };

    FWTOOLS_API void timeModified()
    {
        m_timeStamp->modified();
    };

    FWTOOLS_API void setLifePeriod(::fwCore::TimeStamp::TimeStampType p)
    {
        m_timeStamp->setLifePeriod(p);
    };

    FWTOOLS_API ::fwCore::TimeStamp::TimeStampType getLifePeriod()
    {
        return m_timeStamp->getLifePeriod();
    };

    FWTOOLS_API bool periodExpired()
    {
        return m_timeStamp->periodExpired();
    };

    FWTOOLS_API ::fwCore::TimeStamp::sptr  getTimeStamp()  const { return m_timeStamp; }
    FWTOOLS_API ::fwCore::LogicStamp::sptr getLogicStamp() const { return m_logicStamp; }

    FWTOOLS_API ::fwCore::LogicStamp::csptr getOSRKey() const { return m_OSRKey; }

protected :

    ::fwCore::TimeStamp::sptr  m_timeStamp;

    ::fwCore::LogicStamp::sptr m_logicStamp;

    SPTR(::fwTools::UUID) m_uuid;

private :

    /// Standard copy operator, forbidden.
    Object &operator=(const Object &_obj);

    /// Name identification for an object
    std::string m_strName;

    /// key used to register object in OSR
    ::fwCore::LogicStamp::sptr m_OSRKey;
};

}

#endif /* _FWTOOLS_OBJECT_HPP_ */
