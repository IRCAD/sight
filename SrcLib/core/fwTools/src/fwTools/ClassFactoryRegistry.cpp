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


namespace fwTools {

::boost::shared_ptr<ClassFactoryRegistry>  ClassFactoryRegistry::m_instance = ::boost::shared_ptr<ClassFactoryRegistry>();


ClassFactoryRegistry::ClassFactoryRegistry()
{
}



ClassFactoryRegistry::~ClassFactoryRegistry()
{
}




::boost::shared_ptr<ClassFactoryRegistry>
ClassFactoryRegistry::getDefault()
{
        if (m_instance==NULL)
        {
                m_instance = ::boost::shared_ptr<ClassFactoryRegistry>(new ClassFactoryRegistry);
        }
        //OSLM_TRACE("ClassFactoryRegistry::getDefault() return addr " << m_instance);
        return  m_instance;
}



const ClassFactoryRegistry::ManagedElementContainer
&ClassFactoryRegistry::managedElements()
{
        assert( ClassFactoryRegistry::getDefault()->m_registry.size() == ClassFactoryRegistry::getDefault()->m_managedElements.size() );
        return ClassFactoryRegistry::getDefault()->m_managedElements;

}




void ClassFactoryRegistry::addFactory(::boost::shared_ptr<IClassFactory> factory)
{
        assert( factory );

        OSLM_TRACE("ClassFactoryRegistry::addFactory " 
            << ::fwCore::Demangler(factory).getRootedClassname() << " addr=" << factory << "exist=" <<
                            (ClassFactoryRegistry::getDefault()->m_registry.find(factory)!=ClassFactoryRegistry::getDefault()->m_registry.end()) );
        bool firstInsertion = ClassFactoryRegistry::getDefault()->m_registry.insert( factory ).second;
        OSLM_TRACE( "ClassFactoryRegistry::addFactory(this= " << ClassFactoryRegistry::getDefault() << ") "
                                        << getString(factory->baseClassId()) << "-"
                                        << getString(factory->subClassId())  << "-"
                                        << getString(factory->keyId() )      << " keyVal="
                                        << factory->stringizedKey()
                                        <<   (firstInsertion?" firstInsertion":" secondInsertion") );
        if ( firstInsertion )
        {
                ManagedElement me( factory->baseClassId(), factory->subClassId() ,factory->keyId() );
                ClassFactoryRegistry::getDefault()->m_managedElements.push_back( me );
        }
}




std::list< ::boost::shared_ptr< IClassFactory > > ClassFactoryRegistry::getFactories(const ClassFactoryRegistry::ManagedElement &me)
{
        std::list< ::boost::shared_ptr< IClassFactory > > result;

        BOOST_FOREACH( ::boost::shared_ptr< IClassFactory > factory, getDefault()->m_registry )
        {
                bool candidate = false;
                candidate     = ( keyComparatorEquality(me.m_baseClass, typeid(void))  ||
                                                  keyComparatorEquality( me.m_baseClass ,  factory->baseClassId())
                                                )
                                        &&      ( keyComparatorEquality(me.m_subClass , typeid(void))  ||
                                                  keyComparatorEquality(me.m_subClass  , factory->subClassId())
                                                )
                                        &&      ( keyComparatorEquality(me.m_keyType , typeid(void))   ||
                                                  keyComparatorEquality(me.m_keyType   , factory->keyId())
                                                );
                if (candidate)
                {
                        result.push_back(factory);
                }
        }

        return result;
}




ClassFactoryRegistry::FactoryContainer &ClassFactoryRegistry::getFactories()
{
        return getDefault()->m_registry;
}


//const ClassFactoryRegistry::FactoryContainer &ClassFactoryRegistry::getFactories() const
//{
//      return getDefault()->m_registry;
//}



} // end namespace fwTools {

