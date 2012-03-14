/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwXML/visitor/accept.hpp"

namespace fwData
{
namespace visitor
{

void accept( ::fwData::Object::sptr object,  ::fwData::visitor::IVisitor *visitor )
{
    SLM_ASSERT("visitor not instanced", visitor);
    visitor->visit( object );
}

} // namespace visitor

} // namespace fwData
