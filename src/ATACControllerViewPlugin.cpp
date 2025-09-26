#include "ATACControllerViewPlugin.h"

#include <event/Event.h>

#include <DatasetsMimeData.h>

#include <QDebug>
#include <QMimeData>

Q_PLUGIN_METADATA(IID "studio.manivault.ATACControllerViewPlugin")

using namespace mv;

ATACControllerViewPlugin::ATACControllerViewPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _settingsAction(*this),
    _computation(*this, _settingsAction)
{


}

void ATACControllerViewPlugin::init()
{
    qApp->setStyleSheet("QToolTip { color: black; background: #ffffe1; border: 1px solid black; }");
    getWidget().setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    auto settings = new QHBoxLayout();
    settings->setContentsMargins(0, 0, 0, 0);
    settings->setSpacing(0);
    settings->addWidget(_settingsAction.getComputationOptionsHolder().createWidget(&getWidget()));
    settings->addWidget(_settingsAction.getDataOptionsHolder().createCollapsedWidget(&getWidget()));
    settings->addWidget(_settingsAction.getChartOptionsHolder().createCollapsedWidget(&getWidget()));
    layout->addLayout(settings);

    _stackedBarChartWidget = new StackedBarChartWidget(&getWidget());
    layout->addWidget(_stackedBarChartWidget, 1);
    getWidget().setLayout(layout);

    // Connect to bar segment click
    connect(_stackedBarChartWidget, &StackedBarChartWidget::barSegmentClicked,
        this, [this](int barIndex, int segmentIndex, QString barName, QString segmentName) {
            // Handle click event
            this->_computation.clickTriggered(segmentName);
        });

    //_stackedBarChartWidget->setShowLegend(true);
    //_stackedBarChartWidget->setLegendPosition(StackedBarChartWidget::LegendRight); // or LegendLeft, LegendTop, LegendBottom
    //_stackedBarChartWidget->setLegendFont(QFont("Arial", 10));
    //_stackedBarChartWidget->setLegendStyle(Qt::SolidPattern); // or other Qt::BrushStyle
    //_stackedBarChartWidget->setUseRoundedBars(true);
    //_stackedBarChartWidget->setStackingDirection(StackedBarChartWidget::Vertical); // or Horizontal
    //_stackedBarChartWidget->setBarSpacing(10);
    //_stackedBarChartWidget->setBarWidth(50);
    //_stackedBarChartWidget->setBarBorderColor(Qt::black);
    //_stackedBarChartWidget->setBarBorderThickness(2);
    //_stackedBarChartWidget->setBarCornerRadius(5);
    //_stackedBarChartWidget->setBarOpacity(0.8);
    //_stackedBarChartWidget->setShowValuesOnSegments(true);
    //_stackedBarChartWidget->setShowAxes(true);
    //_stackedBarChartWidget->setAxisFont(QFont("Arial", 10));
    //_stackedBarChartWidget->setShowGrid(true);
    //_stackedBarChartWidget->setGridColor(Qt::lightGray);
    //_stackedBarChartWidget->setHighlightColor(Qt::yellow);
    //_stackedBarChartWidget->setHighlightColors(QVector<QColor>{Qt::blue });
    //_stackedBarChartWidget->setAnimationDuration(400);
    //_stackedBarChartWidget->sortBars(StackedBarChartWidget::SortByTotal); // or SortByLabel
}

// =============================================================================
// Serialization
// =============================================================================

void ATACControllerViewPlugin::fromVariantMap(const QVariantMap& variantMap)
{
    ViewPlugin::fromVariantMap(variantMap);
    mv::util::variantMapMustContain(variantMap, "ATACControllerViewPlugin:Settings");
    _settingsAction.fromVariantMap(variantMap["ATACControllerViewPlugin:Settings"].toMap());
}

QVariantMap ATACControllerViewPlugin::toVariantMap() const
{
    QVariantMap variantMap = ViewPlugin::toVariantMap();
    _settingsAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

// =============================================================================
// Plugin Factory 
// =============================================================================

ATACControllerViewPluginFactory::ATACControllerViewPluginFactory()
{
    getPluginMetadata().setDescription("ATACController view plugin");
    getPluginMetadata().setSummary("This ATACController shows how to implement a basic view plugin in ManiVault Studio.");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setAuthors({
	});
    getPluginMetadata().setOrganizations({
        { "LUMC", "Leiden University Medical Center", "https://www.lumc.nl/en/" },
        { "TU Delft", "Delft university of technology", "https://www.tudelft.nl/" }
	});
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

ViewPlugin* ATACControllerViewPluginFactory::produce()
{
    return new ATACControllerViewPlugin(this);
}

mv::DataTypes ATACControllerViewPluginFactory::supportedDataTypes() const
{
    DataTypes supportedTypes;

    // This ATACController analysis plugin is compatible with points datasets
    supportedTypes.append(PointType);
    supportedTypes.append(ClusterType);
    return supportedTypes;
}

mv::gui::PluginTriggerActions ATACControllerViewPluginFactory::getPluginTriggerActions(const mv::Datasets& datasets) const
{
    PluginTriggerActions pluginTriggerActions;
/*
    const auto getPluginInstance = [this]() -> ATACControllerViewPlugin* {
        return dynamic_cast<ATACControllerViewPlugin*>(plugins().requestViewPlugin(getKind()));
    };

    const auto numberOfDatasets = datasets.count();

    if (numberOfDatasets >= 1 && PluginFactory::areAllDatasetsOfTheSameType(datasets, PointType)) {
        auto pluginTriggerAction = new PluginTriggerAction(const_cast<ATACControllerViewPluginFactory*>(this), this, "ATACController", "View ATACController data", icon(), [this, getPluginInstance, datasets](PluginTriggerAction& pluginTriggerAction) -> void {
            for (auto dataset : datasets)
                getPluginInstance();
        });

        pluginTriggerActions << pluginTriggerAction;
    }
    */
    return pluginTriggerActions;
}
