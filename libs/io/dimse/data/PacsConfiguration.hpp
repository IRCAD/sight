/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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
#include <data/Object.hpp>

namespace sight::io::dimse::data
{

/**
 * @brief   This class defines a Pacs configuration.
 */
class IO_DIMSE_CLASS_API PacsConfiguration : public sight::data::Object
{
public:

    SIGHT_DECLARE_CLASS(PacsConfiguration, sight::data::Object, sight::data::factory::New<PacsConfiguration>);

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    IO_DIMSE_API PacsConfiguration(sight::data::Object::Key key);

    /// Destructor
    IO_DIMSE_API ~PacsConfiguration() noexcept override = default;

    /**
     * @brief Retrieve Method
     */
    typedef enum
    {
        MOVE_RETRIEVE_METHOD = 1,
        GET_RETRIEVE_METHOD  = 2
    } RETRIEVE_METHOD;

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Local application title
     * @{ */
    const std::string& getLocalApplicationTitle() const
    {
        return m_localApplicationTitle;
    }

    //------------------------------------------------------------------------------

    void setLocalApplicationTitle(const std::string& localApplicationTitle)
    {
        m_localApplicationTitle = localApplicationTitle;
    }

    /**  @} */

    /**
     * @brief Pacs host name
     * @{ */
    const std::string& getPacsHostName() const
    {
        return m_pacsHostName;
    }

    //------------------------------------------------------------------------------

    void setPacsHostName(const std::string& pacsHostName)
    {
        m_pacsHostName = pacsHostName;
    }

    /**  @} */

    /**
     * @brief Pacs application title
     * @{ */
    const std::string& getPacsApplicationTitle() const
    {
        return m_pacsApplicationTitle;
    }

    //------------------------------------------------------------------------------

    void setPacsApplicationTitle(const std::string& pacsApplicationTitle)
    {
        m_pacsApplicationTitle = pacsApplicationTitle;
    }

    /**  @} */

    /**
     * @brief Pacs port
     * @{ */
    const std::uint16_t& getPacsApplicationPort() const
    {
        return m_pacsApplicationPort;
    }

    //------------------------------------------------------------------------------

    void setPacsApplicationPort(const std::uint16_t& pacsApplicationPort)
    {
        m_pacsApplicationPort = pacsApplicationPort;
    }

    /**  @} */

    /**
     * @brief Move application title
     * @{ */
    const std::string& getMoveApplicationTitle() const
    {
        return m_moveApplicationTitle;
    }

    //------------------------------------------------------------------------------

    void setMoveApplicationTitle(const std::string& moveApplicationTitle)
    {
        m_moveApplicationTitle = moveApplicationTitle;
    }

    /**  @} */

    /**
     * @brief Move application port
     * @{ */
    std::uint16_t getMoveApplicationPort() const
    {
        return m_moveApplicationPort;
    }

    //------------------------------------------------------------------------------

    void setMoveApplicationPort(std::uint16_t moveApplicationPort)
    {
        m_moveApplicationPort = moveApplicationPort;
    }

    /**  @} */

    /**
     * @brief Request mode
     * @{ */
    RETRIEVE_METHOD getRetrieveMethod() const
    {
        return m_retrieveMethod;
    }

    //------------------------------------------------------------------------------

    void setRetrieveMethod(RETRIEVE_METHOD retrieveMethod)
    {
        m_retrieveMethod = retrieveMethod;
    }

/**  @} */

/**  @} */

protected:

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    IO_DIMSE_API void shallowCopy(const sight::data::Object::csptr& source) override;

    /// Local application title
    std::string m_localApplicationTitle;

    /// Pacs host name
    std::string m_pacsHostName;

    /// Pacs application title
    std::string m_pacsApplicationTitle;

    /// Pacs application port
    std::uint16_t m_pacsApplicationPort {0};

    /// Move application title
    std::string m_moveApplicationTitle;

    /// Move application port
    std::uint16_t m_moveApplicationPort {0};

    /// Retrieve method
    RETRIEVE_METHOD m_retrieveMethod {MOVE_RETRIEVE_METHOD};
};

} // namespace sight::io::dimse::data
