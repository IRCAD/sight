/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST_GENERATOR_IMAGE_HPP__
#define __FWTEST_GENERATOR_IMAGE_HPP__

#include "fwTest/config.hpp"

#include <fwCore/base.hpp>

#include <fwData/Image.hpp>

#include <fwDataTools/helper/Array.hpp>

#include <fwMath/Compare.hpp>

#include <fwTools/Type.hpp>


namespace fwTest
{
namespace generator
{


/**
 * @brief   This class contains helper to generate images.
 */
class Image
{

public:

    /**
     * @brief Initialize 'rand' seed
     */
    FWTEST_API static void initRand();

    /**
     * @brief Generate an image with the given informations. Buffer is filled with 0.
     * @param image     image to generate
     * @param size      vector of image size
     * @param spacing   vector of image spacing
     * @param origin    vector of image origin
     * @param type      image type
     */
    FWTEST_API static void generateImage(::fwData::Image::sptr image,
                                         ::fwData::Image::SizeType size,
                                         std::vector<double> spacing,
                                         std::vector<double> origin,
                                         ::fwTools::Type type);

    /// Generate an image with random information (size, spacing, ...). Buffer is filled with random values.
    FWTEST_API static void generateRandomImage(::fwData::Image::sptr image, ::fwTools::Type type);

    /// Fill array with random value
    FWTEST_API static void randomizeArray(::fwData::Array::sptr array);

    /// Creates an Array with the given type and size and fills buffer with random values.
    FWTEST_API static ::fwData::Array::sptr createRandomizedArray(const std::string& type,
                                                                  ::fwData::Array::SizeType sizes);

};

} // namespace generator
} // namespace fwTest

#endif // __FWTEST_GENERATOR_IMAGE_HPP__
