/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLSELECTION_SSYNCHRONIZEWINDOWINGTF_HPP__
#define __CTRLSELECTION_SSYNCHRONIZEWINDOWINGTF_HPP__

#include <fwServices/IService.hpp>
#include <fwServices/IController.hpp>

#include "ctrlSelection/config.hpp"

namespace ctrlSelection
{


/**
 * @class  SSynchronizeWindowingTF.
 * @brief  This service synchronize the selected transfert functions from the image window min/max.
 * @author IRCAD (Research and Development Team).
 * @date   2011.
 *
 * A selected TF name is stored in a field. There is a default selection fields (id = ::fwComEd::Dictionary::m_transfertFunctionId).
 * Specific selection fiels are given in service configuration.
 *
 * @note This service must have a high communication channel priority, it must synchronize TF before visualisation.
 */
class CTRLSELECTION_CLASS_API SSynchronizeWindowingTF : public ::fwServices::IController
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SSynchronizeWindowingTF)(::fwServices::IController) ) ;

protected:

    /// Vector of field ids containing TF selection
    typedef std::vector< std::string > TFFieldIdContainerType;

    /// Constructor. Initialise TF fields container with default selection.
    CTRLSELECTION_API SSynchronizeWindowingTF() throw() ;

    /// Destructor. Do nothing.
    CTRLSELECTION_API virtual ~SSynchronizeWindowingTF() throw() ;

    /// Synchonize TF from min/max.
    CTRLSELECTION_API virtual void starting()  throw ( ::fwTools::Failed );

    /// Do nothing.
    CTRLSELECTION_API virtual void stopping()  throw ( ::fwTools::Failed );

    /// Synchonize TF from min/max.
    CTRLSELECTION_API virtual void swapping()  throw ( ::fwTools::Failed );

    /**
     * Configure tf selection field to synchronize from windowing min/max
     * @verbatim
       <service uid="GENERIC_UID_synchronizeTF" implementation="::ctrlSelection::SSynchronizeWindowingTF" type="::fwServices::IController" autoComChannel="yes" priority="0.99">
           <tfSelection id="greyLevelTF" />
       </service>
       @endverbatim
     * \b tfSelection id : the identifier of the field containing tf selection
     */
    CTRLSELECTION_API virtual void configuring()  throw ( ::fwTools::Failed );

    /// Do nothing.
    CTRLSELECTION_API virtual void reconfiguring()  throw ( ::fwTools::Failed );

    /// Do nothing.
    CTRLSELECTION_API virtual void updating() throw ( ::fwTools::Failed );

    /// Implements info method derived from IService. Print classname.
    CTRLSELECTION_API virtual void info( std::ostream &_sstream );

    /// Listen ::fwComEd::ImageMsg::WINDOWING event then synchonize TF from min/max.
    CTRLSELECTION_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

private :

    /// Update the selected TFs from image Windowing Min/Max
    void updateTFFromMinMax();

    /// Container of fields identifier containing tf selection
    TFFieldIdContainerType m_tfSelectionFieldIds;
};

} // ctrlSelection

#endif // __CTRLSELECTION_SSYNCHRONIZEWINDOWINGTF_HPP__
