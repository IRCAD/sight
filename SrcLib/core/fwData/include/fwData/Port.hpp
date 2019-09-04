/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
    fwCoreClassDefinitionsWithFactoryMacro( (Port)(::fwData::Object), ::fwData::factory::New< Port >);

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
    std::string& getIdentifier ();
    const std::string& getIdentifier () const;
    void setIdentifier (const std::string& _identifier);
    /// @}

    /**
     * @{
     * @brief Retrieve the type.
     */

    std::string& getType ();
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

inline std::string& Port::getIdentifier ()
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

inline std::string& Port::getType ()
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
