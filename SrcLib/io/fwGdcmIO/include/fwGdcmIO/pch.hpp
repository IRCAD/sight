/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_PCH_HPP__
#define __FWGDCMIO_PCH_HPP__

// We need this PCH because of Windows.
// These files need to be included first in each cpp, don't know why but this has been like this for ages...
// The #ifdef is just a hack to prevents sheldon from reordering the includes
#ifdef WIN32
#include <gdcmSurfaceHelper.h>
#include <gdcmUIDGenerator.h>
#endif // WIN32

#include <fwData/Image.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Equipment.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/Patient.hpp>
#include <fwMedData/Study.hpp>

#ifndef WIN32
#include <gdcmSurfaceHelper.h>
#include <gdcmUIDGenerator.h>
#endif // WIN32

#endif // __FWGDCMIO_PCH_HPP__
