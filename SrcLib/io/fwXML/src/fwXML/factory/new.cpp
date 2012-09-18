/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/XML/XMLTranslator.hpp"
#include "fwXML/factory/new.hpp"


namespace fwXML
{

namespace factory
{

::fwXML::XMLTranslator::sptr New( const ::fwXML::registry::KeyType & classname )
{
    return ::fwXML::registry::get()->create(classname);
}

} // namespace factory

} // namespace fwXML


