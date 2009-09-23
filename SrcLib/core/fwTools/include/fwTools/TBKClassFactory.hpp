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
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo 	Complete doxygen
 */
template <class BASECLASS , class KEY>
class  TBKClassFactory : public TBClassFactory<BASECLASS>
{
public:

	typedef BASECLASS BaseClass;

	typedef KEY		  Key;

	/**
	 * @brief Default destructor : do nothing.
	 */
	 virtual ~TBKClassFactory() {};


	/**
	 * @brief	Return the key type indentifier
	 */
	virtual const std::type_info &keyId() const
	{
		return typeid(KEY);
	}


    /**
	 * @brief 	Inform if the action produced by the factory can Handle de the given Object
	 * @return 	true iff Class can Handle the Object. i.e test if object and OBJECT have the same type_info
	 */
	virtual bool canHandle(const KEY &key) const
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
	 */
	virtual bool isMyKeyLower(const KEY &key) const
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
	 */
	virtual ::boost::shared_ptr< BaseClass > create() const = 0;

	/**
	 * @brief 	Return key value
	 */
	virtual const KEY &keyValue() const
	{
		return m_keyValue;
	}


	/**
	 * @brief 	Return stringized key
	 */
	 std::string stringizedKey() const
	 {
		 //return ::fwTools::getString<KEY>(m_keyValue);
		 return m_keyStringized;
	 }



protected :

	/// Constructor : only derived base class can be instantiated
	TBKClassFactory(const KEY &key) : m_keyValue(key), m_keyStringized( ::fwTools::getString<KEY>(m_keyValue) )
	{};

	const KEY & m_keyValue;
	std::string m_keyStringized;

private :
	/// Constructor purposely not implemented
	TBKClassFactory();

};

} // end namespace fwTools {

#endif /*TBKCLASSFACTORY_H_*/
