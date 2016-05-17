/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMP_SEXTRACTMESHBYTYPE_HPP__
#define __CTRLCAMP_SEXTRACTMESHBYTYPE_HPP__


#include "ctrlCamp/config.hpp"
#include "ctrlCamp/ICamp.hpp"

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IService.hpp>


namespace ctrlCamp
{
/**
 * @class SExtractMeshByType
 * @brief This service get a mesh of a specific structure type from one place to push it in the composite.
 *        If the model series have some meshes with the same structure type if will treat the first one only
 */
class CTRLCAMP_CLASS_API SExtractMeshByType : public ::ctrlCamp::ICamp
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SExtractMeshByType)(::ctrlCamp::ICamp) );

    typedef std::map<std::string, std::pair< std::string, std::string > > ExtractMapType;

    /// Constructor
    CTRLCAMP_API SExtractMeshByType();

    /// Destructor
    CTRLCAMP_API ~SExtractMeshByType();

protected:

    /// Does nothing
    CTRLCAMP_API virtual void starting() throw( ::fwTools::Failed );

    /**
     * @brief Configure the service
     *
     * @code{.xml}
        <service uid="..." type="::fwServices::IController" impl="::ctrlCamp::SExtractMeshByType" autoConnect="no">
            <extract from="@values.myobject1" to="objKey1" type="organ1" />
            <extract from="@values.myobject2" to="objKey2" type="organ2" />
        </service>
       @endcode
     * - \b from (mandatory) : introspection path to the object to extract
     * - \b to (mandatory) : key where to push extracted object
     * - \b type (mandatory) : the structure type of the mesh
     * - \b matching (optional) : the name of the extrtacting organ must match the regex.
     */
    CTRLCAMP_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMP_API virtual void stopping() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMP_API virtual void updating() throw( ::fwTools::Failed );

private:

    /// Map to associate \<from, to\> for object extraction
    ExtractMapType m_extract;

    std::string m_regex;
};

}// namespace ctrlCamp


#endif /* __CTRLCAMP_SEXTRACTMESHBYTYPE_HPP__ */
