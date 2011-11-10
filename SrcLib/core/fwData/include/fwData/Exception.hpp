/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_EXCEPTION_HPP_
#define _FWDATA_EXCEPTION_HPP_

#include <exception>
#include <string>

#include "fwCore/Exception.hpp"
#include "fwData/config.hpp"

namespace fwData
{

/**
 * @brief   Implements data exception class
 * @class   Exception
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
struct FWDATA_CLASS_API Exception : public ::fwCore::Exception
{

    /**
     * @brief       Constructor
     *
     * @param[in]   message a string containing the failure message
     */
    FWDATA_API Exception(const std::string &message) throw();

    /**
     * @brief   Destructor : do nothing.
     */
    FWDATA_API ~Exception() throw();


};

} // namespace fwData

#endif // _FWDATA_EXCEPTION_HPP_
