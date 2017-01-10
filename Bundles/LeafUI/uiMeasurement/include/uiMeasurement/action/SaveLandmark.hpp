/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_SAVELANDMARK_HPP__
#define __UIMEASUREMENT_ACTION_SAVELANDMARK_HPP__

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief This action allows to save the landmarks stored in image.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiMeasurement::action::SaveLandmark" >
        <in key="image" uid="..." />
    </service>
   @endcode
 * @subsection Input Input
 * - \b image [::fwData::Image]: Image containig the landmarks field.
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

    void stopping() throw (::fwTools::Failed);

    void info(std::ostream& _sstream );

    void save(const ::boost::filesystem::path& path);

};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_SAVELANDMARK_HPP__
