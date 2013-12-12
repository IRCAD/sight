/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UISPLINE_SSPLINEPOINTSEDITOR_HPP__
#define __UISPLINE_SSPLINEPOINTSEDITOR_HPP__

#include <vector>
#include <string>

#include <QObject>
#include <QPointer>
#include <QListWidget>
#include <QPushButton>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include <fwData/Point.hpp>

#include <fwThread/Timer.hpp>

#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <navigation/ConnectPoints.hpp>

#include "uiSpline/config.hpp"

class QListWidgetItem;

namespace fwData
{
    class PointList;
}

namespace uiSpline
{

/**
 * @brief Lists spline points.
 * @class SSplinePointsEditor
 */
class UISPLINE_CLASS_API SSplinePointsEditor : public QObject, public ::gui::editor::IEditor
{
    Q_OBJECT

public :

    fwCoreServiceClassDefinitionsMacro ( (SSplinePointsEditor)(::gui::editor::IEditor) ) ;

    /// Constructor.
    UISPLINE_API SSplinePointsEditor () throw() ;

    /// Destructor.
    UISPLINE_API virtual ~SSplinePointsEditor () throw() ;

    /**
     * @name Signal types.
     * @{ */
    typedef ::fwCom::Signal< void(::fwData::Point::sptr) > PointSelectedSignalType;
    typedef ::fwCom::Signal< void(int) > IndexPointSelectedSignalType;
    /**  @} */

    /**
     * @name Signal keys.
     * @{ */
    UISPLINE_API static const ::fwCom::Signals::SignalKeyType s_INDEX_POINT_SELECTED_SIG;
    UISPLINE_API static const ::fwCom::Signals::SignalKeyType s_POINT_SELECTED_SIG;
    /**  @} */

    /// Name of field associated to view points containing view point name.
    UISPLINE_API static const std::string s_FIELD_NAME;

protected:

    /**
     * @name Overrides
     * @{ */
    virtual void starting() throw(::fwTools::Failed);
    virtual void stopping() throw(::fwTools::Failed);
    virtual void receiving( ::fwServices::ObjectMsg::csptr _msg  ) throw(::fwTools::Failed);
    virtual void updating() throw(::fwTools::Failed);
    /**  @} */

    /**
     * @brief Overrides IService::configuring().
     * Configuration example :
     @verbatim
     <config>
        <points uid="..." /> <!-- ::fwData::PointList containing spline points to display -->
     </config>
     @endverbatim
     *
     * @throw fwTools::Failed
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Signal emitted when a spline point is selected.
    PointSelectedSignalType::sptr m_sigPointSelected;

    /// Signal emitted when a spline point is selected.
    IndexPointSelectedSignalType::sptr m_sigIndexPointSelected;

    /**
     * @brief Fills the visualizePointList with the points we want to visualize.
     *
     * @param selectedPointIndex index of point to visualize in associated point list
     */
    void fillVisualizePointList(int selectedPointIndex);

    /**
     * @brief Adds point to visualize point list.
     *
     * @param pointList source points
     * @param selectedPointIndex index of source point
     */
    void addPointToVisualizePointList(const SPTR(::fwData::PointList)& pointList, int selectedPointIndex);

    /// Clears the visualizePointList
    void clearVisualizePointList();

    /// Updates the point list when a point is modified
    void updatePointList();

    /**
     * @name Slot type and key.
     * @{ */
    UISPLINE_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_POINTLIST_SLOT;
    typedef ::fwCom::Slot<void()> UpdatePointListSlotType;
    /**  @} */

protected Q_SLOTS:

    /// Called when the "Rename point" button is clicked.
    void onClickRenamePoint();

    /// Called when the "Remove point" button is clicked.
    void onClickRemovePoint();

    /// Called when the "Remove all points" button is clicked.
    void onClickRemoveAllPoints();

    /// Called when an item in the QListWidget is clicked.
    void onClickItem(QListWidgetItem * item);

    /// Called when an item in the QListWidget is double clicked.
    void onDoubleClickItem(QListWidgetItem * item);

private:

    /// Current number of view points
    int m_numberOfPoints;

    /// Number of next point to be added.
    int m_countPoint;

    /// UID of the point list containing points to be displayed into a scene.
    std::string m_selectedPointsUID;

    /// List of registered points
    QPointer< QListWidget> m_list;

    /// Button to rename selected point.
    QPointer< QPushButton > m_renamePointButton;

    /// Button to remove selected point.
    QPointer< QPushButton > m_removePointButton;

    /// Button to remove all points.
    QPointer< QPushButton > m_removeAllPointsButton;

    /// Helper to connect 
    ::navigation::ConnectPoints::sptr m_connectObj;

    /// Slot called to update the point list of view points.
    UpdatePointListSlotType::sptr m_slotUpdatePointList;
};

} // uiSpline

#endif // __UISPLINE_SSPLINEPOINTSEDITOR_HPP__

