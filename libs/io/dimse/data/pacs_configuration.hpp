/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "io/dimse/config.hpp"

#include <data/factory/new.hpp>
#include <data/object.hpp>

namespace sight::io::dimse::data
{

/**
 * @brief   This class defines a Pacs configuration.
 */
class IO_DIMSE_CLASS_API pacs_configuration : public sight::data::object
{
public:

    SIGHT_DECLARE_CLASS(pacs_configuration, sight::data::object, sight::data::factory::make<pacs_configuration>);

    /// Destructor
    IO_DIMSE_API ~pacs_configuration() noexcept override = default;

    /**
     * @brief Retrieve Method
     */
    enum class retrieve_method
    {
        move = 1,
        get  = 2
    };

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Local application title
     * @{ */
    const std::string& get_local_application_title() const
    {
        return m_local_application_title;
    }

    //------------------------------------------------------------------------------

    void set_local_application_title(const std::string& _local_application_title)
    {
        m_local_application_title = _local_application_title;
    }

    /**  @} */

    /**
     * @brief Pacs host name
     * @{ */
    const std::string& get_pacs_host_name() const
    {
        return m_pacs_host_name;
    }

    //------------------------------------------------------------------------------

    void set_pacs_host_name(const std::string& _pacs_host_name)
    {
        m_pacs_host_name = _pacs_host_name;
    }

    /**  @} */

    /**
     * @brief Pacs application title
     * @{ */
    const std::string& get_pacs_application_title() const
    {
        return m_pacs_application_title;
    }

    //------------------------------------------------------------------------------

    void set_pacs_application_title(const std::string& _pacs_application_title)
    {
        m_pacs_application_title = _pacs_application_title;
    }

    /**  @} */

    /**
     * @brief Pacs port
     * @{ */
    const std::uint16_t& get_pacs_application_port() const
    {
        return m_pacs_application_port;
    }

    //------------------------------------------------------------------------------

    void set_pacs_application_port(const std::uint16_t& _pacs_application_port)
    {
        m_pacs_application_port = _pacs_application_port;
    }

    /**  @} */

    /**
     * @brief Move application title
     * @{ */
    const std::string& get_move_application_title() const
    {
        return m_move_application_title;
    }

    //------------------------------------------------------------------------------

    void set_move_application_title(const std::string& _move_application_title)
    {
        m_move_application_title = _move_application_title;
    }

    /**  @} */

    /**
     * @brief Move application port
     * @{ */
    std::uint16_t get_move_application_port() const
    {
        return m_move_application_port;
    }

    //------------------------------------------------------------------------------

    void set_move_application_port(std::uint16_t _move_application_port)
    {
        m_move_application_port = _move_application_port;
    }

    /**  @} */

    /**
     * @brief Request mode
     * @{ */
    retrieve_method get_retrieve_method() const
    {
        return m_retrieve_method;
    }

    //------------------------------------------------------------------------------

    void set_retrieve_method(retrieve_method _retrieve_method)
    {
        m_retrieve_method = _retrieve_method;
    }

/**  @} */

/**  @} */

protected:

    /// Defines shallow copy
    /// @throws data::exception if an errors occurs during copy
    /// @param[in] _source the source object to copy
    IO_DIMSE_API void shallow_copy(const sight::data::object::csptr& _source) override;

    /// Local application title
    std::string m_local_application_title;

    /// Pacs host name
    std::string m_pacs_host_name;

    /// Pacs application title
    std::string m_pacs_application_title;

    /// Pacs application port
    std::uint16_t m_pacs_application_port {0};

    /// Move application title
    std::string m_move_application_title;

    /// Move application port
    std::uint16_t m_move_application_port {0};

    /// Retrieve method
    retrieve_method m_retrieve_method {retrieve_method::move};
};

} // namespace sight::io::dimse::data
