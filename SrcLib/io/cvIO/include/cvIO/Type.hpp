/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CVIO_TYPE_HPP__
#define __CVIO_TYPE_HPP__

#include "cvIO/config.hpp"

#include <fwTools/Type.hpp>

namespace cvIO
{

class CVIO_CLASS_API Type
{
public:
    /// Returns an OpenCV format given a FW4SPL pixel type and a number of components
    CVIO_API static std::int32_t toCv(const ::fwTools::Type _type, const size_t _numComponents);

    /// Returns a FW4SPL pixel type and a number of components given an OpenCV format
    CVIO_API static std::pair< ::fwTools::Type, std::uint8_t> fromCv(std::int32_t _cvType);
};

} // namespace cvIO

#endif //__CVIO_TYPE_HPP__
