#include "Computation.h"


#include <iostream>
#include <set>
#include "ATACControllerViewPlugin.h"
#include "SettingsAction.h"
#include<string>  
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
#include <QtConcurrent>
#include <QColor>
#include "util/StyledIcon.h"
#include <QLabel>
#include <QStatusBar>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>
#include <QToolTip>
#include <QElapsedTimer>
#include <QDebug>
#include <QFuture>
#include <QFutureWatcher>
#include <QVector>
#include <random> 

#include <chrono>
using namespace mv;
using namespace mv::gui;


Computation::Computation(ATACControllerViewPlugin& ATACControllerViewPlugin, SettingsAction& SettingsAction)
    : WidgetAction(&ATACControllerViewPlugin, "Computation"),
    _viewerPlugin(ATACControllerViewPlugin),
    _settingsAction(SettingsAction)
{
    connect(&_points, &Dataset<Points>::changed, this, [this]() {
        qDebug() << "_points changed";
        prepareChartData();
        });

    connect(&_clusters, &Dataset<Clusters>::changed, this, [this]() {
        qDebug() << "_clusters changed";
        prepareChartData();
        });

    connect(&_points, &Dataset<Points>::dataChanged, this, [this]() {
        qDebug() << "_points dataChanged";
        prepareChartData();
        });

    connect(&_clusters, &Dataset<Clusters>::dataChanged, this, [this]() {
        qDebug() << "_clusters dataChanged";
        prepareChartData();
        });

    connect(&_points, &Dataset<Points>::dataDimensionsChanged, this, [this]() {
        qDebug() << "_points dataDimensionsChanged";
        prepareChartData();
        });

    connect(&_settingsAction.getDataOptionsHolder().getPointDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, [this]() {
        qDebug() << "getPointDatasetAction() changed";

        if (_settingsAction.getDataOptionsHolder().getPointDatasetAction().getCurrentDataset().isValid())
        {
            _settingsAction.getComputationOptionsHolder().getDimensionPickerAction().setPointsDataset(_settingsAction.getDataOptionsHolder().getPointDatasetAction().getCurrentDataset());
            _points = _settingsAction.getDataOptionsHolder().getPointDatasetAction().getCurrentDataset();
        }
        else
        {
            _settingsAction.getComputationOptionsHolder().getDimensionPickerAction().setPointsDataset(Dataset<Points>());
            _points = Dataset<Points>();
        }
        });

    connect(&_settingsAction.getDataOptionsHolder().getClusterDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, [this]() {
        qDebug() << "getClusterDatasetAction() changed";

        if (_settingsAction.getDataOptionsHolder().getClusterDatasetAction().getCurrentDataset().isValid())
        {
            _clusters = _settingsAction.getDataOptionsHolder().getClusterDatasetAction().getCurrentDataset();
        }
        else
        {
            _clusters = Dataset<Clusters>();
        }
        });

    connect(&_settingsAction.getComputationOptionsHolder().getDimensionPickerAction(), &DimensionPickerAction::currentDimensionIndexChanged, this, [this]() {
        qDebug() << "dimensionPicker changed";
        prepareChartData(); // FIXME: is it only intended for Spatial data?
        });

    //chartcustomization
    connect(&_settingsAction.getChartOptionsHolder().getShowLegendAction(), &ToggleAction::toggled, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        chartWidget->setShowLegend(_settingsAction.getChartOptionsHolder().getShowLegendAction().isChecked());

        });
    connect(&_settingsAction.getChartOptionsHolder().getLegendPositionAction(), &OptionAction::currentIndexChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        QString pos = _settingsAction.getChartOptionsHolder().getLegendPositionAction().getCurrentText();
        if (pos == "right")
        {
            chartWidget->setLegendPosition(StackedBarChartWidget::LegendRight);
        }
        else if (pos == "left")
        {
            chartWidget->setLegendPosition(StackedBarChartWidget::LegendLeft);
        }
        else if (pos == "top")
        {
            chartWidget->setLegendPosition(StackedBarChartWidget::LegendTop);
        }
        else if (pos == "bottom")
        {
            chartWidget->setLegendPosition(StackedBarChartWidget::LegendBottom);
        }

        });
    connect(&_settingsAction.getChartOptionsHolder().getLegendFontsizeAction(), &IntegralAction::valueChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        int fontSize = _settingsAction.getChartOptionsHolder().getLegendFontsizeAction().getValue();
        chartWidget->setLegendFont(QFont("Arial", fontSize));

        });
    connect(&_settingsAction.getChartOptionsHolder().getLegendStyleAction(), &OptionAction::currentIndexChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        QString style = _settingsAction.getChartOptionsHolder().getLegendStyleAction().getCurrentText();
        if (style == "solid")
        {
            chartWidget->setLegendStyle(Qt::SolidPattern);
        }
        else if (style == "dense")
        {
            chartWidget->setLegendStyle(Qt::Dense1Pattern);
        }
        else if (style == "dashed")
        {
            chartWidget->setLegendStyle(Qt::DiagCrossPattern);
        }
        else if (style == "crossed")
        {
            chartWidget->setLegendStyle(Qt::CrossPattern);
        }
        else if (style == "horizontal")
        {
            chartWidget->setLegendStyle(Qt::HorPattern);
        }
        else if (style == "vertical")
        {
            chartWidget->setLegendStyle(Qt::VerPattern);
        }
        else if (style == "no brush")
        {
            chartWidget->setLegendStyle(Qt::NoBrush);
        }

        });
    connect(&_settingsAction.getChartOptionsHolder().getRoundedBarsAction(), &ToggleAction::toggled, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        chartWidget->setUseRoundedBars(_settingsAction.getChartOptionsHolder().getRoundedBarsAction().isChecked());

        });
    connect(&_settingsAction.getChartOptionsHolder().getStackdirectionAction(), &OptionAction::currentIndexChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        QString dir = _settingsAction.getChartOptionsHolder().getStackdirectionAction().getCurrentText();
        if (dir == "vertical")
        {
            chartWidget->setStackingDirection(StackedBarChartWidget::Vertical);
        }
        else if (dir == "horizontal")
        {
            chartWidget->setStackingDirection(StackedBarChartWidget::Horizontal);
        }

        });
    connect(&_settingsAction.getChartOptionsHolder().getBarSpacingAction(), &IntegralAction::valueChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        int spacing = _settingsAction.getChartOptionsHolder().getBarSpacingAction().getValue();
        chartWidget->setBarSpacing(spacing);

        });
    connect(&_settingsAction.getChartOptionsHolder().getBarWidthAction(), &IntegralAction::valueChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        int width = _settingsAction.getChartOptionsHolder().getBarWidthAction().getValue();
        chartWidget->setBarWidth(width);

        });
    connect(&_settingsAction.getChartOptionsHolder().getBarBorderColorAction(), &ColorAction::colorChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        QColor color = _settingsAction.getChartOptionsHolder().getBarBorderColorAction().getColor();
        chartWidget->setBarBorderColor(color);

        });
    connect(&_settingsAction.getChartOptionsHolder().getBarBorderThicknessAction(), &IntegralAction::valueChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        int thickness = _settingsAction.getChartOptionsHolder().getBarBorderThicknessAction().getValue();
        chartWidget->setBarBorderThickness(thickness);

        });
    connect(&_settingsAction.getChartOptionsHolder().getBarCornerRadiusAction(), &IntegralAction::valueChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        int radius = _settingsAction.getChartOptionsHolder().getBarCornerRadiusAction().getValue();
        chartWidget->setBarCornerRadius(radius);

        });
    connect(&_settingsAction.getChartOptionsHolder().getBarOpacityAction(), &DecimalAction::valueChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        float opacity = _settingsAction.getChartOptionsHolder().getBarOpacityAction().getValue();
        chartWidget->setBarOpacity(opacity);

        });
    connect(&_settingsAction.getChartOptionsHolder().getShowValuesOnSegmentsAction(), &ToggleAction::toggled, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        chartWidget->setShowValuesOnSegments(_settingsAction.getChartOptionsHolder().getShowValuesOnSegmentsAction().isChecked());

        });
    connect(&_settingsAction.getChartOptionsHolder().getShowAxesAction(), &ToggleAction::toggled, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        chartWidget->setShowAxes(_settingsAction.getChartOptionsHolder().getShowAxesAction().isChecked());

        });
    connect(&_settingsAction.getChartOptionsHolder().getAxesFontsizeAction(), &IntegralAction::valueChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;
        });
    connect(&_settingsAction.getChartOptionsHolder().getShowGridAction(), &ToggleAction::toggled, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        chartWidget->setShowGrid(_settingsAction.getChartOptionsHolder().getShowGridAction().isChecked());

        });
    connect(&_settingsAction.getChartOptionsHolder().getGridColorAction(), &ColorAction::colorChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        QColor color = _settingsAction.getChartOptionsHolder().getGridColorAction().getColor();
        chartWidget->setGridColor(color);


        });
    connect(&_settingsAction.getChartOptionsHolder().getSortingAction(), &OptionAction::currentIndexChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        QString sort = _settingsAction.getChartOptionsHolder().getSortingAction().getCurrentText();

        if (sort == "total")
        {
            chartWidget->sortBars(StackedBarChartWidget::SortByTotal);
        }
        else if (sort == "label")
        {
            chartWidget->sortBars(StackedBarChartWidget::SortByLabel);
        }

        });
    connect(&_settingsAction.getChartOptionsHolder().getAnimationDurationAction(), &IntegralAction::valueChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;

        int duration = _settingsAction.getChartOptionsHolder().getAnimationDurationAction().getValue();
        chartWidget->setAnimationDuration(duration);
        });
    connect(&_settingsAction.getChartOptionsHolder().getHighlightColorAction(), &ColorAction::colorChanged, this, [this]() {
        auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
        if (!chartWidget)
            return;
        QColor color = _settingsAction.getChartOptionsHolder().getHighlightColorAction().getColor();
        chartWidget->setHighlightColor(color);
        });

    auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
    //if (chartWidget)
    //{
    //    //chartWidget->setAxisFont(QFont("Arial", 10));
    //    chartWidget->setHighlightColors(QVector<QColor>{}); // TODO: check if this is needed
    //}
}

