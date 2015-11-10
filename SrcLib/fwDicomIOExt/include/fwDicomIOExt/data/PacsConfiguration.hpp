/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DATA_PACSCONFIGURATION_HPP__
#define __FWDICOMIOEXT_DATA_PACSCONFIGURATION_HPP__

#include "fwDicomIOExt/config.hpp"

#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>

fwCampAutoDeclareDataMacro((fwDicomIOExt)(data)(PacsConfiguration), FWDICOMIOEXT_API);

namespace fwDicomIOExt
{
namespace data
{
/**
 * @class   PacsConfiguration
 * @brief   This class defines a Pacs configuration.
 * @date    2013.
 */

class FWDICOMIOEXT_CLASS_API PacsConfiguration : public ::fwData::Object
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (PacsConfiguration)(::fwData::Object), (()),
                                            ::fwData::factory::New< PacsConfiguration >);
    fwCampMakeFriendDataMacro((fwDicomIOExt)(data)(PacsConfiguration));


    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDICOMIOEXT_API PacsConfiguration(::fwData::Object::Key key);

    /// Destructor
    FWDICOMIOEXT_API virtual ~PacsConfiguration();

    /// Defines shallow copy
    FWDICOMIOEXT_API void shallowCopy( const Object::csptr& _source );

    /// Defines deep copy
    FWDICOMIOEXT_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

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
    const std::string &getLocalApplicationTitle () const
    {
        return m_localApplicationTitle;
    }
    void setLocalApplicationTitle (const std::string &val)
    {
        m_localApplicationTitle = val;
    }
    /**  @} */

    /**
     * @brief Pacs host name
     * @{ */
    const std::string &getPacsHostName () const
    {
        return m_pacsHostName;
    }
    void setPacsHostName (const std::string &val)
    {
        m_pacsHostName = val;
    }
    /**  @} */

    /**
     * @brief Pacs application title
     * @{ */
    const std::string &getPacsApplicationTitle () const
    {
        return m_pacsApplicationTitle;
    }
    void setPacsApplicationTitle (const std::string &val)
    {
        m_pacsApplicationTitle = val;
    }
    /**  @} */

    /**
     * @brief Pacs port
     * @{ */
    unsigned int  getPacsApplicationPort () const
    {
        return m_pacsApplicationPort;
    }
    void setPacsApplicationPort (unsigned int val)
    {
        m_pacsApplicationPort = val;
    }
    /**  @} */

    /**
     * @brief Move application title
     * @{ */
    const std::string &getMoveApplicationTitle() const
    {
        return m_moveApplicationTitle;
    }
    void setMoveApplicationTitle(const std::string &val)
    {
        m_moveApplicationTitle = val;
    }
    /**  @} */

    /**
     * @brief Move application port
     * @{ */
    unsigned int  getMoveApplicationPort() const
    {
        return m_moveApplicationPort;
    }
    void setMoveApplicationPort(unsigned int val)
    {
        m_moveApplicationPort = val;
    }
    /**  @} */

    /**
     * @brief Request mode
     * @{ */
    RETRIEVE_METHOD  getRetrieveMethod () const
    {
        return m_retrieveMethod;
    }
    void setRetrieveMethod (RETRIEVE_METHOD val)
    {
        m_retrieveMethod = val;
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
    unsigned int m_pacsApplicationPort;

    /// Move application title
    std::string m_moveApplicationTitle;

    /// Move application port
    unsigned int m_moveApplicationPort;

    /// Retrieve method
    RETRIEVE_METHOD m_retrieveMethod;
};

} //namespace data
} //namespace fwDicomIOExt

#endif /* __FWDICOMIOEXT_DATA_PACSCONFIGURATION_HPP__ */
