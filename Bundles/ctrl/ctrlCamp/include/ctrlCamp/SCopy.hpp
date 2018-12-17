/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "ctrlCamp/config.hpp"
#include "ctrlCamp/ICamp.hpp"

#include <fwCore/base.hpp>

#include <fwServices/IService.hpp>
#include <fwServices/macros.hpp>

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

    fwCoreServiceClassDefinitionsMacro( (SCopy)(::ctrlCamp::ICamp) );

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
    CTRLCAMP_API virtual void starting() override;

    //// Configure the service
    CTRLCAMP_API virtual void configuring() override;

    /// Does nothing
    CTRLCAMP_API virtual void stopping() override;

    /// Does nothing
    CTRLCAMP_API virtual void updating() override;

private:

    /// Copy method
    void copy();

    /// sesh@ path for appXml
    std::string m_path;

    /// boolean to know if the object to copy is a sesh@ path or an object
    bool m_hasExtractTag;

    /// Determine when the data is copied (start or update)
    ModeType m_mode;
};

} // namespace ctrlCamp
