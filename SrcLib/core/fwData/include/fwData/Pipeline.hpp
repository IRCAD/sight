/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_PIPELINE_HPP_
#define _FWDATA_PIPELINE_HPP_

#include <vector>
#include <list>
#include <map>
#include <utility>

#include <boost/cstdint.hpp>

#include "fwData/config.hpp"
#include "fwData/ProcessObject.hpp"

namespace fwData
{
/**
 * @class   Pipeline
 * @brief   This class defines a pipeline
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Pipeline : public ProcessObject
{

public :
    fwCoreClassDefinitionsWithFactoryMacro( (Pipeline)(::fwData::Object), (()), ::fwTools::Factory::New< Pipeline >) ;

    typedef enum
        {
            STRUCTURE = 0,
            ZONEA = 1,
        } PIPELINE_PURPOSE;

    typedef std::pair< ::fwData::ProcessObject const *, int >   PortIn;

    typedef std::pair< ::fwData::ProcessObject const *, int >   PortOut;

    typedef std::pair<  PortIn, PortOut >                       Link;

    typedef std::list< Link  >                                  typedefLinkContainer;

protected :

    /// Constructor
    FWDATA_API Pipeline();

    /// Destructor
    FWDATA_API virtual ~Pipeline();

    std::vector< ::fwData::ProcessObject::sptr > m_vectorProcessObject;

    typedefLinkContainer m_links;

    std::string m_sStructureType;

    bool m_bIsUsingROI;

    bool m_bIsOutputImageValidatedAsOrgan;

    //! The purpose of the associated ROI
    PIPELINE_PURPOSE m_PipelinePurpose;


};

} // namespace fwData

#endif // _FWDATA_PIPELINE_HPP_
