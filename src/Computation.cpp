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
using namespace mv;
using namespace mv::gui;


Computation::Computation(ATACControllerViewPlugin& ATACControllerViewPlugin, SettingsAction& SettingsAction)
    : WidgetAction(&ATACControllerViewPlugin, "Computation"),
    _viewerPlugin(ATACControllerViewPlugin),
    _settingsAction(SettingsAction)
{
    connect(&_points, &Dataset<Points>::dataChanged, this, [this]() {


        });

    connect(&_clusters, &Dataset<Clusters>::dataChanged, this, [this]() {


        });

    connect(&_settingsAction.getDataOptionsHolder().getPointDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, [this]() {

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
        if( _settingsAction.getDataOptionsHolder().getClusterDatasetAction().getCurrentDataset().isValid())
        {
            _clusters = _settingsAction.getDataOptionsHolder().getClusterDatasetAction().getCurrentDataset();

        }
        else
        {
            _clusters = Dataset<Clusters>();
        }
        });
       

    connect(&_settingsAction.getComputationOptionsHolder().getExportButtonAction(), &TriggerAction::triggered, this, [this]() {
        //prepareChartData();//added for testing remove and add in the method that calls it
        exportDataAsCSV();
        });
    connect(&_settingsAction.getComputationOptionsHolder().getDimensionPickerAction(), &DimensionPickerAction::currentDimensionIndexChanged, this, [this]() {


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
    if (chartWidget)
    {
        //chartWidget->setAxisFont(QFont("Arial", 10));
        chartWidget->setHighlightColors(QVector<QColor>{});
    }
    
}

void Computation::prepareChartData()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(5.0, 35.0);

    QVector<QVector<double>> data;
    int numBars = 1;
    int numSegments = 3;

    for (int bar = 0; bar < numBars; ++bar) {
        QVector<double> segments;
        for (int seg = 0; seg < numSegments; ++seg) {
            segments.append(dis(gen));
        }
        data.append(segments);
    }

    QStringList segmentLabels = { "Segment A", "Segment B", "Segment C" };
    QStringList barLabels = { "Bar 1" };
    QVector<QColor> colors = { QColor("#4e79a7"), QColor("#f28e2b"), QColor("#e15759") };

    auto* chartWidget = _viewerPlugin.getStackedBarChartWidget();
    if (!chartWidget)
        return;

    chartWidget->clearData();
    chartWidget->setData(data, segmentLabels);
    chartWidget->setColors(colors);
    chartWidget->setBarLabels(barLabels);
    chartWidget->setAxisLabels(QStringList{ "X Axis", "Y Axis" });
}

void Computation::highlightTriggered()
{
    //TODO
}

void Computation::clickTriggered()
{
    //TODO
}

void Computation::exportDataAsCSV()
{
    QString exportString = _settingsAction.getDataOptionsHolder().getExportDataAction().getVariant().toString();
    if (!exportString.isEmpty())
    {
        // store data in the exportString as a,b,c\nd,e,f\n g,h,i\n ...
        QString fileName = QFileDialog::getSaveFileName(nullptr, "Save CSV", "", "CSV files (*.csv);;All files (*.*)");
        if (!fileName.isEmpty())
        {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text))
            {
                QTextStream out(&file);
                QString csvString = exportString;
                csvString.replace("\\n", "\n"); 
                out << csvString;
                file.close();
                qDebug() << "Data exported to" << fileName;
            }
            else
            {
                qDebug() << "Could not open file for writing";
            }
        }
    }
    else
    {
        qDebug() << "No data to export";
    }
}
