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

#include <fwServices/macros.hpp>
#include <fwServices/IService.hpp>

namespace ctrlCamp
{

/**
 * @brief This service copies an object and updates its own object or the defined target.
 * It can either copy the data when starting or when updating (default).
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="::ctrlCamp::SCopy" >
           <in key="source" uid="sourceObject">
             <extract from="@path.to.data.0" />
           </in>
           <inout key="target" uid="targetObject" />
           <mode>copyOnStart</mode>
       </service>
      @endcode
 * @subsection Input Input
 * - \b source [::fwData::Object]: define the source object to copy.
 *    - \b extract (optional): define the camp path used to retrieve the object to copy.
 *
 * @subsection In-Out In-Out
 * - \b target [::fwData::Object]: define the target object to update.
 *
 * @subsection Configuration Configuration
 * - \b mode (optional) : The service can copy the data either when starting ("copyOnStart") or when
 * updating ("copyOnUpdate" - default).
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
    CTRLCAMP_API virtual void starting();

    //// Configure the service
    CTRLCAMP_API virtual void configuring();

    /// Does nothing
    CTRLCAMP_API virtual void stopping();

    /// Does nothing
    CTRLCAMP_API virtual void updating();

private:

    /// Copy method
    void copy();

    /// source object uid or sesh@ path
    std::string m_source;

    /// sesh@ path for appXml2
    std::string m_path;

    /// boolean to know if the object to copy is a sesh@ path or an object
    bool m_hasExtractTag;

    /// target sesh@ path
    std::string m_target;

    /// Determine when the data is copied (start or update)
    ModeType m_mode;
};

} // namespace ctrlCamp


#endif /* __CTRLCAMP_SCOPY_HPP__ */
