/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOEXT_DATA_PACSCONFIGURATION_HPP__
#define __FWDICOMIOEXT_DATA_PACSCONFIGURATION_HPP__


#include <fwData/Object.hpp>
#include <fwData/factory/new.hpp>
#include <fwData/macros.hpp>

#include "fwDicomIOExt/config.hpp"

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
    fwDataGetSetCRefMacro(LocalApplicationTitle, std::string);
    /**  @} */

    /**
     * @brief Pacs host name
     * @{ */
    fwDataGetSetCRefMacro(PacsHostName, std::string);
    /**  @} */

    /**
     * @brief Pacs application title
     * @{ */
    fwDataGetSetCRefMacro(PacsApplicationTitle, std::string);
    /**  @} */

    /**
     * @brief Pacs port
     * @{ */
    fwDataGetSetMacro(PacsApplicationPort, unsigned int);
    /**  @} */

    /**
     * @brief Move application title
     * @{ */
    fwDataGetSetCRefMacro(MoveApplicationTitle, std::string);
    /**  @} */

    /**
     * @brief Move application port
     * @{ */
    fwDataGetSetMacro(MoveApplicationPort, unsigned int);
    /**  @} */

    /**
     * @brief Request mode
     * @{ */
    fwDataGetSetMacro(RetrieveMethod, RETRIEVE_METHOD);
    /**  @} */

    /**  @} */


protected:
    /// Local application title
    std::string m_attrLocalApplicationTitle;

    /// Pacs host name
    std::string m_attrPacsHostName;

    /// Pacs application title
    std::string m_attrPacsApplicationTitle;

    /// Pacs application port
    unsigned int m_attrPacsApplicationPort;

    /// Move application title
    std::string m_attrMoveApplicationTitle;

    /// Move application port
    unsigned int m_attrMoveApplicationPort;

    /// Retrieve method
    RETRIEVE_METHOD m_attrRetrieveMethod;


};

} //namespace data
} //namespace fwDicomIOExt

#endif /* __FWDICOMIOEXT_DATA_PACSCONFIGURATION_HPP__ */
