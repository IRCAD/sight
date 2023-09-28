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
#include "io/__/writer/factory/new.hpp"
#include "io/__/writer/registry/detail.hpp"

#include <core/base.hpp>
#include <core/jobs/base.hpp>
#include <core/tools/object.hpp>

#include <cstdint>
#include <filesystem>

namespace sight::io::writer
{

/**
 * @brief   Base class for all object writers.
 *
 * This class defines the API to use basic object writers. This writer is not
 * a service, but its equivalent service exists. See io::service::writer. To write an
 * object with this class, use setLocation and setOject method before executing
 * the method write.
 *
 * @todo Remove m_extension of this class. It must be deprecated.
 */
class IO_CLASS_API IObjectWriter : public core::base_object
{
public:

    SIGHT_DECLARE_CLASS(IObjectWriter, core::base_object);

    typedef std::function<void (std::uint64_t /*progress*/)> ProgressCallback;
    typedef std::function<void ()> CancelCallback;

    /**
     * @brief Class used to register a class factory in factory registry.
     * This class defines also the object factory ( 'create' )
     *
     * @tparam T Factory product type
     */
    template<typename T>
    class Registry
    {
    public:

        Registry()
        {
            io::writer::registry::get()->add_factory(T::classname(), &io::writer::factory::make<T>);
        }
    };

    /**
     * @brief Defines a writer interface.
     *
     * This method write the object given in parameter of setObject method.
     */
    IO_API virtual void write() = 0;

    /**
     * @brief m_object setter.
     * @param[in] _pObject replaces m_object of the instance writer
     * @note m_object is saved in class as a weakptr
     * @note This object can be get with the method getObject()
     */
    IO_API virtual void setObject(core::tools::object::csptr object);

    /**
     * @brief m_object getter.
     *
     * @return m_object
     */
    IO_API virtual core::tools::object::csptr getObject() const;

    /**
     * @brief Return the default filename extension can be dynamic.
     * @return m_extension
     *
     * The extension can be empty (for a repository for example) or must return a string BEGINNING WITH A DOT
     * by default be empty
     */
    IO_API virtual std::string extension() const = 0;

    /**
     * @brief Requests writer abortion.
     */
    IO_API virtual void cancel();

    /// Returns the internal job, nullptr by default
    virtual SPTR(core::jobs::base) getJob() const
    {
        return nullptr;
    }

protected:

    /// Constructor. Do nothing.
    IO_API IObjectWriter();

    /// Destructor. Do nothing.
    IO_API ~IObjectWriter() override;

    /**
     * @brief Object to write on filesystem by the process.
     *
     * This object is given in parameter of setObject method but it is conserved with a weakptr.
     */
    core::tools::object::cwptr m_object;

    /// Extension of file format
    std::string m_extension;
};

} // namespace sight::io::writer
