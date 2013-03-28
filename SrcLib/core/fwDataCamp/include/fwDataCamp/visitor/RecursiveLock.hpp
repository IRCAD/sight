/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWDATACAMP_VISITOR_RECURSIVELOCK_HPP__
#define  __FWDATACAMP_VISITOR_RECURSIVELOCK_HPP__

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <fwCamp/camp/ExtendedClassVisitor.hpp>

#include <fwData/mt/ObjectReadLock.hpp>

#include "fwDataCamp/config.hpp"

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

    typedef std::map< std::string,  SPTR( ::fwData::mt::ObjectReadLock )> LockMapType;

    FWDATACAMP_API RecursiveLock( ::fwData::Object::sptr object,
                                  SPTR(LockMapType) locks = SPTR(LockMapType)(new LockMapType) );

    FWDATACAMP_API virtual ~RecursiveLock();

    FWDATACAMP_API void visit(const camp::SimpleProperty& property);
    FWDATACAMP_API void visit(const camp::EnumProperty& property);
    FWDATACAMP_API void visit(const camp::UserProperty& property);
    FWDATACAMP_API void visit(const camp::ArrayProperty& property);
    FWDATACAMP_API void visit(const camp::Function& function);
    FWDATACAMP_API virtual void visit(const camp::MapProperty& property);

private :

    void lock();


    /// Object given in constructor which will introspected
    ::fwData::Object::sptr m_object;

    /// Reflection in camp world of m_object
    ::camp::UserObject m_campObj;

    SPTR(LockMapType) m_locks;

};

} // namespace visitor

} // namespace fwDataCamp

#endif // __FWDATACAMP_VISITOR_RECURSIVELOCK_HPP__

