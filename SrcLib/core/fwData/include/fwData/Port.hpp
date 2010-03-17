/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PORT_HPP_
#define _FWDATA_PORT_HPP_

#include "fwData/config.hpp"
#include "fwData/Object.hpp"

namespace fwData
{
/**
 * @class   Port
 * @brief   A port is defined by an identifier and a type.
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Port : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Port)(::fwData::Object::Baseclass), (()), ::fwTools::Factory::New< Port >) ;

    /// Constructor
    FWDATA_API Port();

    /// Destructor
    FWDATA_API virtual ~Port();

    fwGettersSettersDocMacro(Identifier, identifier, std::string,the port identifier (example : "ID_SIZEX", "THRESHOLD_LOW") );

    fwGettersSettersDocMacro(Type, type, std::string, the port type (example : "Image", "Mesh", a %data className));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Port::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Port::csptr _source );

protected :

    //! port identifier
    std::string  m_identifier; // example "ID_SIZEX", "THRESHOLD_LOW"
    //! port type
    std::string  m_type ; // example "Image, Mesh" un className de data
};

}

#endif // _FWDATA_PORT_HPP_
