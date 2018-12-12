/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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
