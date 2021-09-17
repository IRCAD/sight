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

#include "io/base/config.hpp"
#include "io/base/reader/factory/new.hpp"
#include "io/base/reader/registry/detail.hpp"

#include <core/base.hpp>
#include <core/jobs/IJob.hpp>
#include <core/tools/Object.hpp>

#include <cstdint>
#include <filesystem>
#include <functional>

namespace sight::io::base
{

namespace reader
{

/**
 * @brief   Base class for all object readers.
 *
 * This class defines the API to use basic object readers. This reader is not a
 * service. Their equivalent exist as services. See io::base::service::IReader. To read an
 * object with this class, use setLocation and setOject method before executing
 * the method read. Reading modifies the object given in parameter with the
 * method setObject.
 */
class IO_BASE_CLASS_API IObjectReader : public core::BaseObject
{
public:

    SIGHT_DECLARE_CLASS(IObjectReader);

    typedef std::function<void (std::uint64_t /*progress*/)> ProgressCallback;
    typedef std::function<void ()> CancelCallback;

    typedef io::base::reader::factory::Key Key;

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
            io::base::reader::registry::get()->addFactory(T::classname(), &io::base::reader::factory::New<T>);
        }
    };

    /**
     * @brief Defines an reader interface.
     *
     * This method (re)inits the object given in parameter of setObject method.
     */
    IO_BASE_API virtual void read() = 0;

    /**
     * @brief m_object setter.
     * @param[out] _pObject replaces weakptr m_object of the instance reader
     * @note m_object is saved in class with a weakptr
     * @note This object can be get with the method getObject()
     */
    IO_BASE_API virtual void setObject(core::tools::Object::sptr _pObject);

    /**
     * @brief m_object getter.
     *
     * @return m_object
     * @note m_object is saved in reader with a weakptr
     */
    IO_BASE_API virtual core::tools::Object::sptr getObject() const;

    IO_BASE_API virtual std::string extension() const = 0;

    /**
     * @brief Requests reader abortion.
     */
    IO_BASE_API void cancel();

    /// Returns the internal job
    IO_BASE_API virtual SPTR(core::jobs::IJob) getJob() const
    {
        return nullptr;
    }

protected:

    /// Constructor. Does nothing.
    IO_BASE_API IObjectReader();

    /// Destructor. Does nothing.
    IO_BASE_API virtual ~IObjectReader();

    /**
     * @brief Object result of reading process.
     *
     * This object is given in parameter of setObject method but it is conserved with a weakptr.
     */
    core::tools::Object::wptr m_object;
};

} // namespace reader

} // namespace sight::io::base
