#include "StackedBarChartWidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QToolTip>
#include <QPropertyAnimation>
#include <QTimer>
#include <algorithm>
#include <numeric>

StackedBarChartWidget::StackedBarChartWidget(QWidget* parent)
    : QWidget(parent)
{
    setMouseTracking(true);
}

void StackedBarChartWidget::setUseRoundedBars(bool useRounded)
{
    m_useRoundedBars = useRounded;
    update();
}

void StackedBarChartWidget::setData(const QVector<QVector<float>>& data, const QStringList& segmentLabels)
{
    m_data = data;
    m_segmentLabels = segmentLabels;
    startAnimation();
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

void StackedBarChartWidget::setLegendPosition(LegendPosition pos)
{
    m_legendPosition = pos;
    update();
}

void StackedBarChartWidget::setLegendFont(const QFont& font)
{
    m_legendFont = font;
    update();
}

void StackedBarChartWidget::setLegendStyle(Qt::BrushStyle style)
{
    m_legendStyle = style;
    update();
}

void StackedBarChartWidget::setStackingDirection(StackingDirection dir)
{
    m_stackingDirection = dir;
    update();
}

void StackedBarChartWidget::setBarSpacing(int spacing)
{
    m_barSpacing = spacing;
    update();
}

void StackedBarChartWidget::setBarWidth(int width)
{
    m_barWidth = width;
    update();
}

void StackedBarChartWidget::setBarBorderColor(const QColor& color)
{
    m_barBorderColor = color;
    update();
}

void StackedBarChartWidget::setBarBorderThickness(int thickness)
{
    m_barBorderThickness = thickness;
    update();
}

void StackedBarChartWidget::setBarCornerRadius(int radius)
{
    m_barCornerRadius = radius;
    update();
}

void StackedBarChartWidget::setBarOpacity(qreal opacity)
{
    m_barOpacity = opacity;
    update();
}

void StackedBarChartWidget::setShowValuesOnSegments(bool show)
{
    m_showValuesOnSegments = show;
    update();
}

void StackedBarChartWidget::setShowAxes(bool show)
{
    m_showAxes = show;
    update();
}

void StackedBarChartWidget::setAxisLabels(const QStringList& labels)
{
    m_axisLabels = labels;
    update();
}

void StackedBarChartWidget::setAxisFont(const QFont& font)
{
    m_axisFont = font;
    update();
}

void StackedBarChartWidget::setShowGrid(bool show)
{
    m_showGrid = show;
    update();
}

void StackedBarChartWidget::setGridColor(const QColor& color)
{
    m_gridColor = color;
    update();
}

void StackedBarChartWidget::setHighlightColor(const QColor& color)
{
    m_highlightColor = color;
    update();
}

void StackedBarChartWidget::setHighlightColors(const QVector<QColor>& colors)
{
    if (m_segmentLabels.isEmpty())
        return;

    int segmentCount = m_segmentLabels.size();
    if (segmentCount == 0) {
        m_highlightColors = colors;
    }
    else if (colors.size() == 1) {
        m_highlightColors = QVector<QColor>(segmentCount, colors[0]);
    }
    else if (colors.size() < segmentCount && colors.size() > 0) {
        m_highlightColors.resize(segmentCount);
        for (int i = 0; i < segmentCount; ++i) {
            m_highlightColors[i] = i < colors.size() ? colors[i] : colors.last();
        }
    }
    else if (colors.size() > segmentCount) {
        m_highlightColors = colors.mid(0, segmentCount);
    }
    else {
        m_highlightColors.clear();
    }
    update();
}

void StackedBarChartWidget::setAnimationDuration(int duration)
{
    m_animationDuration = duration;
}

void StackedBarChartWidget::sortBars(SortType type)
{

    if (type == SortByTotal) {
        // TODO: remove sorting, sorting already done in Computation
        /*QVector<std::pair<float, int>> totals;
        for (int i = 0; i < m_data.size(); ++i) {
            float total = std::accumulate(m_data[i].begin(), m_data[i].end(), 0.0);
            totals.append({ total, i });
        }
        std::sort(totals.begin(), totals.end(), [](auto& a, auto& b) { return a.first > b.first; });
        QVector<QVector<float>> sortedData;
        QStringList sortedBarLabels;
        for (auto& pair : totals) {
            sortedData.append(m_data[pair.second]);
            if (pair.second < m_barLabels.size())
                sortedBarLabels.append(m_barLabels[pair.second]);
        }
        m_data = sortedData;
        m_barLabels = sortedBarLabels;*/
    }
    else if (type == SortByLabel) {
        QVector<std::pair<QString, int>> labels;
        for (int i = 0; i < m_barLabels.size(); ++i)
            labels.append({ m_barLabels[i], i });
        std::sort(labels.begin(), labels.end(), [](auto& a, auto& b) { return a.first < b.first; });
        QVector<QVector<float>> sortedData;
        QStringList sortedBarLabels;
        for (auto& pair : labels) {
            sortedData.append(m_data[pair.second]);
            sortedBarLabels.append(m_barLabels[pair.second]);
        }
        m_data = sortedData;
        m_barLabels = sortedBarLabels;
    }
    update();
}

void StackedBarChartWidget::clearData()
{
    m_data.clear();
    m_segmentLabels.clear();
    m_barLabels.clear();
    update();
}

void StackedBarChartWidget::startAnimation()
{
    m_animatedData = m_data;
    m_animationStep = 0;
    if (m_animationTimer) {
        m_animationTimer->stop();
        m_animationTimer->deleteLater();
    }
    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, &QTimer::timeout, this, &StackedBarChartWidget::animateStep);
    m_animationTimer->start(m_animationDuration / 10);
}

