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
    _computationOptionsHolder(*this),
    _dataOptionsHolder(*this)
{
    setSerializationName("ATACControllerViewPlugin:Settings");
    _dataOptionsHolder.getPointDatasetAction().setSerializationName("ATACController:PointDataset");
    _dataOptionsHolder.getClusterDatasetAction().setSerializationName("ATACController:ClusterDataset");
    _computationOptionsHolder.getDimensionPickerAction().setSerializationName("ATACController:DimensionPicker");

    _dataOptionsHolder.getPointDatasetAction().setToolTip("Point Dataset");
    _dataOptionsHolder.getClusterDatasetAction().setToolTip("Cluster Dataset");
    _computationOptionsHolder.getDimensionPickerAction().setToolTip("Dimension Picker");

    _dataOptionsHolder.getPointDatasetAction().setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() == PointType;
        });
    _dataOptionsHolder.getClusterDatasetAction().setFilterFunction([this](mv::Dataset<DatasetImpl> dataset) -> bool {
        return dataset->getDataType() ==ClusterType;
        });

    _dataOptionsHolder.getPointDatasetAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _dataOptionsHolder.getClusterDatasetAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _computationOptionsHolder.getDimensionPickerAction().setDefaultWidgetFlags(OptionAction::ComboBox);

    //chart customization options
    //_chartOptionsHolder.getShowLegendAction().setSerializationName("ATACController:ChartCustomization:Show Legend");
    //_chartOptionsHolder.getShowLegendAction().setToolTip("Show Legend");
    //_chartOptionsHolder.getShowLegendAction().setDefaultWidgetFlags(ToggleAction::CheckBox);
    //_chartOptionsHolder.getShowLegendAction().setChecked(true);

    //_chartOptionsHolder.getLegendPositionAction().setSerializationName("ATACController:ChartCustomization:Legend Position");
    //_chartOptionsHolder.getLegendPositionAction().setToolTip("Legend Position");
    //_chartOptionsHolder.getLegendPositionAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    //_chartOptionsHolder.getLegendPositionAction().setOptions({ "top","bottom","left","right" });
    //_chartOptionsHolder.getLegendPositionAction().setCurrentText("right");

    _chartOptionsHolder.getLegendFontsizeAction().setSerializationName("ATACController:ChartCustomization:Legend Font Size");
    _chartOptionsHolder.getLegendFontsizeAction().setToolTip("Legend Font Size");
    _chartOptionsHolder.getLegendFontsizeAction().setDefaultWidgetFlags(IntegralAction::Slider);
    _chartOptionsHolder.getLegendFontsizeAction().setRange(6, 36);
    _chartOptionsHolder.getLegendFontsizeAction().setValue(12);

    //_chartOptionsHolder.getLegendStyleAction().setSerializationName("ATACController:ChartCustomization:Legend Style");
    //_chartOptionsHolder.getLegendStyleAction().setToolTip("Legend Style");
    //_chartOptionsHolder.getLegendStyleAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    //_chartOptionsHolder.getLegendStyleAction().setOptions({ "solid",
    //"dense",
    //"dashed",
    //"crossed",
    //"horizontal",
    //"vertical",
    //"no brush"});
    //_chartOptionsHolder.getLegendStyleAction().setCurrentText("solid");

    _chartOptionsHolder.getRoundedBarsAction().setSerializationName("ATACController:ChartCustomization:Rounded Bars");
    _chartOptionsHolder.getRoundedBarsAction().setToolTip("Rounded Bars");
    _chartOptionsHolder.getRoundedBarsAction().setDefaultWidgetFlags(ToggleAction::CheckBox);
    _chartOptionsHolder.getRoundedBarsAction().setChecked(true);

    _chartOptionsHolder.getStackdirectionAction().setSerializationName("ATACController:ChartCustomization:Stack Direction");
    _chartOptionsHolder.getStackdirectionAction().setToolTip("Stack Direction");
    _chartOptionsHolder.getStackdirectionAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _chartOptionsHolder.getStackdirectionAction().setOptions({ "vertical","horizontal" });
    _chartOptionsHolder.getStackdirectionAction().setCurrentText("vertical");

    //_chartOptionsHolder.getBarSpacingAction().setSerializationName("ATACController:ChartCustomization:Bar Spacing");
    //_chartOptionsHolder.getBarSpacingAction().setToolTip("Bar Spacing");
    //_chartOptionsHolder.getBarSpacingAction().setDefaultWidgetFlags(IntegralAction::Slider);
    //_chartOptionsHolder.getBarSpacingAction().setRange(0, 50);
    //_chartOptionsHolder.getBarSpacingAction().setValue(5);

    _chartOptionsHolder.getBarWidthAction().setSerializationName("ATACController:ChartCustomization:Bar Width");
    _chartOptionsHolder.getBarWidthAction().setToolTip("Bar Width");
    _chartOptionsHolder.getBarWidthAction().setDefaultWidgetFlags(IntegralAction::Slider);
    _chartOptionsHolder.getBarWidthAction().setRange(1, 100);
    _chartOptionsHolder.getBarWidthAction().setValue(40);

    //_chartOptionsHolder.getBarBorderColorAction().setSerializationName("ATACController:ChartCustomization:Bar Border Color");
    //_chartOptionsHolder.getBarBorderColorAction().setToolTip("Bar Border Color");
    //_chartOptionsHolder.getBarBorderColorAction().setDefaultWidgetFlags(ColorAction::Basic);
    //_chartOptionsHolder.getBarBorderColorAction().setColor(Qt::black);

    //_chartOptionsHolder.getBarBorderThicknessAction().setSerializationName("ATACController:ChartCustomization:Bar Border Thickness");
    //_chartOptionsHolder.getBarBorderThicknessAction().setToolTip("Bar Border Thickness");
    //_chartOptionsHolder.getBarBorderThicknessAction().setDefaultWidgetFlags(IntegralAction::Slider);
    //_chartOptionsHolder.getBarBorderThicknessAction().setRange(0, 10);
    //_chartOptionsHolder.getBarBorderThicknessAction().setValue(1);

    _chartOptionsHolder.getBarCornerRadiusAction().setSerializationName("ATACController:ChartCustomization:Bar Corner Radius");
    _chartOptionsHolder.getBarCornerRadiusAction().setToolTip("Bar Corner Radius");
    _chartOptionsHolder.getBarCornerRadiusAction().setDefaultWidgetFlags(IntegralAction::Slider);
    _chartOptionsHolder.getBarCornerRadiusAction().setRange(0, 20);
    _chartOptionsHolder.getBarCornerRadiusAction().setValue(5);

    //_chartOptionsHolder.getBarOpacityAction().setSerializationName("ATACController:ChartCustomization:Bar Opacity");
    //_chartOptionsHolder.getBarOpacityAction().setToolTip("Bar Opacity");
    //_chartOptionsHolder.getBarOpacityAction().setDefaultWidgetFlags(DecimalAction::Slider);
    //_chartOptionsHolder.getBarOpacityAction().setRange(0, 1);
    //_chartOptionsHolder.getBarOpacityAction().setSingleStep(0.01);
    //_chartOptionsHolder.getBarOpacityAction().setValue(0.8);

    _chartOptionsHolder.getShowValuesOnSegmentsAction().setSerializationName("ATACController:ChartCustomization:Show Values On Segments");
    _chartOptionsHolder.getShowValuesOnSegmentsAction().setToolTip("Show Values On Segments");
    _chartOptionsHolder.getShowValuesOnSegmentsAction().setDefaultWidgetFlags(ToggleAction::CheckBox);
    _chartOptionsHolder.getShowValuesOnSegmentsAction().setChecked(true);

    _chartOptionsHolder.getShowAxesAction().setSerializationName("ATACController:ChartCustomization:Show Axes");
    _chartOptionsHolder.getShowAxesAction().setToolTip("Show Axes");
    _chartOptionsHolder.getShowAxesAction().setDefaultWidgetFlags(ToggleAction::CheckBox);
    _chartOptionsHolder.getShowAxesAction().setChecked(true);

    _chartOptionsHolder.getAxesFontsizeAction().setSerializationName("ATACController:ChartCustomization:Axes Font Size");
    _chartOptionsHolder.getAxesFontsizeAction().setToolTip("Axes Font Size");
    _chartOptionsHolder.getAxesFontsizeAction().setDefaultWidgetFlags(IntegralAction::Slider);
    _chartOptionsHolder.getAxesFontsizeAction().setRange(6, 36);
    _chartOptionsHolder.getAxesFontsizeAction().setValue(12);

    _chartOptionsHolder.getShowGridAction().setSerializationName("ATACController:ChartCustomization:Show Grid");
    _chartOptionsHolder.getShowGridAction().setToolTip("Show Grid");
    _chartOptionsHolder.getShowGridAction().setDefaultWidgetFlags(ToggleAction::CheckBox);
    _chartOptionsHolder.getShowGridAction().setChecked(true);

    //_chartOptionsHolder.getGridColorAction().setSerializationName("ATACController:ChartCustomization:Grid Color");
    //_chartOptionsHolder.getGridColorAction().setToolTip("Grid Color");
    //_chartOptionsHolder.getGridColorAction().setDefaultWidgetFlags(ColorAction::Basic);
    //_chartOptionsHolder.getGridColorAction().setColor(Qt::lightGray);

    /*_chartOptionsHolder.getSortingAction().setSerializationName("ATACController:ChartCustomization:Sorting");
    _chartOptionsHolder.getSortingAction().setToolTip("Sorting");
    _chartOptionsHolder.getSortingAction().setDefaultWidgetFlags(OptionAction::ComboBox);
    _chartOptionsHolder.getSortingAction().setOptions({ "total","label" });
    _chartOptionsHolder.getSortingAction().setCurrentText("total");*/

    //_chartOptionsHolder.getAnimationDurationAction().setSerializationName("ATACController:ChartCustomization:Animation Duration");
    //_chartOptionsHolder.getAnimationDurationAction().setToolTip("Animation Duration");
    //_chartOptionsHolder.getAnimationDurationAction().setDefaultWidgetFlags(IntegralAction::Slider);
    //_chartOptionsHolder.getAnimationDurationAction().setRange(0, 5000);
    //_chartOptionsHolder.getAnimationDurationAction().setValue(1000);

    _chartOptionsHolder.getHighlightColorAction().setSerializationName("ATACController:ChartCustomization:Highlight Color");
    _chartOptionsHolder.getHighlightColorAction().setToolTip("Highlight Color");
    _chartOptionsHolder.getHighlightColorAction().setDefaultWidgetFlags(ColorAction::Basic);
    _chartOptionsHolder.getHighlightColorAction().setColor(Qt::blue);

}

