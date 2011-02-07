/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWSERVICESVALIDATIONNAMESPACE_HPP_
#define FWSERVICESVALIDATIONNAMESPACE_HPP_

namespace fwServices
{
/**
 * @brief This namespace provides method to verify validity of configuration element.
 * @namespace   fwServices::validation
 *
 * Validation is based on :
 * \li Checking that involved objects and services type are reported either the Class Registry or in the component distribution
 * \li Checking that service to object association are compliant
 * \li Checking that both object and service configurations are valid (concern class attributes) with respect to XSD schema (warning when no XSD is provided)
 *
 * @note Validation can be completely performed on the base of the component distribution, without preloading or linking with dynamic libraries implementing classes.
 * @author IRCAD (Research and Development Team).
 */
namespace validation
{

}
}
#endif /* FWSERVICESVALIDATIONNAMESPACE_HPP_ */
