/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_NAMESPACE_HPP__
#define __FWDICOMIOEXT_NAMESPACE_HPP__

/**
 * @brief fwDicomIOExt contains classes used to pull Dicom images from a pacs using dcmtk library.
 * @date 2013.
 */
namespace fwDicomIOExt
{

/**
 * @brief data contains data classes used to configure pacs connections.
 * @date 2013.
 */
namespace data
{
}

/**
 * @brief exceptions contains exceptions classes used to handle connection and communication errors.
 * @date 2013.
 */
namespace exceptions
{
}

/**
 * @brief dcmtk contains classes that use DCMTK to handle DICOM files and PACS connections.
 * @date 2013.
 */
namespace dcmtk
{

/**
 * @brief helper contains helper classes needed to deal with the DICOM standard.
 * @date 2013.
 */
namespace helper
{
}

/**
 * @brief reader contains readers used to read the differents image types.
 * @date 2013.
 */
namespace reader
{

/**
 * @brief main contains reader helpers to read default images.
 * @date 2013.
 */
namespace main
{
}

/**
 * @brief rgblookup contains reader helpers to read images that require a rgb lookup.
 * @date 2013.
 */
namespace rgblookup
{
}

}

}

}

/**
 * @brief gdcm contains classes that use DCMTK to handle DICOM files.
 * @date 2013.
 */
namespace gdcm
{

/**
 * @brief helper contains helper classes needed to deal with the DICOM standard.
 * @date 2013.
 */
namespace helper
{
}

}

}

#endif /* __FWDICOMIOEXT_NAMESPACE_HPP__ */

