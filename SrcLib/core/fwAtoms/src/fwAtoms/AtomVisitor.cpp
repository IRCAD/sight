/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwAtoms/AtomVisitor.hpp"
#include "fwAtoms/Policy.hpp"

namespace fwAtoms
{

AtomVisitor::AtomVisitor( ::fwAtoms::Policy& policy, ::fwAtoms::Object::sptr visitedAtom )
: m_policy(policy), m_visitedAtom(visitedAtom)
{}

AtomVisitor::~AtomVisitor()
{}


void AtomVisitor::visit()
{
    m_policy.processAttributes( m_visitedAtom->getAttributes() );
    m_policy.processMetaInfos( m_visitedAtom->getMetaInfos() );
}




} // namespace fwAtoms


