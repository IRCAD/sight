/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/visitor/accept.hpp"

namespace fwData
{
namespace visitor
{

void accept( ::fwTools::Object::sptr object,  ::fwData::visitor::IVisitor *visitor )
{
        assert(visitor);

        visitor->visit(object);
        visitor->next( object, object->children() );
}

} // namespace visitor

} // namespace fwData