void StackedBarChartWidget::animateStep()
{
    ++m_animationStep;
    if (m_animationStep >= 10) {
        m_animationTimer->stop();
        m_animatedData = m_data;
        update();
        return;
    }
    for (int i = 0; i < m_data.size(); ++i) {
        for (int j = 0; j < m_data[i].size(); ++j) {
            float target = m_data[i][j];
            float current = m_animatedData[i][j];
            m_animatedData[i][j] = current + (target - current) * 0.1;
        }
    }
    update();
}

void StackedBarChartWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();
    int legendWidth = (m_legendPosition == LegendRight || m_legendPosition == LegendLeft) ? 120 : 0;
    int legendHeight = (m_legendPosition == LegendTop || m_legendPosition == LegendBottom) ? 60 : 0;

    // Always show legend
    m_showLegend = true;

    QRect chartRect = rect.adjusted(
        m_legendPosition == LegendLeft ? legendWidth + 40 : 40,
        m_legendPosition == LegendTop ? legendHeight + 10 : 10,
        m_legendPosition == LegendRight ? -legendWidth - 10 : -10,
        m_legendPosition == LegendBottom ? -legendHeight - 10 : -10
    );

    int barCount = m_data.size();
    if (barCount == 0) return;

    // Calculate barWidth and spacing to fill chartRect tightly
    int availableLength = m_stackingDirection == Vertical ? chartRect.width() : chartRect.height();
    int totalSpacing = (barCount > 1) ? (barCount - 1) * m_barSpacing : 0;
    int barWidth = m_barWidth > 0 ? m_barWidth : std::max(10, (availableLength - totalSpacing) / barCount);

    // Center bars if there is extra space
    int usedLength = barCount * barWidth + totalSpacing;
    int extraSpace = availableLength - usedLength;
    int leftOffset = extraSpace / 2;

    // Find max total value for scaling
    float maxTotal = 0.0;
    for (const auto& bar : m_data) {
        float total = std::accumulate(bar.begin(), bar.end(), 0.0);
        maxTotal = std::max(maxTotal, total);
    }
    if (maxTotal == 0.0) maxTotal = 1.0;

    // Draw grid and axes
    if (m_showGrid || m_showAxes) {
        painter.save();
        painter.setPen(m_gridColor);
        if (m_stackingDirection == Vertical) {
            int steps = 5;
            for (int i = 0; i <= steps; ++i) {
                int y = chartRect.bottom() - i * chartRect.height() / steps;
                if (m_showGrid)
                    painter.drawLine(chartRect.left(), y, chartRect.right(), y);
                if (m_showAxes) {
                    painter.setFont(m_axisFont);
                    painter.setPen(Qt::black);
                    painter.drawText(chartRect.left() - 40, y - 8, 35, 16, Qt::AlignRight | Qt::AlignVCenter,
                        //QString::number(maxTotal * i / steps, 'g', 3)); // would cause 55800 round to 8e+4
                        QString::number(maxTotal * i / steps, 'f', 0)); // used for cell count

                }
            }
            // Draw X axis ticks/labels for each bar
            if (m_showAxes) {
                painter.setFont(m_axisFont);
                for (int i = 0; i < barCount; ++i) {
                    int x = chartRect.left() + leftOffset + i * (barWidth + m_barSpacing);
                    painter.drawLine(x + barWidth / 2, chartRect.bottom(), x + barWidth / 2, chartRect.bottom() + 8);
                    painter.drawText(x, chartRect.bottom() + 10, barWidth, 15, Qt::AlignCenter,
                        m_barLabels.value(i, QString("Bar %1").arg(i + 1)));
                }
                painter.drawText(chartRect.left(), chartRect.bottom() + 30, chartRect.width(), 20, Qt::AlignCenter, m_axisLabels.value(0));
                painter.save();
                painter.translate(chartRect.left() - 60, chartRect.top());
                painter.rotate(-90);
                painter.drawText(0, 0, chartRect.height(), 20, Qt::AlignCenter, m_axisLabels.value(1));
                painter.restore();
            }
        }
        else {
            int steps = 5;
            for (int i = 0; i <= steps; ++i) {
                int x = chartRect.left() + i * chartRect.width() / steps;
                if (m_showGrid)
                    painter.drawLine(x, chartRect.top(), x, chartRect.bottom());
                if (m_showAxes) {
                    painter.setFont(m_axisFont);
                    painter.setPen(Qt::black);
                    painter.drawText(x - 20, chartRect.bottom() + 5, 40, 16, Qt::AlignCenter,
                        QString::number(maxTotal * i / steps, 'g', 3));
                }
            }
            // Draw Y axis ticks/labels for each bar
            if (m_showAxes) {
                painter.setFont(m_axisFont);
                for (int i = 0; i < barCount; ++i) {
                    int y = chartRect.top() + leftOffset + i * (barWidth + m_barSpacing);
                    painter.drawLine(chartRect.left() - 8, y + barWidth / 2, chartRect.left(), y + barWidth / 2);
                    painter.drawText(chartRect.left() - 80, y, 75, barWidth, Qt::AlignRight | Qt::AlignVCenter,
                        m_barLabels.value(i, QString("Bar %1").arg(i + 1)));
                }
                painter.drawText(chartRect.left(), chartRect.bottom() + 25, chartRect.width(), 20, Qt::AlignCenter, m_axisLabels.value(0));
                painter.save();
                painter.translate(chartRect.left() - 60, chartRect.top());
                painter.rotate(-90);
                painter.drawText(0, 0, chartRect.height(), 20, Qt::AlignCenter, m_axisLabels.value(1));
                painter.restore();
            }
        }
        painter.restore();
    }

    // Draw bars
    for (int i = 0; i < barCount; ++i) {
        int x = m_stackingDirection == Vertical ?
            chartRect.left() + leftOffset + i * (barWidth + m_barSpacing) :
            chartRect.left();
        int y = m_stackingDirection == Vertical ?
            chartRect.bottom() :
            chartRect.top() + leftOffset + i * (barWidth + m_barSpacing);

        float yOffset = 0.0;
        float xOffset = 0.0;
        for (int j = 0; j < m_data[i].size(); ++j) {
            float value = m_animationStep > 0 ? m_animatedData[i][j] : m_data[i][j];
            float height = m_stackingDirection == Vertical ?
                chartRect.height() * (value / maxTotal) :
                barWidth;
            float width = m_stackingDirection == Vertical ?
                barWidth :
                chartRect.width() * (value / maxTotal);

            QColor color = (j < m_colors.size()) ? m_colors[j] : Qt::gray;
            color.setAlphaF(m_barOpacity);

            QRectF barRect = m_stackingDirection == Vertical ?
                QRectF(x, y - yOffset - height, barWidth, height) :
                QRectF(x + xOffset, y, width, barWidth);

            // Highlight on legend hover/selection
            if (j == m_hoveredLegendSegment) {
                QColor highlight = m_highlightColors.isEmpty() ? m_highlightColor
                    : (j < m_highlightColors.size() ? m_highlightColors[j] : m_highlightColor);
                painter.setBrush(highlight.lighter(130));
            }
            else if (j == m_highlightedLegendSegment) {
                QColor highlight = m_highlightColors.isEmpty() ? m_highlightColor
                    : (j < m_highlightColors.size() ? m_highlightColors[j] : m_highlightColor);
                painter.setBrush(highlight);
            }
            // Highlight on selection
            else if (i == m_highlightedBar && j == m_highlightedSegment) {
                QColor highlight = m_highlightColors.isEmpty() ? m_highlightColor
                    : (j < m_highlightColors.size() ? m_highlightColors[j] : m_highlightColor);
                painter.setBrush(highlight);
            }
            // Highlight on hover (lighter)
            else if (i == m_hoveredBar && j == m_hoveredSegment) {
                QColor highlight = m_highlightColors.isEmpty() ? m_highlightColor
                    : (j < m_highlightColors.size() ? m_highlightColors[j] : m_highlightColor);
                painter.setBrush(highlight.lighter(130));
            }
            else {
                QBrush brush(color, m_legendStyle);
                painter.setBrush(brush);
            }
            painter.setPen(QPen(m_barBorderColor, m_barBorderThickness));
            if (m_useRoundedBars && m_barCornerRadius > 0)
                painter.drawRoundedRect(barRect, m_barCornerRadius, m_barCornerRadius);
            else
                painter.drawRect(barRect);

            // Show value on segment
            if (m_showValuesOnSegments) {
                painter.setPen(Qt::black);
                painter.setFont(QFont());
                QString valueStr = QString::number(value, 'g', 3);
                painter.drawText(barRect, Qt::AlignCenter, valueStr);
            }

            // Store rect for hit-testing
            if (i >= m_barRects.size()) m_barRects.resize(barCount);
            if (j >= m_barRects[i].size()) m_barRects[i].resize(m_data[i].size());
            m_barRects[i][j] = barRect;

            m_stackingDirection == Vertical ? yOffset += height : xOffset += width;
        }
    }

    // Draw legend
    drawLegend(painter, legendRect(rect, legendWidth, legendHeight));
}

