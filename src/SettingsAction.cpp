#include "SettingsAction.h"

#include <iostream>
#include <set>
#include "ATACControllerViewPlugin.h"
#include <string>
#include <QFileDialog>
#include <QPageLayout>
#include <QWebEngineView>
#include <chrono>
#include <typeinfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QSet>
#include <QJsonValue>
using namespace mv;
using namespace mv::gui;

SettingsAction::SettingsAction(ATACControllerViewPlugin& ATACControllerViewPlugin) :
    WidgetAction(&ATACControllerViewPlugin, "ATACControllerViewPlugin"),
    _viewerPlugin(ATACControllerViewPlugin),
    _chartOptionsHolder(*this),
    _computationOptionsHolder(*this)
{
    setSerializationName("ATACControllerViewPlugin:Settings");
    _computationOptionsHolder.getPointDatasetAction().setSerializationName("ATACController:PointDataset");
    _computationOptionsHolder.getClusterDatasetAction().setSerializationName("ATACController:ClusterDataset");
    _computationOptionsHolder.getDimensionPickerAction().setSerializationName("ATACController:DimensionPicker");
    _computationOptionsHolder.getExportDataAction().setSerializationName("ATACController:ExportData");


    _computationOptionsHolder.getPointDatasetAction().setToolTip("Point Dataset");
    _computationOptionsHolder.getClusterDatasetAction().setToolTip("Cluster Dataset");
    _computationOptionsHolder.getDimensionPickerAction().setToolTip("Dimension Picker");
    _computationOptionsHolder.getExportDataAction().setToolTip("ExportData");



    _computationOptionsHolder.getPointDatasetAction().setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == PointType;
        });

    _computationOptionsHolder.getClusterDatasetAction().setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() ==ClusterType;
        });

    _computationOptionsHolder.getPointDatasetAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _computationOptionsHolder.getClusterDatasetAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _computationOptionsHolder.getDimensionPickerAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _computationOptionsHolder.getExportButtonAction().setDefaultWidgetFlags(TriggerAction::IconText);
    _computationOptionsHolder.getExportDataAction().setDefaultWidgetFlags(StringAction::Label);




}

inline SettingsAction::ComputationOptionsHolder::ComputationOptionsHolder(SettingsAction& settingsAction) :
    HorizontalGroupAction(&settingsAction, "Computation Options"),
    _settingsOptions(settingsAction),
    _pointDatasetAction(this, "Point dataset"),
    _clusterDatasetAction(this, "Cluster dataset"),
    _dimensionPickerAction(this, "Dimension"),
    _exportButtonAction(this, "Data export"),
    _exportDataAction(this, "Export data")
{
    setText("Dataset1 Options");
    setIcon(mv::util::StyledIcon("database"));
    setPopupSizeHint(QSize(300, 0));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::Default);
    addAction(&_pointDatasetAction);
    addAction(&_clusterDatasetAction);
    addAction(&_dimensionPickerAction);
    addAction(&_exportButtonAction);
}

inline SettingsAction::ChartOptionsHolder::ChartOptionsHolder(SettingsAction& settingsAction) :
    HorizontalGroupAction(&settingsAction, "Chart Options"),
    _settingsOptions(settingsAction)
{
    setText("Dataset1 Options");
    setIcon(mv::util::StyledIcon("database"));
    setPopupSizeHint(QSize(300, 0));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::Default);

}

void SettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);
    _computationOptionsHolder.getPointDatasetAction().fromParentVariantMap(variantMap);
    _computationOptionsHolder.getClusterDatasetAction().fromParentVariantMap(variantMap);
    _computationOptionsHolder.getDimensionPickerAction().fromParentVariantMap(variantMap);
    _computationOptionsHolder.getExportDataAction().fromParentVariantMap(variantMap);
}

QVariantMap SettingsAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();
    _computationOptionsHolder.getPointDatasetAction().insertIntoVariantMap(variantMap);
    _computationOptionsHolder.getClusterDatasetAction().insertIntoVariantMap(variantMap);
    _computationOptionsHolder.getDimensionPickerAction().insertIntoVariantMap(variantMap);
    _computationOptionsHolder.getExportDataAction().insertIntoVariantMap(variantMap);

    return variantMap;
}