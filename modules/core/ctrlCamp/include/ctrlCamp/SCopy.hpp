/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <core/base.hpp>

#include <services/IService.hpp>
#include <services/macros.hpp>

namespace ctrlCamp
{

/**
 * @brief This service copies an object and updates its own object or the defined target.
 * It can either copy the data when starting or when updating (default).
 *
 * @section XML XML Configuration
 * @code{.xml}
       <service uid="..." type="::ctrlCamp::SCopy" >
           <in key="source" uid="..." >
             <extract from="@path.to.data.0" />
           </in>
           <inout key="target" uid="..." />
           <mode>copyOnStart</mode>
       </service>

       <service uid="..." type="::ctrlCamp::SCopy" >
           <in key="source" uid="..." />
           <inout key="target" uid="..." />
           <mode>copyOnStart</mode>
       </service>

       <service uid="..." type="::ctrlCamp::SCopy" >
           <in key="source" uid="..." >
             <extract from="@path.to.data.0" />
           </in>
           <out key="target" uid="..." />
           <mode>copyOnStart</mode>
       </service>

       <service uid="..." type="::ctrlCamp::SCopy" >
           <in key="source" uid="..." />
           <out key="target" uid="..." />
           <mode>copyOnStart</mode>
       </service>
   @endcode
 *
 * @subsection Input Input
 * - \b source [data::Object]: define the source object to copy.
 *    - \b extract (optional): define the camp path used to retrieve the object to copy.
 *
 * @subsection In-Out In-Out
 * - \b target [data::Object]: define the target object to update, can't be used with Output.
 *
 * @subsection Output Output
 * - \b target [data::Object]: define the target object to create, can't be used with In-Out.
 *
 * @subsection Configuration Configuration
 * - \b mode (optional) : The service can copy the data either when starting ("copyOnStart") or when
 * updating ("copyOnUpdate" - default).
 */
class CTRLCAMP_CLASS_API SCopy final : public ::ctrlCamp::ICamp
{

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SCopy, ::ctrlCamp::ICamp)

    /// Creates the service.
    CTRLCAMP_API SCopy();

    /// Destroys the service.
    CTRLCAMP_API ~SCopy();

protected:

    //// Configures the service
    CTRLCAMP_API void configuring() override;

    /// Calls copy() if the mode if START.
    CTRLCAMP_API void starting() override;

    /// Calls copy() if the mode if UPDATE.
    CTRLCAMP_API void updating() override;

    /// Sets the output to null.
    CTRLCAMP_API void stopping() override;

private:

    enum class ModeType : std::int8_t
    {
        START,
        UPDATE,
    };

    /// Copies the object to the output.
    void copy();

    /// Defines the sesh@ path for appXml
    std::string m_path;

    /// Defines if the object to copy is a sesh@ path or an object
    bool m_hasExtractTag;

    /// Determines when the data is copied (start or update)
    ModeType m_mode;

};

} // namespace ctrlCamp.
