/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMS_METAVISITOR_HPP_
#define __FWATOMS_METAVISITOR_HPP_

#include <fwCamp/camp/ExtendedClassVisitor.hpp>
#include "fwAtoms/Object.hpp"
#include "fwAtoms/Policy.hpp"

#include "fwAtoms/config.hpp"

namespace fwAtoms
{

/**
 * @brief This class offer the possibility to visit a MetaData and manage his execution with a Policy
 */
class FWATOMS_CLASS_API AtomVisitor: public ::camp::ExtendedClassVisitor
{
public:
    /**
     * @brief Construct a new AtomVisitor.
     * @param policy the policy to manage visitation @see Policy.
     * @param metaObject the visited metaObject.
     */
    FWATOMS_API AtomVisitor(Policy& policy, Object::sptr metaObject);

    /**
     * @brief Meta visitor destructor
     */
    FWATOMS_API ~AtomVisitor();


    /**
     * @brief visit MapProperty(the map of Object attributes).
     * Call Policy::processAttributes method
     */
    FWATOMS_API virtual void visit(const camp::MapProperty& property);

private:
    Policy& m_policy;
    camp::UserObject m_userObj;

};



}  // namespace fwAtoms

#endif /* __FWATOMS_METAVISITOR_HPP_ */
