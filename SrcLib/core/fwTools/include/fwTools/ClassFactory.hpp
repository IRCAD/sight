/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef CLASSBINDER_HPP_
#define CLASSBINDER_HPP_

#ifdef _WIN32
   #pragma warning( disable : 4800 )
#endif

#include <typeinfo>
#include <iostream>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/is_base_of.hpp>
#include <boost/concept_check.hpp>

#include "fwTools/TBKClassFactory.hpp"


/**
 * @brief 		The namespace fwTools
 * @namespace	fwTools.
 * @author		IRCAD (Research and Development Team).
 * @date		2007-2009.
 * @todo 		Comment namespace
 */
namespace fwTools {


/**
 * @brief 	The ClassFactory class is a generic class factory
 * @class	ClassFactory.
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 *
 *
 * The ClassFactory class is a factory  which create an instance of type SUBCLASS for an instance of type Key :
 * \li BASECLASS is the mother of SubClasses (SUBCLASS) and define the common API.
 * \li KEY can be of any type int , std::string, std::type_info etc...
 *
 *  this factory produce an sharedPtr independantly of KEY value this responsability is managed by ClassFactoryRegistry
 */




template<class BASECLASS, class SUBCLASS, class KEY>
class ClassFactory : public TBKClassFactory<BASECLASS,KEY>
{
public:

	/// Notify at compile time iff KEY is not egality comparable in a human readable format
	BOOST_CLASS_REQUIRE( KEY , boost, EqualityComparableConcept);

	/// typedef Concrete
	typedef ClassFactory<BASECLASS, SUBCLASS, KEY> ConcreteFactory;


	/// typedef of Class Filter
	typedef BASECLASS BaseClass;

	/// typedef defining the type of SUBCLASS which can provide this factory. SubClass is a base class of BaseClass
	typedef SUBCLASS SubClass;

	/// typedef defining the type that factory can candle
	typedef KEY Key;

	/// Default constructor
	ClassFactory(const KEY &key) : TBKClassFactory<BASECLASS,KEY>(key)
	{
		/// Ensure that BASECLASS is the mother class of SUBCLASS or SUBCLASS itself ;
		BOOST_STATIC_ASSERT(( boost::is_same< BASECLASS, SUBCLASS >::value ||   boost::is_base_of< BASECLASS, SUBCLASS >::value  ));
	};

	/// Default destructor : do nothing.
	virtual ~ClassFactory() {};

	/// @brief Return the derivated (sub) class indentifier
	virtual const std::type_info &subClassId() const
	{
		return typeid(SubClass);
	}


	/**
	 * @brief Create the SUBCLASS
	 * @return A shared ptr on an BaseClass of type SUBCLASS
	 */
	virtual ::boost::shared_ptr< BaseClass > create() const
	{
		::boost::shared_ptr< BaseClass > bc( new SubClass );
		return bc;
	}


	 virtual bool operator==(const IClassFactory &other) const
	 {
//		 std::cout << std::endl << "=== test"<< std::endl;
//		 std::cout << " this : " << this->baseClassId().name() << "-" << this->subClassId().name() << "-" << this->keyId().name() << " : " << this->stringizedKey()  << std::endl;
//		 std::cout << " other : " << other.baseClassId().name() << "-" << other.subClassId().name() << "-" << other.keyId().name() << " : " << other.stringizedKey() << std::endl;
//		 std::cout << " RESULT:";

		 bool sameTypeInfo =     ( keyComparatorEquality( this->baseClassId() , other.baseClassId()   )  )
							  && ( keyComparatorEquality( this->subClassId()  , other.subClassId()  )  )
							  && ( keyComparatorEquality( this->keyId()       , other.keyId() 	    )  );
		 if ( !sameTypeInfo )
		 {
//			 std::cout << " NOT equal (typeid)"<<std::endl;
			 return false;
		 }
		 else
		 {
			 const ConcreteFactory &concreteFactory =  dynamic_cast< const ConcreteFactory &>(other);
			 const Key &otherKey = concreteFactory.keyValue();

//			 std::cout << ( this->canHandle(otherKey) ? " equal" : " NOT equal (keyvalue)" )  << " [concreteFactory=" << typeid(other).name()<<"]"<< std::endl;
			 return this->canHandle(otherKey);
		 }

	 }

	 virtual bool operator<(const IClassFactory &other) const
	 {
#ifdef CLASSFACTORYORDERDEBUG
		 SLM_TRACE("test op<");
#endif
			if ( keyComparatorLess( this->baseClassId(), other.baseClassId()) ) {return true;}
			if ( keyComparatorLess( other.baseClassId(), this->baseClassId())  ){return false;}
#ifdef CLASSFACTORYORDERDEBUG
			SLM_TRACE("bases are equals");
#endif
			if ( keyComparatorLess( this->subClassId(), other.subClassId() ) ) {return true;}
			if ( keyComparatorLess( other.subClassId(), this->subClassId() ) ) {return false;}
#ifdef CLASSFACTORYORDERDEBUG
			SLM_TRACE("subclasses are equals");
#endif
			if ( keyComparatorLess( this->keyId() 	, other.keyId()      ) ) {return true;}
			if ( keyComparatorLess( other.keyId()   , this->keyId()      ) ) {return false;}
#ifdef CLASSFACTORYORDERDEBUG
			SLM_TRACE("key type are equals");
#endif
			if (  dynamic_cast< const ConcreteFactory *>(&other) ) // same factory Base-Sub-KeyType type
			{
				const ConcreteFactory &concreteFactory =  dynamic_cast< const ConcreteFactory &>(other);
				const Key &otherKey = concreteFactory.keyValue();
#ifdef CLASSFACTORYORDERDEBUG
				SLM_TRACE("keyLower" <<  isMyKeyLower(otherKey) );
#endif
				return isMyKeyLower(otherKey);
			}
#ifdef CLASSFACTORYORDERDEBUG
			OSLM_TRACE("return false");
#endif
			return false; //here  this->keyId().name() > other.keyId().name()

	 }


	protected :


	/// Constructor purposely not implemented
	ClassFactory();



};

} //end namespace fwTools {

#endif /*CLASSBINDER_HPP_*/
