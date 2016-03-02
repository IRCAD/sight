/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMP_SCOPY_HPP__
#define __CTRLCAMP_SCOPY_HPP__

#include "ctrlCamp/config.hpp"
#include "ctrlCamp/ICamp.hpp"

#include <fwCore/base.hpp>

#include <fwServices/Base.hpp>
#include <fwServices/IService.hpp>

namespace ctrlCamp
{
/**
 * @class SCopy
 * @brief This service copies an object and updates its own object or the defined target.
 * It can either copy the data when starting or when updating (default).
 */
class CTRLCAMP_CLASS_API SCopy : public ::ctrlCamp::ICamp
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SCopy)(::ctrlCamp::ICamp) );

    /// Constructor
    CTRLCAMP_API SCopy();

    /// Destructor
    CTRLCAMP_API ~SCopy();

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
        <service uid="..." type="::ctrlCamp::ICamp" impl="::ctrlCamp::SCopy" autoConnect="no">
            <source>imageUID</source>
            <target>@values.myTarget<target>
            <mode>copyOnStart</mode>
        </service>
       @endcode
     * - \b source (mandatory) : parameter defines the source to copy. It can be defined by its uid or by a sesh@ path
     * (beginning with @ : ex. @value.myImageKey).
     * - \b target (optional) : The path of the object to update. If it is not defined, the current object is updated.
     * - \b mode (optional) : The service can copy the data either when starting ("copyOnStart") or when
     * updating ("copyOnUpdate" - default).
     */
    CTRLCAMP_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMP_API virtual void stopping() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMP_API virtual void updating() throw( ::fwTools::Failed );

private:

    /// Copy method
    void copy();

    /// source object uid or sesh@ path
    std::string m_source;

    /// target sesh@ path
    std::string m_target;

    /// Determine when the data is copied (start or update)
    ModeType m_mode;
};

} // namespace ctrlCamp


#endif /* __CTRLCAMP_SCOPY_HPP__ */
