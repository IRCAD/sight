/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPACSIO_EXCEPTIONS_REQUESTFAILURE_HPP__
#define __FWPACSIO_EXCEPTIONS_REQUESTFAILURE_HPP__

#include "fwPacsIO/config.hpp"
#include "fwPacsIO/exceptions/Base.hpp"

namespace fwPacsIO
{
namespace exceptions
{

/**
 * @class RequestFailure
 * @brief RequestFailure Exceptions
 * @date  2013.
 */
class FWPACSIO_CLASS_API RequestFailure : public fwPacsIO::exceptions::Base
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWPACSIO_API RequestFailure(const std::string& message);

    /// Destructor
    FWPACSIO_API virtual ~RequestFailure() throw ();

};

} //exceptions
} //fwPacsIO


#endif /* __FWPACSIO_EXCEPTIONS_REQUESTFAILURE_HPP__ */