void Computation::prepareChartData()
{
    if (_chartDataProcessing)
    {
        qDebug() << "_chartDataProcessing is true, so return";
        return;
    }

    if (!_points.isValid() || !_clusters.isValid())
    {
        qDebug() << "Invalid points or clusters dataset.";
        return;
    }

    _chartDataProcessing = true;

    QVector<Cluster> metadata = _clusters->getClusters();

    std::vector<float> inputVector;

    if (_points->getNumDimensions() == 1)
        _points->extractDataForDimension(inputVector, 0);// only intended for ATAC-seq scalars and RNA mapped data
    else if (_points->getNumDimensions() > 1 && _settingsAction.getComputationOptionsHolder().getDimensionPickerAction().getCurrentDimensionIndex() >= 0)
        _points->extractDataForDimension(inputVector, _settingsAction.getComputationOptionsHolder().getDimensionPickerAction().getCurrentDimensionIndex()); //populate Spatial data dimension
    else
    {
        qDebug() << "No valid dimension selected.";
        _chartDataProcessing = false;
        return;
    }

    const int numPoints = static_cast<int>(inputVector.size());

    int topCells = std::max(1, numPoints / 100); // FIXME: hard coded top 1% of the cells

    std::vector<int> indices(numPoints);
    std::iota(indices.begin(), indices.end(), 0); // [0..numPoints)

    // Partition so first 'topCells' indices are the top by value
    auto nth = indices.begin() + topCells;
    std::nth_element(indices.begin(), nth, indices.end(),
        [&](int a, int b) { return inputVector[a] > inputVector[b]; });

    indices.resize(topCells); // keep only the top ones

    QStringList   segmentLabels;
    QVector<QVector<double>> barData;
    QVector<QColor> barColors;

    std::tie(segmentLabels, barData, barColors) = computeMetadataCounts(metadata, indices, numPoints);

    auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
    if (!chartWidget)
        return;

    chartWidget->clearData();
    chartWidget->setData(barData, segmentLabels); // TODO: if double necessary?
    chartWidget->setColors(barColors);
    //chartWidget->setAxisLabels(QStringList{ "X Axis", "Y Axis" }); // TODO: to remove, not needed

    _chartDataProcessing = false;
}

