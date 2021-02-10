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

#include "io/dimse/config.hpp"

#include <data/factory/new.hpp>
#include <data/Object.hpp>

fwCampAutoDeclareDataMacro((sight)(io)(dimse)(data)(PacsConfiguration), IO_DIMSE_API);

namespace sight::io::dimse
{
namespace data
{
/**
 * @brief   This class defines a Pacs configuration.
 */
class IO_DIMSE_CLASS_API PacsConfiguration : public sight::data::Object
{

public:

    fwCoreClassMacro(PacsConfiguration, sight::data::Object, sight::data::factory::New< PacsConfiguration >)
    fwCampMakeFriendDataMacro((sight)(io)(dimse)(data)(PacsConfiguration));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    IO_DIMSE_API PacsConfiguration(sight::data::Object::Key key);

    /// Destructor
    IO_DIMSE_API virtual ~PacsConfiguration();

    /// Defines shallow copy
    IO_DIMSE_API void shallowCopy( const sight::data::Object::csptr& _source ) override;

    /// Defines deep copy
    IO_DIMSE_API void cachedDeepCopy(const sight::data::Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Retrieve Method
     */
    typedef enum
    {
        MOVE_RETRIEVE_METHOD = 1,
        GET_RETRIEVE_METHOD  = 2,
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
    const unsigned short& getPacsApplicationPort() const
    {
        return m_pacsApplicationPort;
    }

    //------------------------------------------------------------------------------

    void setPacsApplicationPort(const unsigned short& pacsApplicationPort)
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
    unsigned short getMoveApplicationPort() const
    {
        return m_moveApplicationPort;
    }

    //------------------------------------------------------------------------------

    void setMoveApplicationPort(unsigned short moveApplicationPort)
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
    /// Local application title
    std::string m_localApplicationTitle;

    /// Pacs host name
    std::string m_pacsHostName;

    /// Pacs application title
    std::string m_pacsApplicationTitle;

    /// Pacs application port
    unsigned short m_pacsApplicationPort;

    /// Move application title
    std::string m_moveApplicationTitle;

    /// Move application port
    unsigned short m_moveApplicationPort;

    /// Retrieve method
    RETRIEVE_METHOD m_retrieveMethod;

};

} //namespace data
} //namespace sight::io::dimse
