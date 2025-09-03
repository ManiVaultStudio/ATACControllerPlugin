#pragma once

#include <QWidget>
#include <QVector>
#include <QString>
#include <QColor>

class StackedBarChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StackedBarChartWidget(QWidget* parent = nullptr);

    // Data structure: each bar is a vector of values (one per segment)
    void setData(const QVector<QVector<double>>& data, const QStringList& segmentLabels = {});
    void setColors(const QVector<QColor>& colors);
    void setBarLabels(const QStringList& labels);
    void setShowLegend(bool show);
    void clearData();
    void setBarWidth(int width);
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;


private:
    QVector<QVector<double>> m_data;      // Each bar: segments
    QStringList m_segmentLabels;          // Labels for each segment
    QVector<QColor> m_colors;             // Colors for segments
    QStringList m_barLabels;              // Labels for each bar
    bool m_showLegend = true;
    int m_barWidth = -1;               // -1 for auto width
    void drawLegend(QPainter& painter, const QRect& rect);
};