inline SettingsAction::ComputationOptionsHolder::ComputationOptionsHolder(SettingsAction& settingsAction) :
    HorizontalGroupAction(&settingsAction, "Computation Options"),
    _settingsOptions(settingsAction),
    _dimensionPickerAction(this, "Dimension")
{
    setText("Dataset1 Options");
    setIcon(mv::util::StyledIcon("database"));
    setPopupSizeHint(QSize(300, 0));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::Default);
    addAction(&_dimensionPickerAction);
}

inline SettingsAction::DataOptionsHolder::DataOptionsHolder(SettingsAction& settingsAction) :
    VerticalGroupAction(&settingsAction, "Data Options"),
    _settingsOptions(settingsAction),
    _pointDatasetAction(this, "Point dataset"),
    _clusterDatasetAction(this, "Cluster dataset")
{
    setText("Dataset1 Options");
    setIcon(mv::util::StyledIcon("database"));
    setPopupSizeHint(QSize(300, 0));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::Default);
    addAction(&_pointDatasetAction);
    addAction(&_clusterDatasetAction);
}

inline SettingsAction::ChartOptionsHolder::ChartOptionsHolder(SettingsAction& settingsAction) :
    VerticalGroupAction(&settingsAction, "Chart Options"),
    _settingsOptions(settingsAction),
    //_clearChartDataAction(this, "Clear Chart Data"),
    //_showLegendAction(this, "Show Legend"),
    //_legendPositionAction(this, "Legend Position"),
    _legendFontsizeAction(this, "Legend Font Size"),
    //_legendStyleAction(this, "Legend Style"),
    _roundedBarsAction(this, "Rounded Bars"),
    _stackdirectionAction(this, "Stack Direction"),
    //_barSpacingAction(this, "Bar Spacing"),
    _barWidthAction(this, "Bar Width"),
    //_barBorderColorAction(this, "Bar Border Color"),
    //_barBorderThicknessAction(this, "Bar Border Thickness"),
    _barCornerRadiusAction(this, "Bar Corner Radius"),
    //_barOpacityAction(this, "Bar Opacity"),
    _showValuesOnSegmentsAction(this, "Show Values On Segments"),
    _showAxesAction(this, "Show Axes"),
    _axesFontsizeAction(this, "Axes Font Size"),
    _showGridAction(this, "Show Grid"),
    //_gridColorAction(this, "Grid Color"),
    _highlightColorAction(this, "Highlight Color")
    //_sortingAction(this, "Sorting")
    //_animationDurationAction(this, "Animation Duration")
{
    setText("Dataset1 Options");
    setIcon(mv::util::StyledIcon("database"));
    setPopupSizeHint(QSize(300, 0));
    setConfigurationFlag(WidgetAction::ConfigurationFlag::Default);
    //addAction(&_clearChartDataAction);
    //addAction(&_showLegendAction);
    //addAction(&_legendPositionAction);
    addAction(&_legendFontsizeAction);
    //addAction(&_legendStyleAction);
    addAction(&_roundedBarsAction);
    addAction(&_stackdirectionAction);
    //addAction(&_barSpacingAction);
    addAction(&_barWidthAction);
    //addAction(&_barBorderColorAction);
    //addAction(&_barBorderThicknessAction);
    addAction(&_barCornerRadiusAction);
    //addAction(&_barOpacityAction);
    addAction(&_showValuesOnSegmentsAction);
    addAction(&_showAxesAction);
    addAction(&_axesFontsizeAction);
    addAction(&_showGridAction);
    //addAction(&_gridColorAction);
    addAction(&_highlightColorAction);
    //addAction(&_sortingAction);
    //addAction(&_animationDurationAction);
}

void SettingsAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);
    _dataOptionsHolder.getPointDatasetAction().fromParentVariantMap(variantMap);
    _dataOptionsHolder.getClusterDatasetAction().fromParentVariantMap(variantMap);
    _computationOptionsHolder.getDimensionPickerAction().fromParentVariantMap(variantMap);

    //_chartOptionsHolder.getShowLegendAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getLegendPositionAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getLegendFontsizeAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getLegendStyleAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getRoundedBarsAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getStackdirectionAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getBarSpacingAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getBarWidthAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getBarBorderColorAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getBarBorderThicknessAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getBarCornerRadiusAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getBarOpacityAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getShowValuesOnSegmentsAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getShowAxesAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getAxesFontsizeAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getShowGridAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getGridColorAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getSortingAction().fromParentVariantMap(variantMap);
    //_chartOptionsHolder.getAnimationDurationAction().fromParentVariantMap(variantMap);
    _chartOptionsHolder.getHighlightColorAction().fromParentVariantMap(variantMap);

}

QVariantMap SettingsAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();
    _dataOptionsHolder.getPointDatasetAction().insertIntoVariantMap(variantMap);
    _dataOptionsHolder.getClusterDatasetAction().insertIntoVariantMap(variantMap);
    _computationOptionsHolder.getDimensionPickerAction().insertIntoVariantMap(variantMap);

    //_chartOptionsHolder.getShowLegendAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getLegendPositionAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getLegendFontsizeAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getLegendStyleAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getRoundedBarsAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getStackdirectionAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getBarSpacingAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getBarWidthAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getBarBorderColorAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getBarBorderThicknessAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getBarCornerRadiusAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getBarOpacityAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getShowValuesOnSegmentsAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getShowAxesAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getAxesFontsizeAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getShowGridAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getGridColorAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getSortingAction().insertIntoVariantMap(variantMap);
    //_chartOptionsHolder.getAnimationDurationAction().insertIntoVariantMap(variantMap);
    _chartOptionsHolder.getHighlightColorAction().insertIntoVariantMap(variantMap);

    return variantMap;
}