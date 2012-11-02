/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWXML_FACTORY_NEW_HPP__
#define __FWXML_FACTORY_NEW_HPP__

#include <string>

#include <boost/make_shared.hpp>

#include <fwTools/macros.hpp>

#include "fwXML/config.hpp"
#include "fwXML/registry/detail.hpp"

namespace fwXML
{

class XMLTranslator;

namespace factory
{


template<class CLASSNAME > SPTR( CLASSNAME )  New();


FWXML_API SPTR( ::fwXML::XMLTranslator ) New(const ::fwXML::registry::KeyType & classname );


template<class CLASSNAME > SPTR( CLASSNAME )  New()
{
    SPTR(CLASSNAME) obj = ::boost::make_shared< CLASSNAME >();
    return obj;
}

} // namespace factory

} // namespace fwXML

#endif /* __FWXML_FACTORY_NEW_HPP__ */


