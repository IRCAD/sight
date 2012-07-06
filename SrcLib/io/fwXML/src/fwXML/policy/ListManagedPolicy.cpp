/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>

#include "fwXML/policy/ListManagedPolicy.hpp"

namespace fwXML
{

bool ListManagedPolicy::split(::fwData::Object::sptr obj)
{
    std::string className = obj->getLeafClassname();
    return m_classNames.find( className )!= m_classNames.end();
}

void ListManagedPolicy::addSplitClass(const std::string &name)
{
    m_classNames.insert( name );
}

} //namespace fwXML
