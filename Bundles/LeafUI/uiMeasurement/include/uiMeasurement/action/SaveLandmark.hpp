/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_SAVELANDMARK_HPP__
#define __UIMEASUREMENT_ACTION_SAVELANDMARK_HPP__

#include <fwGui/IActionSrv.hpp>

#include "uiMeasurement/config.hpp"

namespace uiMeasurement
{
namespace action
{

/**
 * @brief Save landmark store in image on the filesystem
 */
class UIMEASUREMENT_CLASS_API SaveLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro ( (SaveLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API SaveLandmark() throw();

    UIMEASUREMENT_API virtual ~SaveLandmark() throw();

protected:

    void configuring() throw (::fwTools::Failed);

    void starting() throw (::fwTools::Failed);

    void updating() throw (::fwTools::Failed);

    void receiving( ::fwServices::ObjectMsg::csptr _msg ) throw (::fwTools::Failed);

    void stopping() throw (::fwTools::Failed);

    void info(std::ostream &_sstream );

    void save(const ::boost::filesystem::path& path);

};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_SAVELANDMARK_HPP__
