/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
