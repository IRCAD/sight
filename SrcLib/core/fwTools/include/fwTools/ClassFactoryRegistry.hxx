/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

#include <fwTools/TBClassFactory.hpp>
#include <fwTools/TBKClassFactory.hpp>

namespace fwTools {


// return first find association
template<class BASECLASS, class KEY>
::boost::shared_ptr<BASECLASS >
ClassFactoryRegistry::create(const KEY &key)  throw(std::invalid_argument)
{
    // search factories which produce sub of type BASECLASS and can deal with KEY
    // when found try if the key match
    typename ClassFactoryRegistry::FactoryContainer::iterator f;
    typedef  TBKClassFactory<BASECLASS,KEY>   TFactory;

     OSLM_INFO( "ClassFactoryRegistry::create" )
     f = getDefault()->m_registry.begin();
     while ( f != getDefault()->m_registry.end() )
     {
        if (    keyComparatorEquality(  (*f)->baseClassId() ,  typeid(BASECLASS) )
             && keyComparatorEquality(  (*f)->keyId() ,  typeid(KEY))
             && ::boost::dynamic_pointer_cast< TFactory >( (*f) )                             // for diamond inheritance of BASECLASS if any
            )
        {
            ::boost::shared_ptr<TFactory>  tfactory = ::boost::dynamic_pointer_cast< TFactory >( (*f) );
            assert( tfactory );
            if ( tfactory->canHandle(key) )
            {
                return tfactory->create();
            }

        }
        ++f;
     }

     return ::boost::shared_ptr< BASECLASS >(); // return an empty BASECLASS // not found
}


template<class BASECLASS,class KEY >
std::list< ::boost::shared_ptr<BASECLASS > >
ClassFactoryRegistry::subClasses(const KEY &key)  throw(std::invalid_argument)
{
    std::list< ::boost::shared_ptr<BASECLASS> > subClasses;

    FactoryContainer::iterator f;
    typedef  TBKClassFactory<BASECLASS,KEY>   TFactory;

     f = getDefault()->m_registry.begin();
     while ( f != getDefault()->m_registry.end() )
     {
            if (    keyComparatorEquality(  (*f)->baseClassId() ,  typeid(BASECLASS))
                     && keyComparatorEquality(  (*f)->keyId() ,  typeid(KEY))
                     && ::boost::dynamic_pointer_cast< TFactory >( (*f) ) // for diamond inheritance of BASECLASS if any
               )
        {
            ::boost::shared_ptr<TFactory>  tfactory = ::boost::dynamic_pointer_cast< TFactory >( (*f) );
            assert( tfactory );
            if ( tfactory->canHandle(key) )
            {
                subClasses.push_back( tfactory->create() ) ;
            }

        }
        ++f;
     }

    return subClasses;
}


template<class BASECLASS >
std::list< ::boost::shared_ptr<BASECLASS > >
ClassFactoryRegistry::subClasses()  throw(std::invalid_argument)
{
    std::list< ::boost::shared_ptr<BASECLASS> > subClasses;

    FactoryContainer::iterator f;
    typedef  TBClassFactory<BASECLASS>   TFactory;

     f = getDefault()->m_registry.begin();
     while ( f != getDefault()->m_registry.end() )
     {
                                                                 // for diamond inheritance of BASECLASS if any
        if ( keyComparatorEquality(  (*f)->baseClassId() ,  typeid(BASECLASS) )   && ::boost::dynamic_pointer_cast< TFactory >( (*f) ) )
        {
            ::boost::shared_ptr<TFactory>  tfactory = ::boost::dynamic_pointer_cast< TFactory >( (*f) );
            assert( tfactory );
            subClasses.push_back( tfactory->create() ) ;
        }
        ++f;
     }

    return subClasses;
}


} //end namespace fwTools


