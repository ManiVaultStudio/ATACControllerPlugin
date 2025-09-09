#pragma once

#include <ViewPlugin.h>
#include "StackedBarChartWidget.h"
#include <Dataset.h>
#include <widgets/DropWidget.h>

#include <PointData/PointData.h>
#include <ClusterData/ClusterData.h>
#include <QWidget>

#include "SettingsAction.h"
#include "Computation.h"

/** All plugin related classes are in the ManiVault plugin namespace */
using namespace mv::plugin;

/** Drop widget used in this plugin is located in the ManiVault gui namespace */
using namespace mv::gui;

/** Dataset reference used in this plugin is located in the ManiVault util namespace */
using namespace mv::util;

class QLabel;

class ATACControllerViewPlugin : public ViewPlugin
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param factory Pointer to the plugin factory
     */
    ATACControllerViewPlugin(const PluginFactory* factory);

    /** Destructor */
    ~ATACControllerViewPlugin() override = default;
    
    /** This function is called by the core after the view plugin has been created */
    void init() override;
    StackedBarChartWidget* getStackedBarChartWidget() const { return _stackedBarChartWidget; }


protected:
    SettingsAction          _settingsAction;
    Computation             _computation;
    StackedBarChartWidget*  _stackedBarChartWidget;
};

/**
 * ATACController view plugin factory class
 *
 * Note: Factory does not need to be altered (merely responsible for generating new plugins when requested)
 */
class ATACControllerViewPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(mv::plugin::ViewPluginFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "studio.manivault.ATACControllerViewPlugin"
                      FILE  "PluginInfo.json")

public:

    /** Default constructor */
    ATACControllerViewPluginFactory();

    /** Creates an instance of the ATACController view plugin */
    ViewPlugin* produce() override;

    /** Returns the data types that are supported by the ATACController view plugin */
    mv::DataTypes supportedDataTypes() const override;

    /**
     * Get plugin trigger actions given \p datasets
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    PluginTriggerActions getPluginTriggerActions(const mv::Datasets& datasets) const override;
};
