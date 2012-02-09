/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_IMAGE_HPP_
#define _FWDATATOOLS_IMAGE_HPP_

#include <fwCore/base.hpp>

#include <fwTools/Type.hpp>

#include <fwData/Image.hpp>

#include "fwDataTools/export.hpp"

namespace fwDataTools
{


/**
 * @brief   This helper generates a image.
 * @class   Image.
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 *
 */
class Image : public ::fwCore::BaseObject
{

public :

    fwCoreClassDefinitionsWithFactoryMacro((Image)( ::fwCore::BaseObject ),
                                           (()),
                                           new Image
                                          );
    fwCoreAllowSharedFromThis();

    /**
     * @brief Initialize 'rand' seed
     */
    FWDATATOOLS_API static void initRand();

    /**
     * @brief Generate an image with the given informations. Buffer is filled with 0.
     * @param image     image to generate
     * @param dim       image dimension (3 for 3D)
     * @param size      vector of image size
     * @param spacing   vector of image spacing
     * @param origin    vector of image origin
     * @param type      image type
     */
    FWDATATOOLS_API static void generateImage(::fwData::Image::sptr image,
                                              ::fwData::Image::SizeType size,
                                              std::vector<double> spacing,
                                              std::vector<double> origin,
                                              ::fwTools::Type type);

    /// Generate an image with random information (size, spacing, ...). Buffer is filled with random values.
    FWDATATOOLS_API static void generateRandomImage(::fwData::Image::sptr image, ::fwTools::Type type);

    /**
     * @brief compare two images.
     * @return true if the images are equal.
     * @note if false, the image difference is written in SLM log
     */
    FWDATATOOLS_API static bool compareImage(::fwData::Image::sptr image1, ::fwData::Image::sptr image2);

    /**
     * @brief compare image information.
     * @return true if the image has the given information.
     * @note if false, read SLM log
     */
    FWDATATOOLS_API static bool compareImage(::fwData::Image::sptr image1,
                                             ::fwData::Image::SizeType size,
                                             std::vector<double> spacing,
                                             std::vector<double> origin,
                                             ::fwTools::Type type);
    /**
     * @brief compare two arrays.
     * @return true if the arrays are equal.
     */
    FWDATATOOLS_API static bool compareArray(::fwData::Array::sptr buff1, ::fwData::Array::sptr buff2);

    /// Fill array with random value
    FWDATATOOLS_API static void randomizeArray(::fwData::Array::sptr array);

protected:


    //! @brief Constructor.
    FWDATATOOLS_API Image();

    //! @brief Destructor.
    FWDATATOOLS_API virtual ~Image();

};

} // namespace fwDataTools

#endif // _FWDATATOOLS_IMAGE_HPP_
