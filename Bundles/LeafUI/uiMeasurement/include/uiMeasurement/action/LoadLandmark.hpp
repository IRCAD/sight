/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_LOADLANDMARK_HPP__
#define __UIMEASUREMENT_ACTION_LOADLANDMARK_HPP__

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

#include <boost/filesystem/path.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief This action allows to load landmarks from a file
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="::uiMeasurement::action::LoadLandmark" >
        <inout key="image" uid="..." />
    </service>
   @endcode
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: Image containig the landmarks field.
 */
class UIMEASUREMENT_CLASS_API LoadLandmark : public ::fwGui::IActionSrv
{
public:
    fwCoreServiceClassDefinitionsMacro( (LoadLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API LoadLandmark() throw();

    UIMEASUREMENT_API virtual ~LoadLandmark() throw();

protected:

    void configuring() throw (::fwTools::Failed);

    void starting() throw (::fwTools::Failed);

    void updating() throw (::fwTools::Failed);

    void stopping() throw (::fwTools::Failed);

    void info(std::ostream& _sstream );

    void load(const ::boost::filesystem::path& path);
};

} // namespace action

} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_LOADLANDMARK_HPP__
