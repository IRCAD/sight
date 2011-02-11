///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */
//
//#ifndef _FWSERVICES_NEW_HXX_
//#define _FWSERVICES_NEW_HXX_
//
//#include <fwTools/ClassFactoryRegistry.hpp>
//#include <fwCore/Demangler.hpp>
//#include <fwTools/TypeInfo.hpp>
//#include "fwServices/bundle/Factory.hpp"
//#include "fwServices/IEditionService.hpp"
//
//namespace fwServices
//{
//
//template<class CLASSNAME>
//SPTR(CLASSNAME) New(::fwRuntime::ConfigurationElement::sptr _elt)
//{
//    SPTR(CLASSNAME) myObject = ::boost::dynamic_pointer_cast< CLASSNAME >( ::fwServices::New( _elt ) ) ;
//    assert(myObject);
//    return myObject;
//}
//
//
//} // namespace fwServices
//
//#endif // _FWSERVICES_FACTORY_HXX_
