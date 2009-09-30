/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef TBKCLASSFACTORY_H_
#define TBKCLASSFACTORY_H_

#include "fwTools/TBClassFactory.hpp"
#include "fwTools/Stringizer.hpp"




namespace fwTools {

/**
 * @class 	TBKClassFactory
 * @brief   an inheritance hierarchy helper for ClassFactory
 * @tparam  BASECLASS the type of base class
 * @tparam	KEYTYPE the type of the
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 *
 * This class is an helper/factorization for ClassFactory class which use 3 templates, BASECLASS, SUBCLASS, KEYTYPE.
 * This class inherit TBClassFactory<BASECLASS> and is responsible to store the keyType Identifier ( using a std::type_info)
 * and the value of the key.
 * Several method are provided to manipulate the key. Note that key value is stored using a const reference to allow manipulation
 * of non-copiable class like std::type_info
 *
 */
template <class BASECLASS , class KEYTYPE>
class  TBKClassFactory : public TBClassFactory<BASECLASS>
{
public:

	/// the type of base class
	typedef BASECLASS BaseClass;

	/// the type of the Key
	typedef KEYTYPE		  KeyType;

	/**
	 * @brief Default destructor : do nothing.
	 */
	 virtual ~TBKClassFactory() {};


	/**
	 * @brief	get the key type identifier
	 * @return the key type identifier as a std::type_info
	 */
	virtual const std::type_info &keyId() const
	{
		return typeid(KeyType);
	}


    /**
	 * @brief 	Inform if the action produced by the factory can Handle de the given Object
	 * @param[in] key the key value to evaluate
	 * @return 	true iff this class can Handle the given key i.e if the key have the same value
	 * @note comparison is performed using fwTools::keyComparatorEquality() you can Use template specialization to change ordering policy
	 */
	virtual bool canHandle(const KeyType &key) const
	{
		#ifdef _WIN32
			#pragma warning( disable : 4800 )
		#endif

		//OSLM_INFO( "canHandle(const KEY &key) with key="<<stringizedKey() << "result="<< keyComparatorEquality( m_keyValue, key )  );
		return keyComparatorEquality( m_keyValue, key );

		#ifdef _WIN32
			#pragma warning( default : 4800 )
		#endif
	}

    /**
	 * @brief 	Inform if the key stored in factory is lower than the given key
	 * @param[in] key the key value to evaluate
	 * @return 	true if the stored key value is lower than the given one
	 * @note comparison is performed using fwTools::keyComparatorLess() . you can Use template specialization to change ordering policy
	 */
	virtual bool isMyKeyLower(const KeyType &key) const
	{
		#ifdef _WIN32
			#pragma warning( disable : 4800 )
		#endif

		return keyComparatorLess( m_keyValue, key );

		#ifdef _WIN32
			#pragma warning( default : 4800 )
		#endif
	}

	/**
	 * @brief 	Construction is delegated to derived classes
	 * @return an instance of a specialized class (=SubClass) of  BaseClass
	 */
	virtual ::boost::shared_ptr< BaseClass > create() const = 0;

	/**
	 * @brief 	Return key value
	 * @return a const reference on the key value ( due to allowing management of non-copiable class)
	 */
	virtual const KeyType &keyValue() const
	{
		return m_keyValue;
	}


	/**
	 * @brief 	Return stringized key
	 * @return a string representation of the stored key ( using fwTools::getString() )
	 */
	 std::string stringizedKey() const
	 {
		 //return ::fwTools::getString<KEY>(m_keyValue);
		 return m_keyStringized;
	 }



protected :

	/**
	 * @brief Constructor : only derived base class can be instantiated
	 */
	TBKClassFactory(const KeyType &key) : m_keyValue(key), m_keyStringized( ::fwTools::getString<KEYTYPE>(m_keyValue) )
	{};

	const KeyType & m_keyValue;
	std::string m_keyStringized;

private :
	/// Constructor purposely not implemented
	TBKClassFactory();

};

} // end namespace fwTools {

#endif /*TBKCLASSFACTORY_H_*/
