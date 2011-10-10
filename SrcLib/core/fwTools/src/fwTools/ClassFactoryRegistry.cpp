/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/foreach.hpp>

#include "fwTools/ClassFactoryRegistry.hpp"
#include "fwTools/Stringizer.hpp"
#include <fwCore/base.hpp>
#include <fwCore/Demangler.hpp>


namespace fwTools
{

ClassFactoryRegistry::sptr  ClassFactoryRegistry::m_instance = ::boost::shared_ptr<ClassFactoryRegistry>();

//------------------------------------------------------------------------------

ClassFactoryRegistry::ClassFactoryRegistry()
{}

//------------------------------------------------------------------------------

ClassFactoryRegistry::~ClassFactoryRegistry()
{}

//------------------------------------------------------------------------------

::fwTools::ClassFactoryRegistry::sptr ClassFactoryRegistry::getDefault()
{
    if (m_instance==NULL)
    {
        m_instance = ::boost::shared_ptr<ClassFactoryRegistry>(new ClassFactoryRegistry);
    }
    return  m_instance;
}

//------------------------------------------------------------------------------

ClassFactoryRegistry::FactoryContainerMap &ClassFactoryRegistry::getFactories()
{
    return getDefault()->m_registry;
}

//------------------------------------------------------------------------------

::fwTools::IClassFactory::sptr ClassFactoryRegistry::getFactory(BaseClassType base, KeyType key)
{
    ::fwTools::IClassFactory::sptr factory;
    if(getFactories().find(base) != getFactories().end() )
    {
        FactoryContainer &mapFactory =  getFactories()[base];
        if(mapFactory.find(key) != mapFactory.end() )
        {
            factory = mapFactory[key];
        }
    }
    return factory;
}

//------------------------------------------------------------------------------

void ClassFactoryRegistry::addFactory(::fwTools::IClassFactory::sptr factory)
{
    std::string key = factory->stringizedKey();
    std::string baseClass = ::fwTools::getString(factory->baseClassId());
    std::string subClass = ::fwTools::getString(factory->subClassId());

    ClassFactoryRegistry::FactoryContainerMap &factoryMap = getFactories();

    if(factoryMap.find(baseClass) == factoryMap.end() )
    {
        FactoryContainer mapFactory;
        mapFactory.insert(std::make_pair(key, factory));
        factoryMap.insert(std::make_pair(baseClass, mapFactory));
    }
    else
    {
        if(factoryMap[baseClass].find(key) == factoryMap[baseClass].end())
        {
            factoryMap[baseClass].insert(std::make_pair(key, factory));
        }
        else
        {
            OSLM_WARN("ClassFactory "<< baseClass << " - " << subClass << " - " << key << " already registered.");
        }
    }
}

//------------------------------------------------------------------------------

} // end namespace fwTools

