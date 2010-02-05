/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_DL_POSIXNAMEDECORATOR_HPP_
#define _FWRUNTIME_DL_POSIXNAMEDECORATOR_HPP_



#include "fwRuntime/dl/NameDecorator.hpp"



namespace fwRuntime
{

namespace dl
{


/**
 * @brief       Implements a posix specific name decorator.
 * @struct      DarwinNameDecorator
 * @date        2004-2009
 * @author      IRCAD (Research and Development Team).
 */
struct DarwinNameDecorator : public NameDecorator
{


protected:

        /**
         * @name                Ovrerides
         */
        //@{
        const std::string getNativePrefix() const throw();
        const std::string getNativeExtension() const throw();
        //@}

};



} // namespace dl

} // namespace fwRuntime



#endif /*_FWRUNTIME_DL_POSIXNAMEDECORATOR_HPP_*/
