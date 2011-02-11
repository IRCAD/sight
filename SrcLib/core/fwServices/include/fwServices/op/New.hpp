///* ***** BEGIN LICENSE BLOCK *****
// * FW4SPL - Copyright (C) IRCAD, 2009-2010.
// * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
// * published by the Free Software Foundation.
// * ****** END LICENSE BLOCK ****** */
//
//#ifndef _FWSERVICES_NEW_HPP_
//#define _FWSERVICES_NEW_HPP_
//
//#include <fwTools/Object.hpp>
//#include <fwRuntime/ConfigurationElement.hpp>
//
//namespace fwServices
//{
//
///**
// * @name Factory methods
// */
//
////@{
//
///**
// * @brief Build an object, initializes its content, attach and configure services, all being described by the XML like structure _elt
// * @return the resulting object
// *
// * It supports a tree like description as it is recursivelly performed to child object also having services.
// * This indirectly include XSD schema based _elt and service configuration checking. Template based writting is supported (i.e. myObjectType::sptr = New< myObjectType >(_elt)).
// * @author IRCAD (Research and Development Team).
// */
//FWSERVICES_API ::fwTools::Object::sptr New(::fwRuntime::ConfigurationElement::sptr _elt) ;
//
//template < class CLASSNAME >
//SPTR(CLASSNAME) New(::fwRuntime::ConfigurationElement::sptr _elt) ;
//
//
///**
// * @brief Add services to object from a configuration element, this method is used in ::fwServices::New method.
// * @author IRCAD (Research and Development Team).
// */
//FWSERVICES_API void addServicesToObjectFromCfgElem( ::fwTools::Object::sptr _obj, ::fwRuntime::ConfigurationElement::sptr _cfgElement );
//
////@}
//
//
//
//
//}
//
//#include "fwServices/op/New.hxx"
//
//#endif /*HELPER_HPP_*/
