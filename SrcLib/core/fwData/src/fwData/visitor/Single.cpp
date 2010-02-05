/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwData/visitor/Single.hpp"

namespace fwData
{
namespace visitor
{

void Single::next( ::fwTools::Object::sptr src, ::fwTools::Object::ChildContainer &fields)
{
        m_source = src;
        // perform nothing with child;
}

} // namespace visitor

} // namespace fwData
