/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_FACTORY_HXX_
#define _FWDATA_FACTORY_HXX_

#include <fwTools/Factory.hpp>

namespace fwData
{

template< class CLASSNAME >
::boost::shared_ptr< CLASSNAME > Factory::New()
{
    typename CLASSNAME::sptr newObject = ::fwTools::Factory::New< CLASSNAME >();
    OSLM_ASSERT("Sorry, this object is not a ::fwData::Object.", ::fwData::Object::dynamicCast( newObject ) );
    return newObject;
}

} // namespace fwTools

#endif //_FWDATA_FACTORY_HXX_
