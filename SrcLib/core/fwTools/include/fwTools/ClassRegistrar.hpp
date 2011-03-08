/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef CLASSREGISTRAR_HPP_
#define CLASSREGISTRAR_HPP_

#include <boost/preprocessor/cat.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <fwCore/base.hpp>
#include <fwCore/Demangler.hpp>

#include "fwTools/ClassFactory.hpp"
#include "fwTools/ClassFactoryRegistry.hpp"


/*
 * @brief Macro to simply register an Class ( subclass of baseClass) on Object
 * @author  IRCAD (Research and Development Team).
 */

//The preprocessor token-pasting operator (##) prevents arguments from expanding.  This macro allows its arguments to expand before concatenation.
//Concatenation must not result in an invocation of a macro that uses BOOST_PP_CAT.  If that happens, BOOST_PP_CAT will not expand the second time.


#define REGISTER_BINDING( BaseClassType , SubClassType , KeyType, keyvalue ) \
    REGISTER_BINDING_ID_CPY( BaseClassType , SubClassType , KeyType, keyvalue, __LINE__ )

#define REGISTER_BINDING_ID_REF( BaseClassType , SubClassType , KeyType, keyvalue, id )                                                            \
    static const KeyType & BOOST_PP_CAT(registredKeyValue, id ) = keyvalue;                                                                        \
    static fwTools::ClassRegistrar< BaseClassType, SubClassType ,KeyType > BOOST_PP_CAT( registrar, id ) ( BOOST_PP_CAT(registredKeyValue, id ) );

#define REGISTER_BINDING_ID_CPY( BaseClassType , SubClassType , KeyType, keyvalue, id )                                                            \
    static const KeyType  BOOST_PP_CAT(registredKeyValue, id ) = keyvalue;                                                                         \
    static fwTools::ClassRegistrar< BaseClassType, SubClassType ,KeyType > BOOST_PP_CAT( registrar, id ) ( BOOST_PP_CAT(registredKeyValue, id ) );


#define REGISTER_BINDING_BYCLASSNAME( BaseClassType , SubClassType, Key ) \
    REGISTER_BINDING_ID_CPY( BaseClassType , SubClassType , std::string , (::fwCore::TypeDemangler<Key>().getClassname()) , __LINE__ )



namespace fwTools {

/**
 * @brief This is an helper for registring an Class : internaly it create the factory and register it
 * to FactoryRegistry
 * @class ClassRegistrar
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 */
template<class BASECLASS, class SUBCLASS,class KEY>
class ClassRegistrar
{
public:

    /**
     * @brief Constructor : create the factory and register it
     * @param[in] key type that factory can candle
     */
    ClassRegistrar(const KEY & key)
    {
        // create factory
        ::boost::shared_ptr< IClassFactory >  af( new ClassFactory< BASECLASS,SUBCLASS,KEY >(key) );
        // register it
        OSLM_DEBUG( "Class Factory Registration BaseClass=" << getString( af->baseClassId() )
                    << " SubClass =" << getString( af->subClassId() )
                    << " KeyClass=" << getString( af->keyId() )
                    << " KeyValue=" << af->stringizedKey()
                  )
        ClassFactoryRegistry::addFactory( af );
    }

};

} //end namespace fwTools {


#endif /*CLASSREGISTRAR_HPP_*/
