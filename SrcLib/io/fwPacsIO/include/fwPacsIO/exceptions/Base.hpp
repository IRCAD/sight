/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPACSIO_EXCEPTIONS_BASE_HPP__
#define __FWPACSIO_EXCEPTIONS_BASE_HPP__

#include "fwPacsIO/config.hpp"

#include <fwCore/Exception.hpp>

namespace fwPacsIO
{
namespace exceptions
{

/**
 * @brief Base class of fwPacsIO Exceptions
 */
class FWPACSIO_CLASS_API Base : public fwCore::Exception
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWPACSIO_API Base(const std::string& message);

    /// Destructor
    FWPACSIO_API virtual ~Base() throw ();

};

} //exceptions
} //fwPacsIO


#endif /* __FWPACSIO_EXCEPTIONS_BASE_HPP__ */
