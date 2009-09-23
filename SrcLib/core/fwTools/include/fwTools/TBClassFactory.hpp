/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef TBCLASSFACTORY_H_
#define TBCLASSFACTORY_H_

#include "fwTools/IClassFactory.hpp"

namespace fwTools {

/**
 * @class 	TBClassFactory
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo 	Complete doxygen
 */
template <class BASECLASS >
class  TBClassFactory : public IClassFactory
{
public:

	typedef BASECLASS BaseClass;

	/**
	 * @brief 	Default constructor : do nothing.
	 */
	 TBClassFactory() {};

	 /**
	  * @brief 	Default destructor : do nothing.
	  */
	 virtual ~TBClassFactory() {};


	/**
	 * @brief	Return the baseclass identifier
	 */
	virtual const std::type_info &baseClassId() const
	{
		return typeid(BASECLASS);
	}

	/**
	 * @brief 	Construction is delegated to derived classes
	 */
	virtual ::boost::shared_ptr< BaseClass > create() const = 0;



protected :



};

} // end namespace fwTools {
#endif /*TBCLASSFACTORY_H_*/
