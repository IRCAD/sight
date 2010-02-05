/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_DL_NAMEDECORATOR_HPP_
#define _FWRUNTIME_DL_NAMEDECORATOR_HPP_



#include "fwRuntime/dl/INameDecorator.hpp"



namespace fwRuntime
{

namespace dl
{



/**
 * @brief       Implements a default name decorator. This class is intended to be overriden
 *                      to do plateform specific decorations.
 * @class       NameDecorator
 * @date        2004-2009
 * @author      IRCAD (Research and Development Team).
 */
struct NameDecorator : public INameDecorator
{

        /**
         * @name                Overrides
         */
        //@{
        const std::string getNativeName( const std::string & abstractName ) const throw();
        //@}


protected:

        /**
         * @brief       Retrieves the native file prefix.
         *
         * @remark      The default implements does nothing and should be overridden.
         *
         * @return      a string containing the native file prefix
         */
        virtual const std::string getNativePrefix() const throw();

        /**
         * @brief       Retrieves the native file suffix.
         *
         * @remark      The default implements adds the release or debug suffix.
         *
         * @return      a string containing the native file suffix
         */
        virtual const std::string getNativeSuffix() const throw();

        /**
         * @brief       Retrieves the native file extension.
         *
         * @remark      The default implements does nothing and should be overridden.
         *
         * @return      a string containing the native file extension
         */
        virtual const std::string getNativeExtension() const throw();

};



} // namespace dl

} // namespace fwRuntime



#endif /*_FWRUNTIME_DL_NAMEDECORATOR_HPP_*/
