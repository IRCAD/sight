/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwData/config.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

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

    /**
     * @{
     * @brief Retrieve the identifier.
     */
    std::string& getRefIdentifier ();
    const std::string& getIdentifier () const;
    void setIdentifier (const std::string& _identifier);
    /// @}

    /**
     * @{
     * @brief Retrieve the type.
     */

    std::string& getRefType ();
    const std::string& getType () const;
    void setType (const std::string& _type);
    /// @}

    /**
     * @{
     * @brief Retrieve the input data.
     */
    fwCampMakeFriendDataMacro((fwData)(Port));
    /// @}

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

protected:

    //! port identifier
    std::string m_identifier;  // example "ID_SIZEX", "THRESHOLD_LOW"
    //! port type
    std::string m_type;   // example "Image, Mesh" un className de data
};

//-----------------------------------------------------------------------------

inline std::string& Port::getRefIdentifier ()
{
    return this->m_identifier;
}

//-----------------------------------------------------------------------------

inline const std::string& Port::getIdentifier () const
{
    return this->m_identifier;
}

//-----------------------------------------------------------------------------

inline void Port::setIdentifier (const std::string& _identifier)
{
    this->m_identifier = _identifier;
}

//-----------------------------------------------------------------------------

inline std::string& Port::getRefType ()
{
    return this->m_type;
}

//-----------------------------------------------------------------------------

inline const std::string& Port::getType () const
{
    return this->m_type;
}

//-----------------------------------------------------------------------------

inline void Port::setType (const std::string& _type)
{
    this->m_type = _type;
}

//-----------------------------------------------------------------------------

} // namespace fwData

