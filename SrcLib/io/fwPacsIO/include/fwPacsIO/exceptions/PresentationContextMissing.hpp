/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPACSIO_EXCEPTIONS_PRESENTATIONCONTEXTMISSING_HPP__
#define __FWPACSIO_EXCEPTIONS_PRESENTATIONCONTEXTMISSING_HPP__

#include "fwPacsIO/config.hpp"
#include "fwPacsIO/exceptions/Base.hpp"

namespace fwPacsIO
{
namespace exceptions
{

/**
 * @brief PresentationContextMissing Exceptions
 */
class FWPACSIO_CLASS_API PresentationContextMissing : public fwPacsIO::exceptions::Base
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWPACSIO_API PresentationContextMissing(const std::string& message);

    /// Destructor
    FWPACSIO_API virtual ~PresentationContextMissing() noexcept;

};

} //exceptions
} //fwPacsIO


#endif /* __FWPACSIO_EXCEPTIONS_PRESENTATIONCONTEXTMISSING_HPP__ */
