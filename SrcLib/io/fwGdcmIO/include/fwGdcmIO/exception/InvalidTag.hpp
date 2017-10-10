/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_EXCEPTION_INVALIDTAG_HPP__
#define __FWGDCMIO_EXCEPTION_INVALIDTAG_HPP__

#include "fwGdcmIO/config.hpp"

#include <fwCore/Exception.hpp>

namespace fwGdcmIO
{
namespace exception
{

/**
 * @brief InvalidTag exception
 */
class FWGDCMIO_CLASS_API InvalidTag : public ::fwCore::Exception
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWGDCMIO_API InvalidTag(const std::string& message);

    /// Destructor
    FWGDCMIO_API virtual ~InvalidTag() throw ();

};

} // namespace exception
} // namespace fwGdcmIO


#endif // __FWGDCMIO_EXCEPTION_INVALIDTAG_HPP__

