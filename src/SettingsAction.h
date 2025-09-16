#pragma once

#include <actions/WidgetAction.h>
#include <actions/IntegralAction.h>
#include <actions/OptionAction.h>
#include <actions/OptionsAction.h>
#include <actions/VariantAction.h>
#include "ClusterData/ClusterData.h"
#include "PointData/PointData.h"
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
    class ChartOptionsHolder : public VerticalGroupAction
    {
    public:
        ChartOptionsHolder(SettingsAction& settingsAction);

        //const TriggerAction& getClearChartDataAction() const { return _clearChartDataAction; }
        //TriggerAction& getClearChartDataAction() { return _clearChartDataAction; }
        //const ToggleAction& getShowLegendAction() const { return _showLegendAction; }
        //ToggleAction& getShowLegendAction() { return _showLegendAction; }
        //const OptionAction& getLegendPositionAction() const { return _legendPositionAction; }
        //OptionAction& getLegendPositionAction() { return _legendPositionAction; }
        const IntegralAction& getLegendFontsizeAction() const { return _legendFontsizeAction; }
        IntegralAction& getLegendFontsizeAction() { return _legendFontsizeAction; }
        //const OptionAction& getLegendStyleAction() const { return _legendStyleAction; }
        //OptionAction& getLegendStyleAction() { return _legendStyleAction; }
        const ToggleAction& getRoundedBarsAction() const { return _roundedBarsAction; }
        ToggleAction& getRoundedBarsAction() { return _roundedBarsAction; }
        const OptionAction& getStackdirectionAction() const { return _stackdirectionAction; }
        OptionAction& getStackdirectionAction() { return _stackdirectionAction; }
        //const IntegralAction& getBarSpacingAction() const { return _barSpacingAction; }
        //IntegralAction& getBarSpacingAction() { return _barSpacingAction; }
        const IntegralAction& getBarWidthAction() const { return _barWidthAction; }
        IntegralAction& getBarWidthAction() { return _barWidthAction; }
        //const ColorAction& getBarBorderColorAction() const { return _barBorderColorAction; }
        //ColorAction& getBarBorderColorAction() { return _barBorderColorAction; }
        //const IntegralAction& getBarBorderThicknessAction() const { return _barBorderThicknessAction; }
        //IntegralAction& getBarBorderThicknessAction() { return _barBorderThicknessAction; }
        const IntegralAction& getBarCornerRadiusAction() const { return _barCornerRadiusAction; }
        IntegralAction& getBarCornerRadiusAction() { return _barCornerRadiusAction; }
        //const DecimalAction& getBarOpacityAction() const { return _barOpacityAction; }
        //DecimalAction& getBarOpacityAction() { return _barOpacityAction; }
        const ToggleAction& getShowValuesOnSegmentsAction() const { return _showValuesOnSegmentsAction; }
        ToggleAction& getShowValuesOnSegmentsAction() { return _showValuesOnSegmentsAction; }
        const ToggleAction& getShowAxesAction() const { return _showAxesAction; }
        ToggleAction& getShowAxesAction() { return _showAxesAction; }
        const IntegralAction& getAxesFontsizeAction() const { return _axesFontsizeAction; }
        IntegralAction& getAxesFontsizeAction() { return _axesFontsizeAction; }
        const ToggleAction& getShowGridAction() const { return _showGridAction; }
        ToggleAction& getShowGridAction() { return _showGridAction; }
        //const ColorAction& getGridColorAction() const { return _gridColorAction; }
        //ColorAction& getGridColorAction() { return _gridColorAction; }
        //const IntegralAction& getAnimationDurationAction() const { return _animationDurationAction; }
        //IntegralAction& getAnimationDurationAction() { return _animationDurationAction; }
        const ColorAction& getHighlightColorAction() const { return _highlightColorAction; }
        ColorAction& getHighlightColorAction() { return _highlightColorAction; }



    protected:
        SettingsAction& _settingsOptions;

        // temporarily disabled
        //TriggerAction    _clearChartDataAction;
        //ToggleAction     _showLegendAction;
        //OptionAction     _legendPositionAction;
        //OptionAction     _legendStyleAction;
        //IntegralAction   _barSpacingAction;
        //ColorAction      _barBorderColorAction;
        //IntegralAction   _barBorderThicknessAction;
        //DecimalAction    _barOpacityAction;
        //ColorAction      _gridColorAction;
        //IntegralAction   _animationDurationAction;

        IntegralAction   _legendFontsizeAction;       
        ToggleAction     _roundedBarsAction;
        IntegralAction   _barWidthAction;
        IntegralAction   _barCornerRadiusAction;
        OptionAction     _stackdirectionAction;      
        ToggleAction     _showValuesOnSegmentsAction;
        ToggleAction     _showAxesAction;      
        IntegralAction   _axesFontsizeAction;
        ToggleAction     _showGridAction;
        ColorAction      _highlightColorAction;

    };

    class DataOptionsHolder : public VerticalGroupAction
    {
    public:
        DataOptionsHolder(SettingsAction& settingsAction);
        const DatasetPickerAction& getClusterDatasetAction() const { return _clusterDatasetAction; }
        DatasetPickerAction& getClusterDatasetAction() { return _clusterDatasetAction; }

        const DatasetPickerAction& getPointDatasetAction() const { return _pointDatasetAction; }
        DatasetPickerAction& getPointDatasetAction() { return _pointDatasetAction; }

    protected:
        SettingsAction& _settingsOptions;
        DatasetPickerAction     _clusterDatasetAction;
        DatasetPickerAction     _pointDatasetAction;
    };

    class ComputationOptionsHolder : public HorizontalGroupAction
    {
    public:
        ComputationOptionsHolder(SettingsAction& settingsAction);

        const DimensionPickerAction& getDimensionPickerAction() const { return _dimensionPickerAction; }
        DimensionPickerAction& getDimensionPickerAction() { return _dimensionPickerAction; }

    protected:
        SettingsAction& _settingsOptions;
        DimensionPickerAction   _dimensionPickerAction;
    };

public:
    SettingsAction(ATACControllerViewPlugin& ATACControllerViewPlugin);

    ChartOptionsHolder& getChartOptionsHolder() { return _chartOptionsHolder; }
    ComputationOptionsHolder& getComputationOptionsHolder() { return _computationOptionsHolder; }
    DataOptionsHolder& getDataOptionsHolder() { return _dataOptionsHolder; }

public:
    void fromVariantMap(const QVariantMap& variantMap) override;
    QVariantMap toVariantMap() const override;

protected:
    ATACControllerViewPlugin& _viewerPlugin;
    mv::CoreInterface* _core;
    ChartOptionsHolder _chartOptionsHolder;
    ComputationOptionsHolder _computationOptionsHolder;
    DataOptionsHolder _dataOptionsHolder;

    friend class ChannelAction;
};