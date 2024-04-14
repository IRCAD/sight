/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <sight/io/__/config.hpp>

#include "io/__/writer/object_writer.hpp"

namespace sight::io::writer
{

/**
 * @brief   generic class for all object writers.
 *
 * This class adds 2 methods to the API of object_writer.
 * 'set_object' Replaces m_object with the argument casted into data_t.
 * 'getConcretObject' Returns m_object casted into data_t.
 */
template<class DATATYPE>
class generic_object_writer : public io::writer::object_writer
{
public:

    /// the object type related
    using data_t = DATATYPE;

    SIGHT_DECLARE_CLASS(generic_object_writer<DATATYPE>, io::writer::object_writer);

    /// Constructor. Do nothing.
    generic_object_writer()
    = default;

    /// Destructor. Do nothing.
    ~generic_object_writer() override
    = default;

    /**
     * @brief m_object setter.
     * @param[out] _obj replaces weakptr m_object of the instance reader
     *
     * This method overloads set_object to ensure that given object corresponds to DATATYPE
     */
    void set_object(core::tools::object::csptr _obj) override
    {
        SIGHT_ASSERT(
            "Object type: '" << std::string(typeid(_obj).name())
            << "' is not a '" << typeid(data_t()).name() << "'",
            std::dynamic_pointer_cast<const data_t>(_obj)
        );
        object_writer::set_object(_obj);
    }

    /**
     * @brief m_object getter.
     * @return m_object
     *
     * This method automatically casts object in correct data_t.
     */
    virtual std::shared_ptr<const data_t> get_concrete_object() const
    {
        return std::dynamic_pointer_cast<const data_t>(get_object());
    }
};

} // namespace sight::io::writer
