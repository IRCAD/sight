/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_OBJECT_HPP__
#define __FWTOOLS_OBJECT_HPP__

#include "fwTools/config.hpp"
#include "fwTools/fwID.hpp"

#include <fwCore/base.hpp>
#include <fwCore/TimeStamp.hpp>

#include <string>
#include <vector>

namespace fwTools
{

class UUID;

/**
 * @brief   Define Base class for FW4SPL objects and services
 */
class FWTOOLS_CLASS_API Object  : public ::fwCore::BaseObject,
                                  protected ::fwTools::fwID
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (Object)(::fwCore::BaseObject) );
    fwCoreAllowSharedFromThis();

    friend class ::fwTools::UUID;

    // expose API for ID management
    using  ::fwTools::fwID::hasID;
    using  ::fwTools::fwID::getID;
    using  ::fwTools::fwID::setID;
    using  ::fwTools::fwID::resetID;

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
    void setName ( const std::string _strName )
    {
        m_strName = _strName;
    }

    /**
     * @brief Retrieve the name object
     * @deprecated (should use UUID)
     */
    const std::string getName() const
    {
        return m_strName;
    }

    //@}

    FWTOOLS_API void timeModified()
    {
        m_timeStamp->modified();
    }

    //------------------------------------------------------------------------------

    FWTOOLS_API void setLifePeriod(::fwCore::TimeStamp::TimeStampType p)
    {
        m_timeStamp->setLifePeriod(p);
    }

    //------------------------------------------------------------------------------

    FWTOOLS_API ::fwCore::TimeStamp::TimeStampType getLifePeriod()
    {
        return m_timeStamp->getLifePeriod();
    }

    //------------------------------------------------------------------------------

    FWTOOLS_API bool periodExpired()
    {
        return m_timeStamp->periodExpired();
    }

    //------------------------------------------------------------------------------

    ::fwCore::TimeStamp::sptr  getTimeStamp()  const
    {
        return m_timeStamp;
    }

protected:

    ::fwCore::TimeStamp::sptr m_timeStamp;

    SPTR(::fwTools::UUID) m_uuid;

private:

    /// Standard copy operator, forbidden.
    Object& operator=(const Object& _obj);

    /// Name identification for an object
    std::string m_strName;
};

}

#endif /* __FWTOOLS_OBJECT_HPP__ */
