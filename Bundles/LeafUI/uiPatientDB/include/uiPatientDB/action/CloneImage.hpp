/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIPATIENTDB_ACTION_CLONEIMAGE_HPP_
#define _UIPATIENTDB_ACTION_CLONEIMAGE_HPP_

#include "uiPatientDB/config.hpp"

#include <fwData/Patient.hpp>
#include <fwGui/IActionSrv.hpp>


namespace uiPatientDB
{
namespace action
{

/**
 * @brief   This action allows to clone an Image in a PatientDB
 * @class   CloneImage.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIPATIENTDB_CLASS_API CloneImage : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (CloneImage)( ::fwGui::IActionSrv) ) ;

    UIPATIENTDB_API CloneImage() throw() ;

    UIPATIENTDB_API virtual ~CloneImage() throw() ;

protected:

    /// Configure action
    void configuring() throw( ::fwTools::Failed ) ;

    /// Start action
    void starting() throw (::fwTools::Failed);

    /// Clone the selected Image and add it in the PatientDB
    void updating() throw (::fwTools::Failed);

    /// Does nothing
    void updating( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed) ;

    /// Stop action.
    void stopping() throw (::fwTools::Failed);
    void info(std::ostream &_sstream ) ;


};

} // namespace action
} // namespace uiPatientDB

#endif // _UIPATIENTDB_ACTION_CLONEIMAGE_HPP_
