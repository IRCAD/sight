/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
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
 * @brief This service extract meshes of a specific structure type from ::fwMedData::ModelSeries.
 * If the model series have some meshes with the same structure type, it will extract the first found.
 * Additionnaly, it can also match the organ name with a regular expression.
 *
 * The output objects must be marked as "deferred" in the XML configuration.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service uid="..." type="::ctrlCamp::SExtractMeshByType" >
           <inout key="source" uid="...">
               <extract type="organ1" />
               <extract type="organ2" matching="(.*)surface(.*)"/>
           </inout>
           <out group="target">
               <key uid="..."/>
               <key uid="..."/>
           </out>
        </service>
       @endcode

 * @subsection In-Out In-Out
 * - \b source [::fwMedData::ModelSeries]: define the source models where meshes are extracted from.
 *
 * @subsection Output Output
 * - \b target [::fwData::Mesh]: define the target meshes. The number of \b target keys must match
 * the number of \b from tags.
 *
 * @subsection Configuration Configuration
 * - \b type (mandatory) : structure type of the mesh to extract from the source model. The number of \b from tags must
 * match the number of \b target keys.
 * - \b matching (optional) : regex to match the organ name, that can be used for instance when you have several
 * organs with the same structure type.
 */
class CTRLCAMP_CLASS_API SExtractMeshByType : public ::ctrlCamp::ICamp
{
public:

    fwCoreServiceClassDefinitionsMacro ( (SExtractMeshByType)(::ctrlCamp::ICamp) );

    typedef std::map<std::string, std::tuple< std::string, std::string, std::string > > ExtractMapType;
    typedef std::vector<std::pair< std::string, std::string > > ExtractVectorType;

    /// Constructor
    CTRLCAMP_API SExtractMeshByType();

    /// Destructor
    CTRLCAMP_API ~SExtractMeshByType();

protected:

    /// Does nothing
    CTRLCAMP_API virtual void starting() throw( ::fwTools::Failed );

    /// Configure the service
    CTRLCAMP_API virtual void configuring() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMP_API virtual void stopping() throw( ::fwTools::Failed );

    /// Does nothing
    CTRLCAMP_API virtual void updating() throw( ::fwTools::Failed );

private:

    /// Map to associate \<from, to\> for object extraction
    /// @deprecated
    ExtractMapType m_extractOld;

    /// Vector to associate \<from, to\> for object extraction
    ExtractVectorType m_extract;
};

}// namespace ctrlCamp


#endif /* __CTRLCAMP_SEXTRACTMESHBYTYPE_HPP__ */
