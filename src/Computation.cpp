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

    connect(&_settingsAction.getComputationOptionsHolder().getPointDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, [this]() {

        if (_settingsAction.getComputationOptionsHolder().getPointDatasetAction().getCurrentDataset().isValid())
        {
            _settingsAction.getComputationOptionsHolder().getDimensionPickerAction().setPointsDataset(_settingsAction.getComputationOptionsHolder().getPointDatasetAction().getCurrentDataset());
            _points = _settingsAction.getComputationOptionsHolder().getPointDatasetAction().getCurrentDataset();
        }
        else
        {
            _settingsAction.getComputationOptionsHolder().getDimensionPickerAction().setPointsDataset(Dataset<Points>());
            _points = Dataset<Points>();
        }
        });

    connect(&_settingsAction.getComputationOptionsHolder().getClusterDatasetAction(), &DatasetPickerAction::currentIndexChanged, this, [this]() {
        if( _settingsAction.getComputationOptionsHolder().getClusterDatasetAction().getCurrentDataset().isValid())
        {
            _clusters = _settingsAction.getComputationOptionsHolder().getClusterDatasetAction().getCurrentDataset();

        }
        else
        {
            _clusters = Dataset<Clusters>();
        }
        });
       

    connect(&_settingsAction.getComputationOptionsHolder().getExportButtonAction(), &TriggerAction::triggered, this, [this]() {
        prepareChartData();//added for testing remove and add in the method that calls it
        exportDataAsCSV();
        });
    connect(&_settingsAction.getComputationOptionsHolder().getDimensionPickerAction(), &DimensionPickerAction::currentDimensionIndexChanged, this, [this]() {


        });


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

    chartWidget->setData(data, segmentLabels);
    chartWidget->setColors(colors);
    chartWidget->setBarLabels(barLabels);
    chartWidget->setBarWidth(5);
    chartWidget->setShowLegend(true);
}

void Computation::exportDataAsCSV()
{
    QString exportString = _settingsAction.getComputationOptionsHolder().getExportDataAction().getVariant().toString();
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
