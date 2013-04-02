/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWDATACAMP_VISITOR_RECURSIVELOCK_HPP__
#define  __FWDATACAMP_VISITOR_RECURSIVELOCK_HPP__

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include "fwDataCamp/config.hpp"

namespace fwData
{
    class Object;

    namespace mt
    {
        class ObjectReadLock;
    }
}

namespace fwDataCamp
{

namespace visitor
{

/**
 * @class   RecursiveLock
 * @brief   This class is an helper to lock an object and all its children recursively.
 *
 * @date    2013.
 */
class FWDATACAMP_CLASS_API RecursiveLock : public ::camp::ExtendedClassVisitor
{

public:

    /**
     * @brief Container definition dedicated to object locks storage.
     */
    typedef std::map< std::string,  SPTR( ::fwData::mt::ObjectReadLock )> LockMapType;

    /**
     * @brief Constructor.
     * Creates a recursive lock onto the given object and recursively locks it.
     * This recursive lock can be built using a given container of previously acquired locks.
     *
     * @param object data object (containing child data objects) to lock recursively
     * @param locks container of previously acquired locks
     */
    FWDATACAMP_API RecursiveLock( SPTR(::fwData::Object) object,
                                  SPTR(LockMapType) locks = SPTR(LockMapType)(new LockMapType) );

    FWDATACAMP_API virtual ~RecursiveLock();

    /**
     * @name Introspection methods implementation
     * @{ */
    FWDATACAMP_API void visit(const camp::SimpleProperty& property);
    FWDATACAMP_API void visit(const camp::EnumProperty& property);
    FWDATACAMP_API void visit(const camp::UserProperty& property);
    FWDATACAMP_API void visit(const camp::ArrayProperty& property);
    FWDATACAMP_API void visit(const camp::Function& function);
    FWDATACAMP_API virtual void visit(const camp::MapProperty& property);
    /**  @} */

private :

    /// Locks associated object recursively (called by constructor).
    void lock();


    /// Object given in constructor which will introspected
    SPTR(::fwData::Object) m_object;

    /// Reflection in camp world of m_object
    ::camp::UserObject m_campObj;

    /// Container of acquired locks.
    SPTR(LockMapType) m_locks;

};

} // namespace visitor

} // namespace fwDataCamp

#endif // __FWDATACAMP_VISITOR_RECURSIVELOCK_HPP__

