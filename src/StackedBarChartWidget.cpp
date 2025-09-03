#include "StackedBarChartWidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <algorithm>

StackedBarChartWidget::StackedBarChartWidget(QWidget* parent)
    : QWidget(parent)
{
}

void StackedBarChartWidget::setData(const QVector<QVector<double>>& data, const QStringList& segmentLabels)
{
    m_data = data;
    m_segmentLabels = segmentLabels;
    update();
}

void StackedBarChartWidget::setColors(const QVector<QColor>& colors)
{
    m_colors = colors;
    update();
}

void StackedBarChartWidget::setBarLabels(const QStringList& labels)
{
    m_barLabels = labels;
    update();
}

void StackedBarChartWidget::setShowLegend(bool show)
{
    m_showLegend = show;
    update();
}

void StackedBarChartWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    QRect rect = this->rect();

    int legendWidth = m_showLegend ? 120 : 0;
    QRect chartRect = rect.adjusted(10, 10, -legendWidth - 10, -10);

    int barCount = m_data.size();
    if (barCount == 0) return;

    int barWidth;
    if (m_barWidth > 0) {
        barWidth = m_barWidth;
    }
    else {
        barWidth = std::max(10, static_cast<int>(chartRect.width() / barCount) - 10);
    }

    int maxSegments = 0;
    for (const auto& bar : m_data)
        maxSegments = std::max(maxSegments, static_cast<int>(bar.size()));

    // Find max total value for scaling
    double maxTotal = 0.0;
    for (const auto& bar : m_data) {
        double total = std::accumulate(bar.begin(), bar.end(), 0.0);
        maxTotal = std::max(maxTotal, total);
    }
    if (maxTotal == 0.0) maxTotal = 1.0;

    // Draw bars
    for (int i = 0; i < barCount; ++i) {
        int x = chartRect.left() + i * (barWidth + 10);
        int y = chartRect.bottom();
        double total = 0.0;
        for (double v : m_data[i]) total += v;

        double yOffset = 0.0;
        for (int j = 0; j < m_data[i].size(); ++j) {
            double value = m_data[i][j];
            double height = chartRect.height() * (value / maxTotal);

            QColor color = (j < m_colors.size()) ? m_colors[j] : Qt::gray;
            painter.setBrush(color);
            painter.setPen(Qt::black);
            painter.drawRect(x, y - yOffset - height, barWidth, height);

            yOffset += height;
        }

        // Draw bar label
        if (i < m_barLabels.size()) {
            painter.setPen(Qt::black);
            painter.drawText(x, chartRect.bottom() + 15, barWidth, 15, Qt::AlignCenter, m_barLabels[i]);
        }
    }

    // Draw legend
    if (m_showLegend)
        drawLegend(painter, QRect(rect.right() - legendWidth, rect.top() + 10, legendWidth - 10, rect.height() - 20));
}

void StackedBarChartWidget::drawLegend(QPainter& painter, const QRect& rect)
{
    painter.save();
    painter.setPen(Qt::black);
    QFontMetrics fm(painter.font());

    int itemHeight = 20;
    int y = rect.top();
    int count = m_segmentLabels.size();
    for (int i = 0; i < count; ++i) {
        QColor color = (i < m_colors.size()) ? m_colors[i] : Qt::gray;
        painter.setBrush(color);
        painter.drawRect(rect.left(), y, 20, 15);

        QString label = m_segmentLabels.isEmpty() ? QString("Segment %1").arg(i + 1) : m_segmentLabels[i];
        painter.drawText(rect.left() + 25, y + 13, label);

        y += itemHeight;
    }
    painter.restore();
}

void StackedBarChartWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}
void StackedBarChartWidget::setBarWidth(int width)
{
    m_barWidth = width;
    update();
}

void StackedBarChartWidget::clearData()
{
    m_data.clear();
    m_segmentLabels.clear();
    m_barLabels.clear();
    update();
}