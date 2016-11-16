/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOGDCM_PCH_HPP__
#define __IOGDCM_PCH_HPP__

// To include first because of Windows compilation
// The #ifdef prevents our hook to reformat the includes
#ifdef WIN32
#include <gdcmSurfaceHelper.h>
#include <gdcmUIDGenerator.h>
#endif // WIN32

#include <fwServices/IService.hpp>

#ifndef WIN32
#include <gdcmSurfaceHelper.h>
#include <gdcmUIDGenerator.h>
#endif // WIN32

#endif // __IOGDCM_PCH_HPP__
