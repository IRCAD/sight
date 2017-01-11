/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIMEDDATAQT_WIDGET_ACTIVITYDATAVIEW_HPP__
#define __UIMEDDATAQT_WIDGET_ACTIVITYDATAVIEW_HPP__

#include "uiMedDataQt/config.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwData/Composite.hpp>

#include <fwMedData/ActivitySeries.hpp>

#include <QEvent>
#include <QPointer>
#include <QTabWidget>
#include <QTreeWidget>

namespace uiMedData
{
namespace widget
{

/**
 * @brief This class displays a tab widget allowing to select the required data to create an activity.
 */
class UIMEDDATAQT_CLASS_API ActivityDataView : public QTabWidget
{
Q_OBJECT;

public:

    typedef std::map< std::string, std::string > ObjectIconMapType;

    UIMEDDATAQT_API ActivityDataView(QWidget *parent = 0);

    UIMEDDATAQT_API virtual ~ActivityDataView();

    /// Identifier of the role UID in the series tree item.
    UIMEDDATAQT_API static const int s_UID_ROLE;

    enum class ColumnType : int
    {
        NAME,
        TYPE,
        DESC,
        PATIENT,
        STUDY
    };

    /// Fill tab widget with activity info (create on item by activity parameter)
    UIMEDDATAQT_API void fillInformation(const ::fwActivities::registry::ActivityInfo & info);

    /// Fill widget from existing activity info (create on item by activity parameter)
    UIMEDDATAQT_API void fillInformation(const ::fwMedData::ActivitySeries::sptr & actSeries);

    /**
     * @brief Check if all the required data are selected and fill activity data composite.
     * @return True if data are correct, else false and errorMsg contains detailled error.
     * @param[in] actSeries activitySeries to fill with the selected data
     * @param[out] errorMsg it will contain the error information if the data are not correct.
     *
     * It checks if all required data are present in the widget, and create the activity data composite.
     * Then it checks if the activity parameters are accessible from the composite of data (it checks if the object are
     * accessible from the sesh@ paths.
     */
    UIMEDDATAQT_API bool checkAndComputeData(const ::fwMedData::ActivitySeries::sptr & actSeries,
                                             std::string & errorMsg);

    /**
     * @brief Check if the current required data are selected And return this data.
     * @return return the data if it is correct, else return a nullptr and errorMsg contains detailled error.
     * @param[in] index index of the requirement to check (this index also correspond to a tab).
     * @param[out] errorMsg it will contain the error information if the data are not correct.
     */
    UIMEDDATAQT_API ::fwData::Object::sptr checkData(size_t index, std::string & errorMsg);

    /// Set io selector configuration used to import data
    void setIOSelectorConfig(const std::string & ioSelectorSrvConfig)
    {
        m_ioSelectorSrvConfig = ioSelectorSrvConfig;
    }

    /// Remove all the widget tabs.
    UIMEDDATAQT_API virtual void clear();

    /// Set the map that defines the icon associated to an object.
    void setObjectIconAssociation(const ObjectIconMapType & objectIcons)
    {
        m_objectIcons = objectIcons;
    }

protected:

    /// Catch KeyEvent and DropEvent from tree widgets
    bool eventFilter(QObject *obj, QEvent *event);

private:

    typedef std::vector< ::fwData::Object::sptr > ImportedObjectType;
    typedef std::vector<QPointer<QTreeWidget> > TreeContainerType;

    /// Import an object required for the selected tab
    void importObject();

    /// Create a new object for the selected tab (just use ::fwData::factory::New(type))
    void createNewObject();

    /// Call SIOSelector service to read the object. Return true if the object is properly read.
    bool readObject(::fwData::Object::sptr obj);

    /// Removed the selected object in the current tree
    void removeSelectedObjects();

    /// Remove all the objects in the current tree
    void clearTree();

    /**
     * @brief Called on tree item double click : allows to edit the current data.
     * @note Currently it is only available for simple types (ie ::fwData::String)
     */
    void onTreeItemDoubleClicked(QTreeWidgetItem *item, int column);

    /**
     * @brief Add the current item in the tree.
     * @param index index used to find the associated tree widget
     * @param obj object to add in the tree
     */
    void addObjectItem(size_t index, const ::fwData::Object::sptr &obj);

    ::fwActivities::registry::ActivityInfo m_activityInfo; ///< activity information

    std::string m_ioSelectorSrvConfig; ///< io selector config

    ImportedObjectType m_importedObject; ///< refereces on the imported before to add them in the activity series.
    TreeContainerType m_treeWidgets; ///< Register the tree widget of each tab.

    ObjectIconMapType m_objectIcons; ///< Map defining the icon associated to an object.
};


} // namespace widget
} // namespace uiMedData

#endif // __UIMEDDATAQT_WIDGET_ACTIVITYDATAVIEW_HPP__
