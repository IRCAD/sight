/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#ifndef __FWDCMTKIO_NAMESPACE_HPP__
#define __FWDCMTKIO_NAMESPACE_HPP__

/// fwDcmtkIO contains classes used to pull Dicom images from a pacs using dcmtk library.
namespace fwDcmtkIO
{

/// helper contains helper classes needed to deal with the DICOM standard.
namespace helper
{
} //namespace helper

/// reader contains readers used to read the differents image types.
namespace reader
{

/// main contains reader helpers to read default images.
namespace main
{
} //namespace main

/// rgblookup contains reader helpers to read images that require a rgb lookup.
namespace rgblookup
{
} //namespace rgblookup

} //namespace reader

} //namespace fwDcmtkIO


#endif /* __FWDCMTKIO_NAMESPACE_HPP__ */
