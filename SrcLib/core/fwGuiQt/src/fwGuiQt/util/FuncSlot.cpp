/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2004-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwGuiQt/util/FuncSlot.hpp>

namespace fwGuiQt
{

namespace util
{

void doNothing()
{
}

FuncSlot::FuncSlot() : m_func( doNothing )
{
}

void FuncSlot::trigger()
{
    m_func();
}

} // namespace util

} // namespace fwGuiQt

