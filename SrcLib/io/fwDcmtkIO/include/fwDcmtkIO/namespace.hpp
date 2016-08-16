/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
