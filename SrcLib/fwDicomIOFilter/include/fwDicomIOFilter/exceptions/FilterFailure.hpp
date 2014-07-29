/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_EXCEPTIONS_FILTERFAILURE_HPP__
#define __FWDICOMIOFILTER_EXCEPTIONS_FILTERFAILURE_HPP__

#include <fwCore/Exception.hpp>

#include "fwDicomIOFilter/config.hpp"

namespace fwDicomIOFilter
{
namespace exceptions
{

/**
 * @class FilterFailure
 * @brief FilterFailure Exceptions
 * @date  2014.
 */
class FWDICOMIOFILTER_CLASS_API FilterFailure : public fwCore::Exception
{
public:
    /**
     * @brief Constructor
     * @param[in] message Exception message
     */
    FWDICOMIOFILTER_API FilterFailure(const std::string& message);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~FilterFailure() throw ();

};

} // namespace exceptions
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_EXCEPTIONS_FILTERFAILURE_HPP__ */
