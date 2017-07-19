/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEASUREMENT_ACTION_SFOCUSLANDMARK_HPP__
#define __UIMEASUREMENT_ACTION_SFOCUSLANDMARK_HPP__

#include "uiMeasurement/config.hpp"

#include <fwGui/IActionSrv.hpp>

namespace uiMeasurement
{
namespace action
{

/**
 * @brief This action moves the image slice on selected landmark.
 *
 * @section Slots Slots
 * - \b selectLandmark(groupName, index) : keep a reference to the selected landmark
 * - \b deselectLandmark(groupName, index) : deselect the landmark
 * - \b deselectFromGroup(groupName) : deselect the landmark if it is in the group
 * - \b renameGroup(oldGroupName, newGroupName) : rename the selected landmark group
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::uiMeasurement::action::SSFocusLandmark">
       <in key="landmarks" uid="..." autoConnect="yes" />
       <inout key="image" uid="..." />
   </service>
   @endcode
 * @subsection Input Input
 * - \b landmarks [::fwData::Landmarks]: landmarks to focus.
 * @subsection In-Out In-Out
 * - \b image [::fwData::Image]: image to modify the slice.
 */
class UIMEASUREMENT_CLASS_API SFocusLandmark : public ::fwGui::IActionSrv
{

public:
    fwCoreServiceClassDefinitionsMacro( (SFocusLandmark)( ::fwGui::IActionSrv) );

    UIMEASUREMENT_API SFocusLandmark() noexcept;

    UIMEASUREMENT_API virtual ~SFocusLandmark() noexcept;

    /// Defines connection to Landmarks data
    UIMEASUREMENT_API KeyConnectionsMap getAutoConnections() const;

protected:

    /// Do nothing
    UIMEASUREMENT_API void starting();

    /// Do nothing
    UIMEASUREMENT_API void stopping();

    /// Do nothing
    UIMEASUREMENT_API void configuring();

    /// Focus the image slices on the selected landmark
    UIMEASUREMENT_API void updating();

private:
    /// Slot: keep a reference to the selected landmark
    void selectLandmark(std::string groupName, size_t index);

    /// Slot: deselect the landmark
    void deselectLandmark(std::string groupName, size_t index);

    /// Slot: deselect the landmark if it is in the group
    void deselectFromGroup(std::string groupName);

    /// Slot: rename the selected landmark group
    void renameGroup(std::string oldGroupName, std::string newGroupName);

    std::string m_groupName;
    size_t m_index;
    bool m_isSelected;
};

} // namespace action
} // namespace uiMeasurement

#endif // __UIMEASUREMENT_ACTION_SFOCUSLANDMARK_HPP__
