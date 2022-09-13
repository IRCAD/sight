/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/factory/new.hpp"
#include "data/Object.hpp"

namespace sight::data
{

/**
 * @brief A port is defined by an identifier and a type.
 */
class DATA_CLASS_API Port final : public Object
{
public:

    SIGHT_DECLARE_CLASS(Port, Object, factory::New<Port>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Port(Object::Key key);

    /// Destructor
    inline ~Port() noexcept override = default;

    /**
     * @{
     * @brief Retrieve the identifier.
     */
    std::string& getIdentifier();
    const std::string& getIdentifier() const;
    void setIdentifier(const std::string& _identifier);
    /// @}

    /**
     * @{
     * @brief Retrieve the type.
     */

    std::string& getType();
    const std::string& getType() const;
    void setType(const std::string& _type);
    /// @}

    /**
     * @{
     * @brief Retrieve the input data.
     */
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Port& other) const noexcept;
    DATA_API bool operator!=(const Port& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

protected:

    //! port identifier
    std::string m_identifier; // example "ID_SIZEX", "THRESHOLD_LOW"
    //! port type
    std::string m_type; // example "Image, Mesh" un className de data
};

//-----------------------------------------------------------------------------

inline std::string& Port::getIdentifier()
{
    return this->m_identifier;
}

//-----------------------------------------------------------------------------

inline const std::string& Port::getIdentifier() const
{
    return this->m_identifier;
}

//-----------------------------------------------------------------------------

inline void Port::setIdentifier(const std::string& _identifier)
{
    this->m_identifier = _identifier;
}

//-----------------------------------------------------------------------------

inline std::string& Port::getType()
{
    return this->m_type;
}

//-----------------------------------------------------------------------------

inline const std::string& Port::getType() const
{
    return this->m_type;
}

//-----------------------------------------------------------------------------

inline void Port::setType(const std::string& _type)
{
    this->m_type = _type;
}

//-----------------------------------------------------------------------------

} // namespace sight::data