std::tuple<QStringList, QVector<QVector<double>>, QVector<QColor>> Computation::computeMetadataCounts(const QVector<Cluster>& metadata, const std::vector<int>& topPoints, int numPoints)
{
    const int nClusters = metadata.size();

    std::vector<int> cellToCluster(numPoints, -1); // -1 means "no cluster"
    for (int c = 0; c < nClusters; ++c) {
        const auto& idx = metadata[c].getIndices();
        for (int cell : idx) {
            if (cell >= 0 && cell < numPoints) cellToCluster[cell] = c;
        }
    }

    // Count occurrences per cluster among top points
    QVector<int> counts(nClusters, 0);
    for (int cell : topPoints) {
        if (cell >= 0 && cell < numPoints) {
            int cluster = cellToCluster[cell];
            if (cluster >= 0) ++counts[cluster];
        }
    }

    // Collect only non-zero clusters and sort ascending by count
    std::vector<std::pair<int, int>> nonZeroClusters;  // (clusterIndex, count)
    nonZeroClusters.reserve(nClusters);
    for (int cluster = 0; cluster < nClusters; ++cluster) {
        if (counts[cluster] > 0) nonZeroClusters.emplace_back(cluster, counts[cluster]);
    }
    std::sort(nonZeroClusters.begin(), nonZeroClusters.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    QStringList labels;
    labels.reserve(static_cast<int>(nonZeroClusters.size()));

    QVector<QVector<double>> data(1);
    data[0].reserve(static_cast<int>(nonZeroClusters.size()));

    QVector<QColor> colors;
    colors.reserve(static_cast<int>(nonZeroClusters.size()));

    for (const auto& [clusterIndex, count] : nonZeroClusters) {
        labels << metadata[clusterIndex].getName();
        data[0].append(static_cast<double>(count));
        colors << metadata[clusterIndex].getColor();
    }

    return { labels, data, colors };
}

std::tuple<QStringList, QVector<QVector<double>>, QVector<QColor>> Computation::computeMetadataCounts(const QVector<Cluster>& metadata, const std::vector<int>& topPoints)
{
    // TODO: to remove, not used
    // adapted from DualViewPlugin
    QStringList labels;
    QVector<QVector<double>> data(1);  // one bar, multiple segments
    QVector<QColor> colors;

    std::unordered_map<int, int> cellToClusterMap; // maps global cell index to cluster index
    for (int clusterIndex = 0; clusterIndex < metadata.size(); ++clusterIndex) {
        const auto& ptIndices = metadata[clusterIndex].getIndices();
        for (int cellIndex : ptIndices) {
            cellToClusterMap[cellIndex] = clusterIndex;
        }
    }

    std::unordered_map<int, int> clusterCount; // maps cluster index to count
    for (int i = 0; i < topPoints.size(); ++i) {
        int index = topPoints[i];
        if (cellToClusterMap.find(index) != cellToClusterMap.end()) {
            int clusterIndex = cellToClusterMap[index];
            clusterCount[clusterIndex]++;
        }
    }

    std::vector<std::pair<int, int>> sortedClusterCount(clusterCount.begin(), clusterCount.end());
    std::sort(sortedClusterCount.begin(), sortedClusterCount.end(), [](const auto& a, const auto& b) { return a.second < b.second; }); // sort in ascending order, because painting starts from the bottom

    for (const auto& [clusterIndex, count] : sortedClusterCount)
    {
        QString clusterName = metadata[clusterIndex].getName();
        labels << clusterName;
        data[0].append(count);
        QColor color = metadata[clusterIndex].getColor();
        colors << color;
    }

    return { labels, data, colors };
}

void Computation::highlightTriggered()
{
    //TODO
}

void Computation::clickTriggered()
{
    //TODO
}

