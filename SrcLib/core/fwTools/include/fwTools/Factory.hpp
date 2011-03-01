/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_FACTORY_HPP_
#define _FWTOOLS_FACTORY_HPP_

#include <string>
#include <list>
#include <boost/foreach.hpp>
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include <fwCore/base.hpp>
#include <fwCore/LogicStamp.hpp>

#include "fwTools/Failed.hpp"
#include "fwTools/Object.hpp"
#include "fwTools/ClassFactoryRegistry.hpp"
#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @brief   The purpose of this class is to provide a data Object Factory and  initialized them
 * @class   Factory
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 * The purpose of this class is to provide a data Object Factory which produce initialized data.
 *
 * Data can be initialized (typically install default service)
 * by install initializer functors (  specialization Factory::Initializer ). The installation is performed using Factory::addInitializer() method.
 *
 * To produce the data this factory use the ClassFactoryRegistry scheme. You have to use REGISTER_BINDING macros and to specify ::fwTools::Object as BaseClass and your Concrete Object
 * class name as SubClass, you can use any type for the key. This key will be used to retrieve the object using classFactory::create< ::fwTools::Object >(mykeyvalue)
 *
 */
class FWTOOLS_CLASS_API Factory
{
public :

    /**
     * @brief define the Interface for Initializer Functors. This functor is to be added to the Factory class using  Factory::addInitializer()
     * end user must must implement ::boost::weak_ptr< fwTools::Object >
     */
    class FWTOOLS_CLASS_API Initializer : public ::fwCore::BaseObject
    {
    public :
        fwCoreNonInstanciableClassDefinitionsMacro((Initializer)(::fwCore::BaseObject));

        /// default constructor
        FWTOOLS_API Initializer() {} ;

        /// default destructor
        FWTOOLS_API virtual ~Initializer() {} ;

        /**
         * @brief method <b>to implement</b>. When the Object is produced by the Factory.
         * The Factory will process Initializer::init() on this object for all registered Initializers
         * @param[in] _obj  the object to be initialized (weak ptr)
         */
        FWTOOLS_API virtual void init( ::fwTools::Object::wptr _obj ) = 0;

        /**
         * @brief Uninitializes data with the LogicStamp 'key'
         */
        FWTOOLS_API virtual void uninit( ::fwCore::LogicStamp::csptr key ) = 0;
    };


    /**
     * @brief   Return a smart pointer on the specialized data required
     * @param[in] a full rooted className (i.e "::fwData::Image")
     * @return   smart pointer on created Object (Empty smart pointer if not found)
     */
    FWTOOLS_API static ::fwTools::Object::sptr buildData(const std::string &className);

    /**
     * @brief Uninitializes data with the LogicStamp 'key'
     */
    FWTOOLS_API static void uninitData(::fwCore::LogicStamp::csptr key );

    /**
     * @brief build an object from template CLASSNAME and call each Initializer::init() to this object
     * @tparam CLASSNAME the class name to build
     * @return a smart pointer to the new & initialized object
     */
    template<class CLASSNAME >
    static ::boost::shared_ptr< CLASSNAME > New();

    /**
     * @brief Return a smart pointer data required from a given key registered by a macro REGISTER_BINDING...( ...,  KeyType, keyvalue )
     * This factory construct a new Object from a key which is registered using a REGISTER_BINDING macro
     * @tparam KEYTYPE the type of the key used
     * @param[in] key a const reference on the key value. We use a const reference to allow also keys which are no copy constructible class like std::type_info
     * @return smart pointer on created ( and initialized ) Object (Empty smart pointer if not found)*
     * @warning using Factory::New("::fwData::Image") will use a KEYTYPE of type <b>char *</b>. So if you do not register with char * (generally std::string are used)
     * the factory cannot produce the required Object
     * @todo  do an explicit specialization of this method for char * to avoid  mistake
     */
    // reference on KEYTYPE mandatory to also enable non copiable key for example typeid
    template<class KEYTYPE>
    static ::fwTools::Object::sptr New(const KEYTYPE &key);

    /**
     * @brief add a new Initializer to the Factory. This initializer will be processed on the object created by Factory
     */
    FWTOOLS_API static void addInitializer( Initializer::sptr _init ) ;

    /// class destructor
    FWTOOLS_API virtual ~Factory();

protected:

    FWTOOLS_API Factory();

    FWTOOLS_API static std::list< Initializer::sptr > m_initializers;
};



template< class CLASSNAME >
::boost::shared_ptr< CLASSNAME > Factory::New()
{
    ::boost::shared_ptr< CLASSNAME > newObject( new CLASSNAME ) ;
    OSLM_FATAL_IF("Factory::buildData unable to build class from key=" << ::fwTools::getString(typeid(CLASSNAME)), !newObject);
    if ( newObject == 0 )
    {
        std::string mes = "Factory::buildData unable to build class from key=";
        mes += ::fwTools::getString(typeid(CLASSNAME));
        throw ::fwTools::Failed(mes);
    }

    BOOST_FOREACH( Initializer::sptr initializer , m_initializers )
    {
        initializer->init( newObject );
    }

    return newObject;
}



template<class KEYTYPE>
::fwTools::Object::sptr Factory::New(const KEYTYPE &key)
{
    BOOST_STATIC_ASSERT( ::boost::is_pointer<KEYTYPE>::value == false );

    ::fwTools::Object::sptr newObject( ::fwTools::ClassFactoryRegistry::create< ::fwTools::Object >( key ) );
    OSLM_FATAL_IF("Factory::buildData unable to build class from key=" << ::fwTools::getString(key), !newObject);
    if ( newObject == 0 )
    {
        std::string mes = "Factory::buildData unable to build class from key=";
        mes += ::fwTools::getString(key);
        throw ::fwTools::Failed(mes);
    }

    BOOST_FOREACH( Initializer::sptr initializer , m_initializers )
    {
        initializer->init( newObject );
    }

    return newObject;
}

} // namespace fwTools

#endif //_FWTOOLS_FACTORY_HPP_
