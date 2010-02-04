/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_COMPOSITE_HPP_
#define _FWDATA_COMPOSITE_HPP_

#include <map>
#include <boost/shared_ptr.hpp>
#include "fwData/config.hpp"

#include <fwData/Object.hpp>

namespace fwData
{
/**
 * @class 	Composite
 * @brief 	This class defines a composite object.
 *
 * Composite contains a map of ::fwData::Object.
 *
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */

class FWDATA_CLASS_API Composite : public Object{
public:
	fwCoreClassDefinitionsWithFactoryMacro( (Composite)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Composite >) ;

	typedef std::map< std::string, Object::sptr > Container;

	/// Constructor
	FWDATA_API Composite();

	/// Destructor
	FWDATA_API virtual ~Composite();

	/// @brief get the container of ::fwData::Object
	FWDATA_API Container &getRefMap();
	/// @brief get the container of ::fwData::Object
	FWDATA_API Container const &getRefMap() const;

    FWDATA_API Composite & operator=( const Composite & _composite );

protected:
	/// container of ::fwData::Object
	Container m_map;
};
}
#endif /* _FWDATA_COMPOSITE_HPP_ */

