/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __MEMORY_NAMESPACE_HPP__
#define __MEMORY_NAMESPACE_HPP__

/**
 * @ingroup   apprequirement
 * @brief     The namespace memory contains tools to manage memory. It is used for dump.
 *            It allows to define the buffer loading mode for the application (lazy and direct) at the start of this
 *            bundle. Add it in the application's \p \<requirement\> section.
 *
 * To configure the loading mode (here 'lazy') , add the following line in your application's Properties.cmake
 * @code{cmake}
    bundleParam(memory PARAM_LIST loading_mode PARAM_VALUES lazy)
   @endcode
 */
namespace memory
{

}
#endif /* __MEMORY_NAMESPACE_HPP__ */
