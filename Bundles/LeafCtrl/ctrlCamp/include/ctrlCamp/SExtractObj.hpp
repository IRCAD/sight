/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMP_SEXTRACTOBJ_HPP__
#define __CTRLCAMP_SEXTRACTOBJ_HPP__

#include "ctrlCamp/config.hpp"
#include "ctrlCamp/ICamp.hpp"

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/IService.hpp>



namespace ctrlCamp
{
/**
 * @class SExtractObj
 * @brief This service get an object from one place to push it in the composite.
 */
class CTRLCAMP_CLASS_API SExtractObj : public ::ctrlCamp::ICamp
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SExtractObj)(::ctrlCamp::ICamp) );

    typedef std::map<std::string, std::string> ExtractMapType;

    /// Constructor
    CTRLCAMP_API SExtractObj();

    /// Destructor
    CTRLCAMP_API ~SExtractObj();

protected:

    enum class ModeType : std::int8_t
    {
        START,
        UPDATE,
    };

    /// Does nothing
    CTRLCAMP_API virtual void starting() throw( ::fwTools::Failed );

    /**
     * @brief Configure the service
     *
     * @code{.xml}
        <service uid="..." type="::ctrlCamp::ICamp" impl="::ctrlCamp::SExtractObj" autoConnect="no">
            <extract from="@values.myobject1" to="objKey1" />
            <extract from="@values.myobject2" to="objKey2" />
            <mode>extractOnStart</mode>
        </service>
       @endcode
     * - \b from (mandatory) : introspection path to the object to extract
     * - \b to (mandatory) : key where to push extracted object
     * - \b mode (optional) : The service can extract the data either when starting ("extractOnStart") or when
     * updating ("extractOnUpdate" - default).
     */
    CTRLCAMP_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMP_API virtual void stopping() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMP_API virtual void updating() throw( ::fwTools::Failed );

private:

    /// Extract method
    void extract();

    /// Map to associate \<from, to\> for object extraction
    ExtractMapType m_extract;

    /// Determine when the data is copied (start or update)
    ModeType m_mode;
};

} // namespace ctrlCamp


#endif /* __CTRLCAMP_SEXTRACTOBJ_HPP__ */
