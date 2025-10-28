#include "StackedBarChartWidget.h"
#include <QPainter>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QToolTip>
#include <QPropertyAnimation>
#include <QTimer>
#include <algorithm>
#include <numeric>

// --- Helper functions (file-scope) ---
static bool isLightColor(const QColor& c)
{
    // Use relative luminance (sRGB) approximation.
    // Convert to linear-ish luminance using NTSC weights.
    double r = c.redF();
    double g = c.greenF();
    double b = c.blueF();
    // Perceived luminance
    double lum = 0.2126 * r + 0.7152 * g + 0.0722 * b;
    // threshold chosen so medium colors are considered dark -> use white text
    return lum > 0.6;
}

static QColor contrastColor(const QColor& underlying)
{
    // Return black for light underlying colors, white for dark ones.
    return isLightColor(underlying) ? Qt::black : Qt::white;
}

// ------------------------------------------------

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
        m_legendPosition == LegendLeft ? legendWidth + 25 : 25,
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
        float total = std::accumulate(bar.begin(), bar.end(), 0.0f);
        maxTotal = std::max(maxTotal, total);
    }
    if (maxTotal == 0.0) maxTotal = 1.0;

    // Determine widget background color (used for grid/axes/legend text contrast)
    QColor widgetBg = palette().color(backgroundRole());
    QColor gridContrast = contrastColor(widgetBg);

    // Draw grid and axes
    if (m_showGrid || m_showAxes) {
        painter.save();
        // Use grid color if explicit, but ensure it is black/white contrast for legibility.
        QColor gridPenColor = m_gridColor.isValid() ? m_gridColor : gridContrast;
        // If m_gridColor is set to something non-monochrome, convert it to black/white contrast to satisfy requirement:
        gridPenColor = contrastColor(widgetBg); // ensure black/white only
        painter.setPen(gridPenColor);
        if (m_stackingDirection == Vertical) {
            int steps = 5;
            for (int i = 0; i <= steps; ++i) {
                int y = chartRect.bottom() - i * chartRect.height() / steps;
                if (m_showGrid)
                    painter.drawLine(chartRect.left(), y, chartRect.right(), y);
                if (m_showAxes) {
                    painter.setFont(m_axisFont);
                    painter.setPen(contrastColor(widgetBg));
                    painter.drawText(chartRect.left() - 40, y - 8, 35, 16, Qt::AlignRight | Qt::AlignVCenter,
                        QString::number(maxTotal * i / steps, 'f', 0)); // used for cell count
                }
            }
            // Draw X axis ticks/labels for each bar
            if (m_showAxes) {
                painter.setFont(m_axisFont);
                painter.setPen(contrastColor(widgetBg));
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
                painter.setPen(contrastColor(widgetBg));
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
                    painter.setPen(contrastColor(widgetBg));
                    painter.drawText(x - 20, chartRect.bottom() + 5, 40, 16, Qt::AlignCenter,
                        QString::number(maxTotal * i / steps, 'g', 3));
                }
            }
            // Draw Y axis ticks/labels for each bar
            if (m_showAxes) {
                painter.setFont(m_axisFont);
                painter.setPen(contrastColor(widgetBg));
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
                painter.setPen(contrastColor(widgetBg));
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

        float yOffset = 0.0f;
        float xOffset = 0.0f;
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

            // Pick pen color for borders/text inside bar based on the bar's color:
            QColor insideContrast = contrastColor(color);

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

            // Pen for bar border: use contrast color relative to bar color (black/white)
            QPen borderPen(contrastColor(color), m_barBorderThickness);
            painter.setPen(borderPen);

            if (m_useRoundedBars && m_barCornerRadius > 0)
                painter.drawRoundedRect(barRect, m_barCornerRadius, m_barCornerRadius);
            else
                painter.drawRect(barRect);

            // Show value on segment (centered in the segment) - choose color that contrasts against the bar
            if (m_showValuesOnSegments) {
                painter.setPen(insideContrast);
                painter.setFont(QFont());
                //QString valueStr = QString::number(value, 'g', 3);
                QString valueStr = QString::number(value, 'f', 0); // integer format for cell counts
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

    QColor widgetBg = palette().color(backgroundRole());
    QColor legendTextColor = contrastColor(widgetBg);

    painter.setPen(legendTextColor);

    m_legendItemRects.clear();
    m_legendItemRects.resize(m_segmentLabels.size());

    int itemHeight = 20;
    int itemWidth = 0;
    QFontMetrics fm(m_legendFont);

    // Calculate max label width
    for (const auto& label : m_segmentLabels)
        itemWidth = std::max(itemWidth, fm.horizontalAdvance(label));

    int legendContentHeight = m_segmentLabels.size() * itemHeight;
    int legendContentWidth = 25 + itemWidth + 10;

    // Determine if scrollbars are needed
    m_legendVerticalScrollbarVisible = legendContentHeight > rect.height();
    m_legendHorizontalScrollbarVisible = legendContentWidth > rect.width();

    // Adjust drawing area for scrollbars and add padding for horizontal scrollbar
    int scrollbarThickness = 10;
    const int legendRightPadding = 2;
    const int legendBottomPadding = 10;
    QRect legendArea = rect;
    if (m_legendVerticalScrollbarVisible)
        legendArea.setWidth(legendArea.width() - scrollbarThickness - legendRightPadding);
    else
        legendArea.setWidth(legendArea.width() - legendRightPadding);

    if (m_legendHorizontalScrollbarVisible)
        legendArea.setHeight(legendArea.height() - scrollbarThickness - legendBottomPadding);
    else
        legendArea.setHeight(legendArea.height() - legendBottomPadding);

    // Clamp scroll offsets
    m_legendScrollOffsetY = std::clamp(m_legendScrollOffsetY, 0, std::max(0, legendContentHeight - legendArea.height()));
    m_legendScrollOffsetX = std::clamp(m_legendScrollOffsetX, 0, std::max(0, legendContentWidth - legendArea.width()));

    // Draw legend items (with scroll offset)
    int y = legendArea.top() - m_legendScrollOffsetY;
    int x = legendArea.left() - m_legendScrollOffsetX;
    int count = m_segmentLabels.size();

    for (int visualIndex = 0; visualIndex < count; ++visualIndex) {
        int segmentIndex = count - 1 - visualIndex;
        QRect itemRect(x, y, 20, 15);
        m_legendItemRects[segmentIndex] = itemRect.translated(m_legendScrollOffsetX, m_legendScrollOffsetY);

        QColor color = (segmentIndex < m_colors.size()) ? m_colors[segmentIndex] : Qt::gray;
        QBrush brush(color, m_legendStyle);

        QPen boxPen(contrastColor(color));
        painter.setPen(boxPen);

        if (segmentIndex == m_highlightedLegendSegment) {
            QColor highlight = m_highlightColors.isEmpty() ? m_highlightColor
                : (segmentIndex < m_highlightColors.size() ? m_highlightColors[segmentIndex] : m_highlightColor);
            painter.setBrush(highlight);
        }
        else if (segmentIndex == m_hoveredLegendSegment) {
            QColor highlight = m_highlightColors.isEmpty() ? m_highlightColor
                : (segmentIndex < m_highlightColors.size() ? m_highlightColors[segmentIndex] : m_highlightColor);
            painter.setBrush(highlight.lighter(130));
        }
        else {
            painter.setBrush(brush);
        }

        // Only draw if visible
        if (y + itemHeight > legendArea.top() && y < legendArea.bottom()) {
            painter.drawRect(itemRect);
            painter.setPen(legendTextColor);
            // Move text up by 1 pixel for better vertical alignment
            QRect textRect(x + 25, y - 1, legendArea.right() - (x + 25), 15);
            painter.drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                m_segmentLabels.isEmpty() ? QString("Segment %1").arg(segmentIndex + 1) : m_segmentLabels[segmentIndex]);
        }
        y += itemHeight;
    }

    // Draw vertical scrollbar if needed
    if (m_legendVerticalScrollbarVisible) {
        int barHeight = std::max(20, legendArea.height() * legendArea.height() / legendContentHeight);
        int barY = legendArea.top() + (legendArea.height() - barHeight) * m_legendScrollOffsetY / std::max(1, legendContentHeight - legendArea.height());
        m_legendVerticalScrollbarRect = QRect(legendArea.right() + legendRightPadding, barY, scrollbarThickness, barHeight);
        painter.setBrush(Qt::gray);
        painter.setPen(Qt::NoPen);
        painter.drawRect(legendArea.right() + legendRightPadding, legendArea.top(), scrollbarThickness, legendArea.height());
        painter.setBrush(Qt::darkGray);
        painter.drawRect(m_legendVerticalScrollbarRect);
    }

    // Draw horizontal scrollbar if needed
    if (m_legendHorizontalScrollbarVisible) {
        int barWidth = std::max(20, legendArea.width() * legendArea.width() / legendContentWidth);
        int barX = legendArea.left() + (legendArea.width() - barWidth) * m_legendScrollOffsetX / std::max(1, legendContentWidth - legendArea.width());
        m_legendHorizontalScrollbarRect = QRect(barX, legendArea.bottom() + legendBottomPadding, barWidth, scrollbarThickness);
        painter.setBrush(Qt::gray);
        painter.setPen(Qt::NoPen);
        painter.drawRect(legendArea.left(), legendArea.bottom() + legendBottomPadding, legendArea.width(), scrollbarThickness);
        painter.setBrush(Qt::darkGray);
        painter.drawRect(m_legendHorizontalScrollbarRect);
    }

    painter.restore();
}

void StackedBarChartWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    // Recalculate legend content and area
    int legendWidth = (m_legendPosition == LegendRight || m_legendPosition == LegendLeft) ? 120 : 0;
    int legendHeight = (m_legendPosition == LegendTop || m_legendPosition == LegendBottom) ? 60 : 0;
    QRect legendRect = this->legendRect(this->rect(), legendWidth, legendHeight);

    QFontMetrics fm(m_legendFont);
    int itemWidth = 0;
    for (const auto& label : m_segmentLabels)
        itemWidth = std::max(itemWidth, fm.horizontalAdvance(label));
    int legendContentWidth = 25 + itemWidth + 10;
    int legendContentHeight = m_segmentLabels.size() * 20;

    int scrollbarThickness = 12;
    QRect legendArea = legendRect;
    bool vScroll = legendContentHeight > legendRect.height();
    bool hScroll = legendContentWidth > legendRect.width();
    if (vScroll)
        legendArea.setWidth(legendArea.width() - scrollbarThickness);
    if (hScroll)
        legendArea.setHeight(legendArea.height() - scrollbarThickness);

    // Update scrollbars visibility
    m_legendVerticalScrollbarVisible = legendContentHeight > legendArea.height();
    m_legendHorizontalScrollbarVisible = legendContentWidth > legendArea.width();

    // Clamp or reset scroll offsets if not needed
    if (!m_legendHorizontalScrollbarVisible)
        m_legendScrollOffsetX = 0;
    else
        m_legendScrollOffsetX = std::clamp(m_legendScrollOffsetX, 0, std::max(0, legendContentWidth - legendArea.width()));

    if (!m_legendVerticalScrollbarVisible)
        m_legendScrollOffsetY = 0;
    else
        m_legendScrollOffsetY = std::clamp(m_legendScrollOffsetY, 0, std::max(0, legendContentHeight - legendArea.height()));

    update();
}

void StackedBarChartWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_data.isEmpty() || m_barRects.isEmpty())
        return;

    if (m_draggingLegendVScroll) {
        int legendContentHeight = m_segmentLabels.size() * 20;
        int legendAreaHeight = m_legendVerticalScrollbarRect.height() + m_legendVerticalScrollbarRect.top() - m_legendVerticalScrollbarRect.y();
        int deltaY = event->pos().y() - m_dragStartPosY;
        int maxOffset = std::max(0, legendContentHeight - legendAreaHeight);
        int scrollArea = height() - m_legendVerticalScrollbarRect.height();
        if (scrollArea > 0) {
            m_legendScrollOffsetY = std::clamp(m_scrollStartOffsetY + deltaY * maxOffset / scrollArea, 0, maxOffset);
            update();
        }
        return;
    }
    if (m_draggingLegendHScroll) {
        QFontMetrics fm(m_legendFont);
        int itemWidth = 0;
        for (const auto& label : m_segmentLabels)
            itemWidth = std::max(itemWidth, fm.horizontalAdvance(label));
        int legendContentWidth = 25 + itemWidth + 10;
        int legendAreaWidth = m_legendHorizontalScrollbarRect.width() + m_legendHorizontalScrollbarRect.left() - m_legendHorizontalScrollbarRect.x();
        int deltaX = event->pos().x() - m_dragStartPosX;
        int maxOffset = std::max(0, legendContentWidth - legendAreaWidth);
        int scrollArea = width() - m_legendHorizontalScrollbarRect.width();
        if (scrollArea > 0) {
            m_legendScrollOffsetX = std::clamp(m_scrollStartOffsetX + deltaX * maxOffset / scrollArea, 0, maxOffset);
            update();
        }
        return;
    }

    int oldBar = m_hoveredBar, oldSeg = m_hoveredSegment, oldLegend = m_hoveredLegendSegment;
    m_hoveredBar = m_hoveredSegment = m_hoveredLegendSegment = -1;

    // Adjust mouse position for legend scroll offsets
    QPoint legendTestPos = event->pos();
    legendTestPos.setX(legendTestPos.x() + m_legendScrollOffsetX);
    legendTestPos.setY(legendTestPos.y() + m_legendScrollOffsetY);

    // Check legend hover first
    for (int seg = 0; seg < m_legendItemRects.size(); ++seg) {
        if (m_legendItemRects[seg].contains(legendTestPos)) {
            m_hoveredLegendSegment = seg;
            update();
            return;
        }
    }

    // Check bar hover
    for (int i = 0; i < m_barRects.size(); ++i) {
        for (int j = 0; j < m_barRects[i].size(); ++j) {
            if (m_barRects[i][j].contains(event->pos())) {
                if (i < m_data.size() && j < m_data[i].size()) {
                    m_hoveredBar = i;
                    m_hoveredSegment = j;
                    m_hoveredLegendSegment = j;
                    QString tooltip = QString("%1\nValue: %2")
                        .arg(m_segmentLabels.value(j, QString("Segment %1").arg(j + 1)))
                        .arg(m_data[i][j]);
                    QToolTip::showText(event->globalPos(), tooltip, this);
                }
                update();
                return;
            }
        }
    }

    if (oldBar != m_hoveredBar || oldSeg != m_hoveredSegment || oldLegend != m_hoveredLegendSegment)
        update();
}

