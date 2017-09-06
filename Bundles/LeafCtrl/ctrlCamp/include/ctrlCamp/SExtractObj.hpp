/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLCAMP_SEXTRACTOBJ_HPP__
#define __CTRLCAMP_SEXTRACTOBJ_HPP__

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

    fwCoreServiceClassDefinitionsMacro( (SExtractObj)(::ctrlCamp::ICamp) );

    typedef std::map<std::string, std::string> ExtractMapType;

    /// Constructor
    CTRLCAMP_API SExtractObj();

    /// Destructor
    CTRLCAMP_API ~SExtractObj();

protected:

    /// Does nothing
    CTRLCAMP_API virtual void starting();

    /// Configure the service
    CTRLCAMP_API virtual void configuring();

    /// Does nothing
    CTRLCAMP_API virtual void stopping();

    /// Extract the object(s)
    CTRLCAMP_API virtual void updating();

private:

    /// Map to associate \<from, to\> for object extraction
    ExtractMapType m_extract;

    /// Vector to associate source paths
    std::vector<std::string> m_sourcePaths;
};

} // namespace ctrlCamp

#endif /* __CTRLCAMP_SEXTRACTOBJ_HPP__ */
