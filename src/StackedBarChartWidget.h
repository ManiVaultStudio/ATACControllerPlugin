#pragma once

#include <QWidget>
#include <QVector>
#include <QColor>
#include <QStringList>
#include <QFont>
#include <QRect>
#include <QRectF>
#include <QTimer>

class StackedBarChartWidget : public QWidget
{
    Q_OBJECT

public:
    enum LegendPosition { LegendRight, LegendLeft, LegendTop, LegendBottom };
    enum StackingDirection { Vertical, Horizontal };

    explicit StackedBarChartWidget(QWidget* parent = nullptr);

    // Data and appearance
    void setData(const QVector<QVector<float>>& data, const QStringList& segmentLabels = {});
    void setColors(const QVector<QColor>& colors);
    void setBarLabels(const QStringList& labels);
    void setShowLegend(bool show);
    void setLegendPosition(LegendPosition pos);
    void setLegendFont(const QFont& font);
    void setLegendStyle(Qt::BrushStyle style);
    void setUseRoundedBars(bool useRounded);
    void setStackingDirection(StackingDirection dir);
    void setBarSpacing(int spacing);
    void setBarWidth(int width);
    void setBarBorderColor(const QColor& color);
    void setBarBorderThickness(int thickness);
    void setBarCornerRadius(int radius);
    void setBarOpacity(qreal opacity);
    void setShowValuesOnSegments(bool show);
    void setShowAxes(bool show);
    void setAxisLabels(const QStringList& labels);
    void setAxisFont(const QFont& font);
    void setShowGrid(bool show);
    void setGridColor(const QColor& color);
    void setHighlightColor(const QColor& color);
    void setHighlightColors(const QVector<QColor>& colors);
    void setAnimationDuration(int duration);
    void clearData();

signals:
    void barSegmentClicked(int barIndex, int segmentIndex, QString barName, QString segmentName);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
private:
    void drawLegend(QPainter& painter, const QRect& rect, int scrollOffset, int visibleHeight, int totalHeight);
    QRect legendRect(const QRect& rect, int legendWidth, int legendHeight) const;
    void startAnimation();
    void animateStep();
    
    // Data
    QVector<QVector<float>> m_data;
    QVector<QVector<float>> m_animatedData;
    QStringList m_segmentLabels;
    QVector<QColor> m_colors;
    QStringList m_barLabels;
    QVector<QRect> m_legendItemRects;

    // Appearance
    bool m_showLegend = true;
    LegendPosition m_legendPosition = LegendRight;
    QFont m_legendFont;
    Qt::BrushStyle m_legendStyle = Qt::SolidPattern;
    bool m_useRoundedBars = true;
    StackingDirection m_stackingDirection = Vertical;
    int m_barSpacing = 10;
    int m_barWidth = 40;
    QColor m_barBorderColor = Qt::black;
    int m_barBorderThickness = 1;
    int m_barCornerRadius = 5;
    qreal m_barOpacity = 1.0;
    bool m_showValuesOnSegments = true;
    bool m_showAxes = true;
    QStringList m_axisLabels = { "X Axis", "Y Axis" };
    QFont m_axisFont;
    bool m_showGrid = true;
    QColor m_gridColor = Qt::lightGray;

    // Highlight
    QColor m_highlightColor = Qt::yellow;
    QVector<QColor> m_highlightColors;
    int m_highlightedBar = -1;
    int m_highlightedSegment = -1;
    int m_highlightedLegendSegment = -1;
    int m_hoveredBar = -1;
    int m_hoveredSegment = -1;
    int m_hoveredLegendSegment = -1;
    int m_legendScrollOffset = 0;

    // Animation
    int m_animationDuration = 400;
    int m_animationStep = 0;
    QTimer* m_animationTimer = nullptr;

    // Interaction
    QVector<QVector<QRectF>> m_barRects;
};