QRect StackedBarChartWidget::legendRect(const QRect& rect, int legendWidth, int legendHeight) const
{
    switch (m_legendPosition) {
    case LegendRight:
        return QRect(rect.right() - legendWidth, rect.top() + 10, legendWidth - 10, rect.height() - 20);
    case LegendLeft:
        return QRect(rect.left() + 10, rect.top() + 10, legendWidth - 10, rect.height() - 20);
    case LegendTop:
        return QRect(rect.left() + 10, rect.top() + 10, rect.width() - 20, legendHeight - 10);
    case LegendBottom:
        return QRect(rect.left() + 10, rect.bottom() - legendHeight, rect.width() - 20, legendHeight - 10);
    }
    return QRect();
}

void StackedBarChartWidget::drawLegend(QPainter& painter, const QRect& rect)
{
    painter.save();
    painter.setFont(m_legendFont);
    painter.setPen(Qt::black);

    m_legendItemRects.clear();

    int itemHeight = 20;
    int y = rect.top();
    int x = rect.left();
    int count = m_segmentLabels.size();
    for (int i = 0; i < count; ++i) {
        QRect itemRect(x, y, 20, 15);
        m_legendItemRects.append(itemRect);

        QColor color = (i < m_colors.size()) ? m_colors[i] : Qt::gray;
        QBrush brush(color, m_legendStyle);

        // Highlight on selection
        if (i == m_highlightedLegendSegment) {
            QColor highlight = m_highlightColors.isEmpty() ? m_highlightColor
                : (i < m_highlightColors.size() ? m_highlightColors[i] : m_highlightColor);
            painter.setBrush(highlight);
        }
        // Highlight on hover
        else if (i == m_hoveredLegendSegment) {
            QColor highlight = m_highlightColors.isEmpty() ? m_highlightColor
                : (i < m_highlightColors.size() ? m_highlightColors[i] : m_highlightColor);
            painter.setBrush(highlight.lighter(130));
        }
        else {
            painter.setBrush(brush);
        }

        painter.drawRect(itemRect);

        QString label = m_segmentLabels.isEmpty() ? QString("Segment %1").arg(i + 1) : m_segmentLabels[i];
        painter.drawText(x + 25, y + 13, label);

        y += itemHeight;
    }
    painter.restore();
}

void StackedBarChartWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    update();
}

void StackedBarChartWidget::mouseMoveEvent(QMouseEvent* event)
{
    int oldBar = m_hoveredBar, oldSeg = m_hoveredSegment, oldLegend = m_hoveredLegendSegment;
    m_hoveredBar = m_hoveredSegment = m_hoveredLegendSegment = -1;

    // Check legend hover first
    for (int seg = 0; seg < m_legendItemRects.size(); ++seg) {
        if (m_legendItemRects[seg].contains(event->pos())) {
            m_hoveredLegendSegment = seg;
            update();
            return;
        }
    }

    // Check bar hover
    for (int i = 0; i < m_barRects.size(); ++i) {
        for (int j = 0; j < m_barRects[i].size(); ++j) {
            if (m_barRects[i][j].contains(event->pos())) {
                m_hoveredBar = i;
                m_hoveredSegment = j;
                m_hoveredLegendSegment = j;
                QString tooltip = QString("%1\nValue: %2")
                    .arg(m_segmentLabels.value(j, QString("Segment %1").arg(j + 1)))
                    .arg(m_data[i][j]);
                QToolTip::showText(event->globalPos(), tooltip, this);
                update();
                return;
            }
        }
    }

    if (oldBar != m_hoveredBar || oldSeg != m_hoveredSegment || oldLegend != m_hoveredLegendSegment)
        update();
}

