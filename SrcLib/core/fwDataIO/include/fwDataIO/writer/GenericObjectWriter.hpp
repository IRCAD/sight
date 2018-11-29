/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/IObjectWriter.hpp"

namespace fwDataIO
{

namespace writer
{

/**
 * @brief   generic class for all object writers.
 *
 * This class adds 2 methods to the API of IObjectWriter. This
 * generic reader test the object type given to the method setObject.
 * And add a new get method getConcreteObject() that returns an object
 * already cast.
 */
template<class DATATYPE>
class GenericObjectWriter : public ::fwDataIO::writer::IObjectWriter
{

public:

    /// the object type related
    typedef DATATYPE DataType;

    fwCoreNonInstanciableClassDefinitionsMacro( (GenericObjectWriter<DATATYPE>)(::fwDataIO::writer::IObjectWriter) );

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
     * @param[out] obj replace weakptr m_object of the instance reader
     *
     * This method overload setObject to ensure that given object correspond to DATATYPE
     */
    virtual void setObject( ::fwTools::Object::csptr obj) override
    {
        OSLM_ASSERT("Object type: '" << std::string(typeid(obj).name())
                                     << "' is not a '" << typeid(DataType()).name()<<"'",
                    std::dynamic_pointer_cast< const DataType >(obj));
        IObjectWriter::setObject(obj);
    }

    /**
     * @brief m_object getter.
     * @return m_object
     *
     * This method automatic cast object in correct DataType.
     */
    virtual std::shared_ptr< const DataType > getConcreteObject() const
    {
        return std::dynamic_pointer_cast< const DataType >( getObject() );
    }

};

} // namespace writer

} // namespace fwDataIO
