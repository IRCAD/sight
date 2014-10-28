/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWATOMS_EXCEPTION_HPP_
#define _FWATOMS_EXCEPTION_HPP_

#include <exception>
#include <string>

#include <fwCore/Exception.hpp>
#include "fwAtoms/config.hpp"

namespace fwAtoms
{

/**
 * @brief   Implements atoms exception
 */
struct FWATOMS_CLASS_API Exception : public ::fwCore::Exception
{

    /**
     * @brief       Constructor
     *
     * @param[in]   message a string containing the failure message
     */
    FWATOMS_API Exception(const std::string &message) throw();

    /**
     * @brief   Destructor : do nothing.
     */
    FWATOMS_API ~Exception() throw();


};

} // namespace fwAtoms

#endif // _FWATOMS_EXCEPTION_HPP_

