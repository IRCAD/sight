/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_LOGICSTAMP_HPP
#define __FWCORE_LOGICSTAMP_HPP

#include "fwCore/base.hpp"
#include "fwCore/HiResClock.hpp"
 

namespace fwCore
{

/**
 * @brief Provides a Logical timestamp system.
 *
 *
 */
class FWCORE_CLASS_API LogicStamp : public BaseObject
{

public:
	fwCoreClassDefinitionsWithFactoryMacro( (LogicStamp)(BaseObject::Baseclass), (()), new LogicStamp);

    /** 
     * @brief Type used in logical typestamp.
     */
    typedef unsigned long LogicStampType;

    /** 
     * @name Constructor/Destructor
     * @{ */

    FWCORE_API LogicStamp() 
    {
        this->m_modifiedLogicalTime = 0;
    };

    /**  @} */


    /** 
     * @brief Increment global Logical counter and copy it to this LogicStamp.
     */
    FWCORE_API void modified();


    /** 
     * @return LogicStamp's current value
     */
    FWCORE_API LogicStampType getLogicStamp() const 
    {
        return this->m_modifiedLogicalTime;
    };


    /** 
     * @brief Greater than operator for LogicStamp.
     * 
     * @param ls LogicStamp to compare to
     */
    FWCORE_API bool operator>(LogicStamp& ls) 
    {
        return ( this->m_modifiedLogicalTime > ls.m_modifiedLogicalTime );
    };

    /** 
     * @brief Lesser than operator for LogicStamp.
     * 
     * @param ls LogicStamp to compare to
     */
    FWCORE_API bool operator<(LogicStamp& ls) 
    {
        return ( this->m_modifiedLogicalTime < ls.m_modifiedLogicalTime );
    };

    /** 
     * @brief Cast operator for LogicStamp.
     */
    FWCORE_API operator LogicStampType() const 
    {
        return this->m_modifiedLogicalTime;
    };


private:

    /** 
     * @brief Stored logical time
     */
    LogicStampType m_modifiedLogicalTime;

};

} //namespace fwCore

#endif // __FWCORE_LOGICSTAMP_HPP

