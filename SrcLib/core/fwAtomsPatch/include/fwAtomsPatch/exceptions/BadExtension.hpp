/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWATOMSPATCH_EXCEPTIONS_BADEXTENSION_HPP__
#define __FWATOMSPATCH_EXCEPTIONS_BADEXTENSION_HPP__

#include "fwAtomsPatch/exceptions/Base.hpp"

namespace fwAtomsPatch
{

namespace exceptions
{

/**
 * @brief Exception defining a wrong extension in a filename.
 */
class BadExtension : public fwAtomsPatch::exceptions::Base
{
public:
    BadExtension(const std::string& message);
    virtual ~BadExtension() noexcept
    {
    }
};

} // namespace exceptions
} // namespace fwAtomsPatch

#endif /* __FWATOMSPATCH_EXCEPTIONS_BADEXTENSION_HPP__ */
