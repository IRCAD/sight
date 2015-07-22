/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_EXCEPTIONS_BASE_HPP__
#define __FWDICOMIOEXT_EXCEPTIONS_BASE_HPP__

#include <fwCore/Exception.hpp>

#include "fwDicomIOExt/config.hpp"

namespace fwDicomIOExt
{
namespace exceptions
{

/**
 * @class Base
 * @brief Base class of fwDicomIOExt Exceptions
 * @date  2013.
 */
class FWDICOMIOEXT_CLASS_API Base : public fwCore::Exception
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWDICOMIOEXT_API Base(const std::string& message);

    /// Destructor
    FWDICOMIOEXT_API virtual ~Base() throw ();

};

} //exceptions
} //fwDicomIOExt


#endif /* __FWDICOMIOEXT_EXCEPTIONS_BASE_HPP__ */
