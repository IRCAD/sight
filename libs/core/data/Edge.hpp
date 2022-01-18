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

#include <utility>

SIGHT_DECLARE_DATA_REFLECTION((sight) (data) (Edge));

namespace sight::data
{

/**
 * @brief This class defines an edge object.
 *
 * An edge is represented by a to identifier ("from" and "to") and a nature.
 */
class DATA_CLASS_API Edge : public Object
{
public:

    SIGHT_DECLARE_CLASS(Edge, Object, factory::New<Edge>);
    SIGHT_MAKE_FRIEND_REFLECTION((sight) (data) (Edge));

    DATA_API static std::string NATURE_FLOW;
    DATA_API static std::string NATURE_DATA;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    DATA_API Edge(Object::Key key);
    /// Destructor
    DATA_API virtual ~Edge();

    /// @brief do a shallow copy of edge
    DATA_API void shallowCopy(const Object::csptr& _source) override;

    /**
     * @brief Set the edge identifier ("ID_SIZEX" , ...)
     * @param[in] _fromPortIndentifier identifier of port "from"
     * @param[in] _toPortIndentifier identifier of port "to"
     */
    DATA_API void setIdentifiers(const std::string& _fromPortIndentifier, const std::string& _toPortIndentifier);

    /**
     * @brief Get the edge identifier
     *
     * @return std::pair <from,to>
     */
    DATA_API std::pair<std::string, std::string> getIdentifiers() const;

    /// @brief return "from" identifier
    DATA_API std::string getFromPortID() const;

    /// @brief return "to" identifier
    DATA_API std::string getToPortID() const;

    /// @brief return "to or from" identifier according to upStream
    DATA_API std::string getPortID(bool upStream) const;

    /// @brief Set edge nature ("FLOW" , "FWDATA")
    DATA_API void setNature(std::string nature);

    /// @brief Get edge nature ("FLOW" , "FWDATA")
    DATA_API const std::string& getNature() const;

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const Edge& other) const noexcept;
    DATA_API bool operator!=(const Edge& other) const noexcept;
    /// @}

protected:

    /// @brief do a deep copy of edge
    DATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    std::string m_fromPortIdentifier; // "ID_SIZEX" , ...
    std::string m_toPortIdentifier;   // "ID_SIZEX" , ...
    std::string m_nature;             // "FLOW" , "FWDATA"
};

} // namespace sight::data