void StackedBarChartWidget::wheelEvent(QWheelEvent* event)
{
    if (m_legendVerticalScrollbarVisible) {
        int maxOffset = std::max(0, static_cast<int>(m_segmentLabels.size()) * 20 - (height() - (m_legendHorizontalScrollbarVisible ? 12 : 0)));
        m_legendScrollOffsetY = std::clamp(m_legendScrollOffsetY - event->angleDelta().y() / 2, 0, maxOffset);
        update();
        event->accept();
        return;
    }
    if (m_legendHorizontalScrollbarVisible) {
        // Calculate legendContentWidth as in drawLegend for accuracy
        QFontMetrics fm(m_legendFont);
        int itemWidth = 0;
        for (const auto& label : m_segmentLabels)
            itemWidth = std::max(itemWidth, fm.horizontalAdvance(label));
        int legendContentWidth = 25 + itemWidth + 10;
        int maxOffset = std::max(0, legendContentWidth - (width() - (m_legendVerticalScrollbarVisible ? 12 : 0)));
        m_legendScrollOffsetX = std::clamp(m_legendScrollOffsetX - event->angleDelta().x() / 2, 0, maxOffset);
        update();
        event->accept();
        return;
    }
    QWidget::wheelEvent(event);
}

void StackedBarChartWidget::setEnableClickSelect(bool enable)
{
    m_enableClickSelect = enable;
}

