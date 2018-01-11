/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UINETWORK_COMPOSITECONNECTORWIDGET_HPP__
#define __UINETWORK_COMPOSITECONNECTORWIDGET_HPP__

#include "uiNetwork/ObjectClassSelectorDialog.hpp"
#include "uiNetwork/AObjectConnectorWidget.hpp"
#include "uiNetwork/config.hpp"

#include <fwData/Composite.hpp>
#include <fwData/Vector.hpp>

#include <QVBoxLayout>

namespace uiNetwork
{
/**
 *
 * @brief CompositeConnectorWidget used to represent graphically container of object
 */
class UINETWORK_CLASS_API CompositeConnectorWidget : public AObjectConnectorWidget
{
Q_OBJECT

public:
    /**
     * @typedef ItemMapType
     *
     * @brief map of obj id -> AObjectConnectorWidget
     */
    typedef std::map<std::string, AObjectConnectorWidget*> ItemMapType;


    /// constructor
    UINETWORK_API CompositeConnectorWidget(QTreeWidget* root,
                                           ::fwData::Object::sptr obj,
                                           ConnectorModel const& connectorModel);

    /// Destructor
    UINETWORK_API ~CompositeConnectorWidget();

    /**
     * @see AObjectConnectorWidget
     */
    UINETWORK_API bool match(::boost::regex const& regex);

    /**
     * @see AObjectConnectorWidget
     */
    UINETWORK_API void refresh(::boost::regex const& regex);
private:

    /**
     * @brief Look into the data object and fill the tree widget
     *
     * @param[in] obj container to iterate
     * @param[in] regex regex to filter which object could be added to the tree
     * @param[in] itemSnapshot item copied to check if a object is removed(like a snapshot)
     */
    void fillTreeWidget(::fwData::Object::sptr obj,
                        ::boost::regex const& regex,
                        ItemMapType& itemSnapshot);

    /**
     * @brief Remove connector (used if an object is no longer exist)
     */
    void removeUnusedConnectors(ItemMapType const& removeItems);

private Q_SLOTS:
    /**
     * @brief show the ObjectClassSelectorDialog when user click on add object button
     */
    void onAddObject();

    /**
     * @brief add object to the container when user selected a object classname to add
     */
    void onClassnameSelected(std::string const& classname);

private:
    /// AObjectConnectorWidget for each object id
    ItemMapType m_items;

    /// Class dialog selector
    ObjectClassSelectorDialog m_classSelectorDialog;
};
} //namespace uiNetwork

#endif // __UINETWORK_COMPOSITECONNECTORWIDGET_HPP__
