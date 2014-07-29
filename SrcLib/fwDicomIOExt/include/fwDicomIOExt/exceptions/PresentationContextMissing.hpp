/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_EXCEPTIONS_PRESENTATIONCONTEXTMISSING_HPP__
#define __FWDICOMIOEXT_EXCEPTIONS_PRESENTATIONCONTEXTMISSING_HPP__

#include "fwDicomIOExt/exceptions/Base.hpp"
#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace exceptions
{

/**
 * @class PresentationContextMissing
 * @brief PresentationContextMissing Exceptions
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API PresentationContextMissing : public fwDicomIOExt::exceptions::Base
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWDICOMIOEXT_API PresentationContextMissing(const std::string& message);

    /// Destructor
    FWDICOMIOEXT_API virtual ~PresentationContextMissing() throw ();

};

} //exceptions
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_EXCEPTIONS_PRESENTATIONCONTEXTMISSING_HPP__ */
