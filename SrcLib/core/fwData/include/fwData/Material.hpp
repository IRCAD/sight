/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _DATA_MATERIAL_HPP_
#define _DATA_MATERIAL_HPP_


#include "fwData/Object.hpp"
#include "fwData/Color.hpp"

namespace fwData {
/**
 * @class 	Material
 * @brief 	This class defines a material. A material is represented by an ambient color and a diffuse color
 * @see 	::fwData::Color
 *
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 */

class FWDATA_CLASS_API Material : public Object {
public:
	fwCoreClassDefinitionsWithFactoryMacro( (Material)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Material >) ;

	/// Constructor
	FWDATA_API Material();

	/// Destructor
	FWDATA_API virtual ~Material();

	/// Clone method
	Material::sptr clone() const;

	/**
	* @brief returns editable ambient color
	*/
	FWDATA_API Color &ambient() ;

	/**
	* @brief returns editable diffuse color
	*/
	FWDATA_API Color &diffuse() ;

	/**
	* @brief returns read-only  ambient color
	*/
	FWDATA_API const Color &ambient() const;

	/**
	* @brief returns read-only  diffuse color
	*/
	FWDATA_API const Color &diffuse() const;

protected :

    Color m_ambient;
    Color m_diffuse;
};

}

#endif /* _DATA_MATERIAL_HPP_ */
