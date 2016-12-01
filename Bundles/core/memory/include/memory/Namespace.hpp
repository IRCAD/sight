/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
