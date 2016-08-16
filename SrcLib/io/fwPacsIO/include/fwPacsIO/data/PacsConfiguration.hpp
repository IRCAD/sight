/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWPACSIO_DATA_PACSCONFIGURATION_HPP__
#define __FWPACSIO_DATA_PACSCONFIGURATION_HPP__

#include "fwPacsIO/config.hpp"

#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>

fwCampAutoDeclareDataMacro((fwPacsIO)(data)(PacsConfiguration), FWPACSIO_API);

namespace fwPacsIO
{
namespace data
{
/**
 * @brief   This class defines a Pacs configuration.
 */
class FWPACSIO_CLASS_API PacsConfiguration : public ::fwData::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (PacsConfiguration)(::fwData::Object), (()),
                                            ::fwData::factory::New< PacsConfiguration >);
    fwCampMakeFriendDataMacro((fwPacsIO)(data)(PacsConfiguration));


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWPACSIO_API PacsConfiguration(::fwData::Object::Key key);

    /// Destructor
    FWPACSIO_API virtual ~PacsConfiguration();

    /// Defines shallow copy
    FWPACSIO_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWPACSIO_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache);

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
} //namespace fwPacsIO

#endif /* __FWPACSIO_DATA_PACSCONFIGURATION_HPP__ */
