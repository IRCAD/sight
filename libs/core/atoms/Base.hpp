/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "atoms/config.hpp"
#include "atoms/factory/new.hpp"
#include "atoms/registry/detail.hpp"

#include <core/base.hpp>

#include <string>

namespace sight::atoms
{

/**
 * @brief Base class for all Atom classes.
 */
class ATOMS_CLASS_API Base : public core::BaseObject
{
public:
    typedef atoms::factory::Key Key;

    /// Defines fwAtoms version.
    ATOMS_API static const std::string s_VERSION;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template <typename T>
    class Registrar
    {
    public:
        Registrar()
        {
            atoms::registry::get()->addFactory(T::classname(), &atoms::factory::New<T>);
        }
    };

    typedef enum
    {
        BOOLEAN = 1,
        NUMERIC,
        STRING,
        OBJECT,
        SEQUENCE,
        MAP,
        BLOB
    } AtomType;

    fwCoreClassMacro(Base, core::BaseObject)
    fwCoreAllowSharedFromThis();

    virtual ~Base()
    {
    }

    /**
     * @brief Returns a clone object
     */
    virtual Base::sptr clone() const = 0;

    /**
     *@brief Test if the current base is a value or not (Value = {string;numeric;boolean})
     *@return the test result
     */
    bool isValue() const
    {
        return (type() == BOOLEAN) || (type() == NUMERIC) || (type() == STRING);
    }

    /**
     *@brief Test if the current base is a boolean
     *@return the test result
     */
    bool isBoolean() const
    {
        return (type() == BOOLEAN);
    }

    /**
     *@brief Test if the current base is a Numeric (real or integer)
     *@return the test result
     */
    bool isNumeric() const
    {
        return (type() == NUMERIC);
    }

    /**
     *@brief Test if the current base is a string
     *@return the test result
     */
    bool isString() const
    {
        return (type() == STRING);
    }

    /**
     *@brief Test if the current base is a sequence
     *@return the test result
     */
    bool isSequence() const
    {
        return (type() == SEQUENCE);
    }

    /**
     *@brief Test if the current base is a mapping
     *@return the test result
     */
    bool isMapping() const
    {
        return (type() == MAP);
    }

    /**
     *@brief Test if the current base is a blob (a object with a buffer)
     *@return the test result
     */
    bool isBlob() const
    {
        return (type() == BLOB);
    }

    /**
     *@brief Test if the current base is an object (representation of fwData)
     *@return the test result
     */
    bool isObject() const
    {
        return (type() == OBJECT);
    }

    /**
     * @brief Return the string representation of a value.
     * @return the value's representation, else "<Unknown>"
     */
    ATOMS_API virtual std::string getString() const;

    /**
     * @brief returns Atom type
     */
    virtual AtomType type() const = 0;

protected:
    /**
     * @brief Constructor
     */
    Base()
    {
    }

};

}
