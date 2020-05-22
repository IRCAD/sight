/************************************************************************
 *
 * Copyright (C) 2014-2019 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
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
 * @brief This service get objects from a source object and expose them as new objects.
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::ctrlCamp::SExtractObj" >
           <inout key="source" uid="...">
               <extract from="@values.myobject1" />
               <extract from="@values.myobject2" />
           </inout>
           <out group="target">
               <key uid="..."/>
               <key uid="..."/>
           </out>
        </service>
       @endcode
 * @subsection In-Out In-Out
 * - \b source [::fwData::Object]: define the source object where objects are extracted from.
 *
 * @subsection Output Output
 * - \b target [::fwData::Object]: define the target objects. The number of \b target keys must match
 * the number of \b from tags.
 *
 * @subsection Configuration Configuration
 * - \b from (mandatory) : key to extract from the source object. The number of \b from tags must match
 * the number of \b target keys.
 */
class CTRLCAMP_CLASS_API SExtractObj : public ::ctrlCamp::ICamp
{
public:

    fwCoreServiceMacro(SExtractObj, ::ctrlCamp::ICamp);

    typedef std::map<std::string, std::string> ExtractMapType;

    /// Constructor
    CTRLCAMP_API SExtractObj();

    /// Destructor
    CTRLCAMP_API ~SExtractObj();

protected:

    /// Does nothing
    CTRLCAMP_API virtual void starting() override;

    /// Configure the service
    CTRLCAMP_API virtual void configuring() override;

    /// Does nothing
    CTRLCAMP_API virtual void stopping() override;

    /// Extract the object(s)
    CTRLCAMP_API virtual void updating() override;

private:

    /// Map to associate \<from, to\> for object extraction
    ExtractMapType m_extract;

    /// Vector to associate source paths
    std::vector<std::string> m_sourcePaths;
};

} // namespace ctrlCamp
