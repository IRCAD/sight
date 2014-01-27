/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C), 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_NAMESPACE_HPP__
#define __IODICOMEXT_NAMESPACE_HPP__

/**
 * @brief ioDicomExt contains services to read and preview Dicom images from a pacs and filesystem.
 * @date 2013.
 */
namespace ioDicomExt
{

/**
 * @brief common contains common services and classes used to manage the Dicom standard.
 * @date 2013.
 */
namespace common
{
/**
 * @brief controller contains controllers used to manage the Dicom standard.
 * @date 2013.
 */
namespace controller
{
}

/**
 * @brief data contains data object used for the reading process of Dicom images from a pacs.
 * @date 2013.
 */
namespace data
{
}

/**
 * @brief editor contains editors used to manage the Dicom standard.
 * @date 2013.
 */
namespace editor
{
}

}

// ----------------------------------------------------------------------------

/**
 * @brief dcmtk contains services that use the dcmtk library to manage the Dicom standard.
 * @date 2013.
 */
namespace dcmtk
{
/**
 * @brief controller contains controllers that use the dcmtk library to manage the Dicom standard.
 * @date 2013.
 */
namespace controller
{
}

/**
 * @brief editor contains editors that use the dcmtk library to manage the Dicom standard.
 * @date 2013.
 */
namespace editor
{
}

}

// ----------------------------------------------------------------------------

/**
 * @brief gdcm contains services that use the gdcm library to manage the Dicom standard.
 * @date 2013.
 */
namespace gdcm
{
/**
 * @brief controller contains controllers that use the gdcm library to manage the Dicom standard.
 * @date 2013.
 */
namespace controller
{
}

/**
 * @brief editor contains editors that use the gdcm library to manage the Dicom standard.
 * @date 2013.
 */
namespace editor
{
}

}


}

#endif /* __IODICOMEXT_NAMESPACE_HPP__ */