void StackedBarChartWidget::mouseReleaseEvent(QMouseEvent* event)
{
    m_draggingLegendVScroll = false;
    m_draggingLegendHScroll = false;
    QWidget::mouseReleaseEvent(event);
}

void StackedBarChartWidget::mousePressEvent(QMouseEvent* event)
{
    // Scrollbar drag start
    if (m_legendVerticalScrollbarVisible && m_legendVerticalScrollbarRect.contains(event->pos())) {
        m_draggingLegendVScroll = true;
        m_dragStartPosY = event->pos().y();
        m_scrollStartOffsetY = m_legendScrollOffsetY;
        return;
    }
    if (m_legendHorizontalScrollbarVisible && m_legendHorizontalScrollbarRect.contains(event->pos())) {
        m_draggingLegendHScroll = true;
        m_dragStartPosX = event->pos().x();
        m_scrollStartOffsetX = m_legendScrollOffsetX;
        return;
    }

    // Only allow selection if enabled
    if (!m_enableClickSelect)
        return;

    // Check legend items first
    for (int seg = 0; seg < m_legendItemRects.size(); ++seg) {
        if (m_legendItemRects[seg].contains(event->pos())) {
            // Toggle highlight for legend
            if (m_highlightedLegendSegment == seg) {
                m_highlightedLegendSegment = -1;
                m_highlightedBar = -1;
                m_highlightedSegment = -1;
            }
            else {
                m_highlightedLegendSegment = seg;
                m_highlightedBar = -1;
                m_highlightedSegment = seg;
                emit barSegmentClicked(-1, seg, QString(), m_segmentLabels.value(seg));
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
                QString barName = m_barLabels.value(i, QString("Bar %1").arg(i + 1));
                QString segmentName = m_segmentLabels.value(j, QString("Segment %1").arg(j + 1));
                if (m_highlightedBar == i && m_highlightedSegment == j) {
                    m_highlightedBar = -1;
                    m_highlightedSegment = -1;
                    m_highlightedLegendSegment = -1;
                }
                else {
                    m_highlightedBar = i;
                    m_highlightedSegment = j;
                    m_highlightedLegendSegment = j;
                    emit barSegmentClicked(i, j, barName, segmentName);
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
        update();
    }
}