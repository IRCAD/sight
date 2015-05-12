/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_PORT_HPP__
#define __FWDATA_PORT_HPP__

#include "fwData/config.hpp"
#include "fwData/Object.hpp"
#include "fwData/factory/new.hpp"

fwCampAutoDeclareDataMacro((fwData)(Port), FWDATA_API);

namespace fwData
{
/**
 * @brief A port is defined by an identifier and a type.
 */
class FWDATA_CLASS_API Port : public ::fwData::Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Port)(::fwData::Object), (()), ::fwData::factory::New< Port >);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Port(::fwData::Object::Key key);

    /// Destructor
    FWDATA_API virtual ~Port();

    fwGettersSettersDocMacro(Identifier, identifier, std::string,
                             "the port identifier (example : 'ID_SIZEX', 'THRESHOLD_LOW')" );

    fwGettersSettersDocMacro(Type, type, std::string, "the port type (example : 'Image', 'Mesh', a %data className)");

    fwCampMakeFriendDataMacro((fwData)(Port));

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

protected:

    //! port identifier
    std::string m_identifier;  // example "ID_SIZEX", "THRESHOLD_LOW"
    //! port type
    std::string m_type;   // example "Image, Mesh" un className de data
};

} // namespace fwData

#endif // __FWDATA_PORT_HPP__

