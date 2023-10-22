/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "io/__/config.hpp"
#include "io/__/reader/object_reader.hpp"

namespace sight::io::reader
{

/**
 * @brief   generic class for all object readers.
 *
 *
 * This class adds 2 methods to the API of object_reader.
 * 'set_object' Replaces m_object with the argument casted into data_t.
 * 'getConcretObject' Returns m_object casted into data_t.
 */
template<class DATATYPE>
class generic_object_reader : public io::reader::object_reader
{
public:

    /// the object type related
    typedef DATATYPE data_t;

    SIGHT_DECLARE_CLASS(generic_object_reader<DATATYPE>, io::reader::object_reader);

    /// Constructor. Does nothing.
    generic_object_reader() = default;

    //------------------------------------------------------------------------------

    std::string extension() const override
    {
        return "";
    }

    /// Destructor. Does nothing.
    ~generic_object_reader() override
    = default;

    /**
     * @brief m_object setter.
     * @param[out] obj replaces weakptr m_object of the instance reader
     *
     * This method overloads set_object to ensure that given object corresponds to DATATYPE
     */
    void set_object(core::tools::object::sptr _obj) override
    {
        assert(std::dynamic_pointer_cast<data_t>(_obj));
        m_object = _obj;
    }

    /**
     * @brief m_object getter.
     * @return m_object
     *
     * This method automatically casts object in the correct data_t.
     */
    [[nodiscard]] std::shared_ptr<data_t> getConcreteObject() const
    {
        return std::dynamic_pointer_cast<data_t>(getObject());
    }
};

} // namespace sight::io::reader