void StackedBarChartWidget::mousePressEvent(QMouseEvent* event)
{
    // Check legend items first
    for (int seg = 0; seg < m_legendItemRects.size(); ++seg) {
        if (m_legendItemRects[seg].contains(event->pos())) {
            // Toggle highlight for legend
            if (m_highlightedLegendSegment == seg) {
                m_highlightedLegendSegment = -1;
                m_highlightedBar = -1;
                m_highlightedSegment = -1;
                emit barSegmentSelected(-1, -1);
            }
            else {
                m_highlightedLegendSegment = seg;
                m_highlightedBar = -1;
                m_highlightedSegment = seg;
                emit barSegmentClicked(-1, seg);
                emit barSegmentSelected(-1, seg);
            }
            update();
            return;
        }
    }

    // Check bars
    bool found = false;
    for (int i = 0; i < m_barRects.size(); ++i) {
        for (int j = 0; j < m_barRects[i].size(); ++j) {
            if (m_barRects[i][j].contains(event->pos())) {
                found = true;
                if (m_highlightedBar == i && m_highlightedSegment == j) {
                    m_highlightedBar = -1;
                    m_highlightedSegment = -1;
                    m_highlightedLegendSegment = -1;
                    emit barSegmentSelected(-1, -1);
                }
                else {
                    m_highlightedBar = i;
                    m_highlightedSegment = j;
                    m_highlightedLegendSegment = j;
                    emit barSegmentClicked(i, j);
                    emit barSegmentSelected(i, j);
                }
                update();
                return;
            }
        }
    }
    // Clicked outside any bar/legend: remove highlight
    if (!found) {
        m_highlightedBar = -1;
        m_highlightedSegment = -1;
        m_highlightedLegendSegment = -1;
        emit barSegmentSelected(-1, -1);
        update();
    }
}