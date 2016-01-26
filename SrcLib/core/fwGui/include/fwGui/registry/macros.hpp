/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUI_REGISTRY_MACROS_HPP__
#define __FWGUI_REGISTRY_MACROS_HPP__

#include <boost/preprocessor/cat.hpp>

#include "fwGui/GuiBaseObject.hpp"
#include "fwGui/registry/detail.hpp"

namespace fwGui
{
namespace registry
{

#define fwGuiRegisterMacro( GuiClassname, GuiFunctorKey )             \
    static ::fwGui::GuiBaseObject::Registrar< GuiClassname >          \
    BOOST_PP_CAT( s__factory__record__, __LINE__) ( GuiFunctorKey );

} // end namespace registry
} // end namespace fwGui

#endif /*__FWGUI_REGISTRY_MACROS_HPP__*/
