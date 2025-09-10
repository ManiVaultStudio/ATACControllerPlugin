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
#include<QRadioButton>
#include <event/Event.h>
#include<actions/ColorMap1DAction.h>
#include <set>
#include <actions/HorizontalToolbarAction.h>
#include <actions/VerticalToolbarAction.h>
#include "QStatusBar"

using namespace mv::gui;
class QMenu;
class ATACControllerViewPlugin;
class SettingsAction;


namespace mv
{
    class CoreInterface;
}


class Computation : public WidgetAction
{

public:
    Computation(ATACControllerViewPlugin& ATACControllerViewPlugin, SettingsAction& SettingsAction);
    void highlightTriggered();
    void clickTriggered();
private:
    std::tuple<QStringList, QVector<QVector<float>>, QVector<QColor>> computeMetadataCounts(const QVector<Cluster>& metadata, const std::vector<int>& topPoints); // TODO: to remove

    std::tuple<QStringList, QVector<QVector<float>>, QVector<QColor>> computeMetadataCounts(const QVector<Cluster>& metadata, const std::vector<int>& topPoints, int numPoints);

    void prepareChartData();
protected:
    ATACControllerViewPlugin&  _viewerPlugin;
    SettingsAction&            _settingsAction;
    mv::Dataset<Points>        _points;                    /** Points smart pointer */
    mv::Dataset<Clusters>      _clusters;                  /** Clusters smart pointer */

    bool _dimensionBlockerFlag = false;
    friend class ChannelAction;
};