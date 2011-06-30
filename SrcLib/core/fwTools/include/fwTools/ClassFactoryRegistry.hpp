/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef CLASSFACTORYREGISTRY_HPP_
#define CLASSFACTORYREGISTRY_HPP_

#include <map>
#include <iostream>
#include <stdexcept>

#include <boost/shared_ptr.hpp>

#include "fwTools/config.hpp"
#include "fwTools/TypeInfo.hpp"
#include "fwTools/IClassFactory.hpp"
#include "fwTools/Stringizer.hpp"

namespace fwTools
{

class IClassFactory;


/**
 * @brief   Class which store all factories for combination BASECLASS,SUBCLASS,KEY
 * @note    This class is a singleton
 * @class   ClassFactoryRegistry.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 *
 */
class FWTOOLS_CLASS_API ClassFactoryRegistry
{
public:

    typedef ::boost::shared_ptr< ::fwTools::ClassFactoryRegistry > sptr;

    typedef std::string KeyType;
    typedef std::map< KeyType, ::fwTools::IClassFactory::sptr > FactoryContainer;
    typedef std::string BaseClassType;
    typedef std::map< BaseClassType, FactoryContainer> FactoryContainerMap;

    /// Destructor : do nothing
    FWTOOLS_API virtual ~ClassFactoryRegistry();

    /**
     * @brief Return ALL factories
     */
    FWTOOLS_API static FactoryContainerMap &getFactories();

    /**
     * @brief Return *the* SubClass instance ( derivated form Baseclass) which is related to given key value.
     * @note That KEY is template so it can be of any type
     * @warning It use is only valid when *ONLY ONE* BASECLASS-SUBCLASS registration for a KEY type (aka subClasses(myKey).size()<2 )
     * @return The SubClass instance or a null smart ptr if not found
     */
    template<class BASECLASS, class KEY>
    static ::boost::shared_ptr< BASECLASS > create(const KEY &key)  throw(std::invalid_argument);

    FWTOOLS_API static ::fwTools::IClassFactory::sptr getFactory(BaseClassType base, KeyType key);

    FWTOOLS_API static void addFactory(::fwTools::IClassFactory::sptr factory);

protected :

    /// @brief Default constructor : do nothing.
    FWTOOLS_API ClassFactoryRegistry();

    /// @brief Return the default Instance
    FWTOOLS_API static ::fwTools::ClassFactoryRegistry::sptr getDefault();

    static ::fwTools::ClassFactoryRegistry::sptr m_instance;

    /// the container for the factories
    FactoryContainerMap m_registry;

};

} //end namespace fwTools

#include "fwTools/ClassFactoryRegistry.hxx"



#endif /*CLASSFACTORYREGISTRY_HPP_*/
