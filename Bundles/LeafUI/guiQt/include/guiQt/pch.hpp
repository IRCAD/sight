/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_PCH_HPP__
#define __GUIQT_PCH_HPP__

// Cause an internal compiler error on windows...
#ifndef _WIN32
#include <fwCom/HasSlots.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>
#endif // _WIN32

#include <fwServices/macros.hpp>

#include <gui/editor/IEditor.hpp>

#endif // __GUIQT_PCH_HPP__
