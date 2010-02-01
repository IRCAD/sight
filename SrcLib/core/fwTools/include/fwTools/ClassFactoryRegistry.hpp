/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef CLASSFACTORYREGISTRY_HPP_
#define CLASSFACTORYREGISTRY_HPP_

#include <vector>
#include <set>
#include <list>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <stdexcept>

#include "fwTools/config.hpp"
#include "fwTools/TypeInfo.hpp"
#include "fwTools/IClassFactory.hpp"

//VAGRM
#include <iostream>
#include "fwTools/Stringizer.hpp"
//using namespace boost;

namespace fwTools {


class IClassFactory;


/**
 * @brief 	Class which store all factories for combination BASECLASS,SUBCLASS,KEY
 * @note 	This class is a singleton
 * @class	ClassFactoryRegistry.
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 *
 */
class FWTOOLS_CLASS_API ClassFactoryRegistry
{
public:

	FWTOOLS_API struct ManagedElement
	{
		ManagedElement(const TypeInfo &base, const TypeInfo &sub,const TypeInfo &key)
		: m_baseClass(base), m_subClass(sub),m_keyType(key)
		{ }

		TypeInfo m_baseClass;
		TypeInfo m_subClass;
		TypeInfo m_keyType;
	};

	typedef std::vector<ManagedElement > ManagedElementContainer;


	/**
	 * @brief define a order between factory.
	 * The ordering is defined as a lexical ordering with tuple ( BASECLASS,SUBCLASS,KEYTYPE ). element of tuple are
	 * compared using their string representation
	 */
	struct FactoryWeakOrderer
	{
		bool operator()(::boost::shared_ptr<IClassFactory> f1, ::boost::shared_ptr<IClassFactory> f2) const
		{
			//return ( !((*f1)==(*f2)) ) && (f1 < f2); // factory which produce same things (but can be different by ptr)
			//are considerer to be equal
			//return ( !((*f1)==(*f2)) ) && (f1->stringizedKey() < f2->stringizedKey() );
#ifdef CLASSFACTORYORDERDEBUG
			SLM_TRACE ( "FactoryWeakOrderer:");
			OSLM_TRACE ( "  f1 " << getString(f1->baseClassId()) << "#"
							  << getString(f1->subClassId())    << "#"
							  << getString(f1->keyId() )        << " #keyVal="
							  << f1->stringizedKey() );
			OSLM_TRACE ( "  f2 " << getString(f2->baseClassId()) << "#"
								 << getString(f2->subClassId())  << "#"
								 << getString(f2->keyId() )      << " #keyVal="
							     << f2->stringizedKey() );
#endif
			bool result;

			result = (*f1) < (*f2);
#ifdef CLASSFACTORYORDERDEBUG
			OSLM_TRACE ( "  result=" << result );
#endif
			return result;

		}
	};

    /// Container of all SUBCLASS factories
	typedef std::set< ::boost::shared_ptr<IClassFactory> ,FactoryWeakOrderer > FactoryContainer;


	/// Destructor : do nothing
	FWTOOLS_API virtual ~ClassFactoryRegistry();


	/**
	 * @brief Add the factory in registry
	 */
	FWTOOLS_API static void addFactory(::boost::shared_ptr< IClassFactory > factory);


	/**
	 * @brief Return factories which correspond to me. typeid(void) in me act as wildCard
	 */
	FWTOOLS_API static std::list< ::boost::shared_ptr< IClassFactory > > getFactories(const ManagedElement &me);

	/**
	 * @brief Return ALL factories
	 */
	FWTOOLS_API static FactoryContainer &getFactories();

	/*
		 * @brief return ALL factories (const version)
		 */
	//FWTOOLS_API static const FactoryContainer &getFactories();

	/**
	 * @brief Retreive managed Element by ClassFactoryRegistry
	 */
	FWTOOLS_API static const ManagedElementContainer &managedElements();


	/**
	 * @brief Return *the* SubClass instance ( derivated form Baseclass) which is related to given key value.
	 * @note That KEY is template so it can be of any type
	 * @warning It use is only valid when *ONLY ONE* BASECLASS-SUBCLASS registration for a KEY type (aka subClasses(myKey).size()<2 )
	 * @return The SubClass instance or a null smart ptr if not found
	 */
	template<class BASECLASS, class KEY>
	static ::boost::shared_ptr<BASECLASS > create(const KEY &key)  throw(std::invalid_argument);

	/**
	 * @brief Return the  list of SubClass instances ( derivated form Baseclass) registred in the registry with KEY value
	 * @return The  list of SubClass instances or an empty list if not found
	 */
	template<class BASECLASS, class KEY>
	static std::list< ::boost::shared_ptr<BASECLASS > >  subClasses(const KEY &key)  throw(std::invalid_argument);


	/**
	 * @brief Return the  list of SubClass instances ( derivated form Baseclass) registred in the registry for all Key
	 * @return The  list of SubClass instances or an empty list if not found
	 */
	template<class BASECLASS>
	static std::list< ::boost::shared_ptr<BASECLASS > >  subClasses()  throw(std::invalid_argument);


//	/*
//	 * @brief return the  map < key , SubClass>  of instances ( SubClass derivated form Baseclass) registred in the registry for all Key
//	 * return empty map if not found
//	 */DOESNT WORK BECAUSE const KEY & is not manager when copying map
//	 */
//	template<class BASECLASS, class KEY>
//	static std::map< const KEY &, ::boost::shared_ptr<BASECLASS> >  mapKeySubClasses()  throw(std::invalid_argument);





protected :

	/// @brief Default constructor : do nothing.
	FWTOOLS_API ClassFactoryRegistry();

	/// @brief Return the default Instance
	FWTOOLS_API static ::boost::shared_ptr<ClassFactoryRegistry> getDefault();

	static ::boost::shared_ptr<ClassFactoryRegistry> m_instance;




	/// the container for the factories
	FactoryContainer m_registry;

	/// Container BaseClass-SubClass-KeyType Info
	ManagedElementContainer	m_managedElements;

};

} //end namespace fwTools {

#include "fwTools/ClassFactoryRegistry.hxx"



#endif /*CLASSFACTORYREGISTRY_HPP_*/
