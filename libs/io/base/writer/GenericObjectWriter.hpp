/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/base/config.hpp"
#include "io/base/writer/IObjectWriter.hpp"

namespace sight::io::base
{

namespace writer
{

/**
 * @brief   generic class for all object writers.
 *
 * This class adds 2 methods to the API of IObjectWriter.
 * 'setObject' Replaces m_object with the argument casted into DataType.
 * 'getConcretObject' Returns m_object casted into DataType.
 */
template<class DATATYPE>
class GenericObjectWriter : public io::base::writer::IObjectWriter
{
public:

    /// the object type related
    typedef DATATYPE DataType;

    SIGHT_DECLARE_CLASS(GenericObjectWriter<DATATYPE>, io::base::writer::IObjectWriter);

    /// Constructor. Do nothing.
    GenericObjectWriter()
    {
    }

    /// Destructor. Do nothing.
    virtual ~GenericObjectWriter()
    {
    }

    /**
     * @brief m_object setter.
     * @param[out] obj replaces weakptr m_object of the instance reader
     *
     * This method overloads setObject to ensure that given object corresponds to DATATYPE
     */
    void setObject(core::tools::Object::csptr obj) override
    {
        SIGHT_ASSERT(
            "Object type: '" << std::string(typeid(obj).name())
            << "' is not a '" << typeid(DataType()).name() << "'",
            std::dynamic_pointer_cast<const DataType>(obj)
        );
        IObjectWriter::setObject(obj);
    }

    /**
     * @brief m_object getter.
     * @return m_object
     *
     * This method automatically casts object in correct DataType.
     */
    virtual std::shared_ptr<const DataType> getConcreteObject() const
    {
        return std::dynamic_pointer_cast<const DataType>(getObject());
    }
};

} // namespace writer

} // namespace sight::io::base
