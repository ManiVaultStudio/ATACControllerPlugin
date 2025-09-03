#pragma once

#include <actions/WidgetAction.h>
#include <actions/IntegralAction.h>
#include <actions/OptionAction.h>
#include <actions/OptionsAction.h>
#include <actions/VariantAction.h>
#include "ClusterData/ClusterData.h"
#include "PointData/PointData.h"
#include <actions/ToggleAction.h>
#include "actions/DatasetPickerAction.h"
#include <PointData/DimensionPickerAction.h>
#include "event/EventListener.h"
#include "actions/Actions.h"
#include "Plugin.h"
#include "DataHierarchyItem.h"
#include "Set.h"
#include <AnalysisPlugin.h>
#include <memory>
#include <algorithm>    
#include <QDebug>
#include <QLabel>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QGridLayout>
#include <QFormLayout>
#include <QString>
#include <string>
#include <QRadioButton>
#include <event/Event.h>
#include <PointData/DimensionPickerAction.h>
#include <QDebug>
#include <QLabel>
#include <string>
#include <actions/ColorMap1DAction.h>
#include <set>
#include <actions/HorizontalToolbarAction.h>
#include <actions/VerticalToolbarAction.h>
#include "QStatusBar"
#include <./actions/ToggleAction.h>

using namespace mv::gui;
class QMenu;
class ATACControllerViewPlugin;

namespace mv
{
    class CoreInterface;
}

class SettingsAction : public WidgetAction
{
public:
    class ChartOptionsHolder : public HorizontalGroupAction
    {
    public:
        ChartOptionsHolder(SettingsAction& settingsAction);


    protected:
        SettingsAction& _settingsOptions;

    };

    class ComputationOptionsHolder : public HorizontalGroupAction
    {
    public:
        ComputationOptionsHolder(SettingsAction& settingsAction);
        const DatasetPickerAction& getClusterDatasetAction() const { return _clusterDatasetAction; }
        DatasetPickerAction& getClusterDatasetAction() { return _clusterDatasetAction; }

        const DatasetPickerAction& getPointDatasetAction() const { return _pointDatasetAction; }
        DatasetPickerAction& getPointDatasetAction() { return _pointDatasetAction; }

        const DimensionPickerAction& getDimensionPickerAction() const { return _dimensionPickerAction; }
        DimensionPickerAction& getDimensionPickerAction() { return _dimensionPickerAction; }

        const TriggerAction& getExportButtonAction() const { return _exportButtonAction; }
        TriggerAction& getExportButtonAction() { return _exportButtonAction; }

        const VariantAction& getExportDataAction() const { return _exportDataAction; }
        VariantAction& getExportDataAction() { return _exportDataAction; }

    protected:
        SettingsAction& _settingsOptions;
        DatasetPickerAction     _clusterDatasetAction;
        DatasetPickerAction     _pointDatasetAction;
        DimensionPickerAction   _dimensionPickerAction;
        TriggerAction           _exportButtonAction;
        VariantAction           _exportDataAction;
    };

public:
    SettingsAction(ATACControllerViewPlugin& ATACControllerViewPlugin);

    ChartOptionsHolder& getChartOptionsHolder() { return _chartOptionsHolder; }
    ComputationOptionsHolder& getComputationOptionsHolder() { return _computationOptionsHolder; }

public:
    void fromVariantMap(const QVariantMap& variantMap) override;
    QVariantMap toVariantMap() const override;



protected:
    ATACControllerViewPlugin& _viewerPlugin;
    mv::CoreInterface* _core;
    ChartOptionsHolder _chartOptionsHolder;
    ComputationOptionsHolder _computationOptionsHolder;


    friend class ChannelAction;
};