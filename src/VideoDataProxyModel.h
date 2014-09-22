#ifndef CLUSTERVIDEODATAPROXYMODEL_H
#define CLUSTERVIDEODATAPROXYMODEL_H

#include <QAbstractTableModel>
#include "commonTool.h"

class VideoDataProxyModel : public QSortFilterProxyModel {

};

class ClusterVideoDataProxyModel :  public VideoDataProxyModel {
    Q_OBJECT
public:
    ClusterVideoDataProxyModel(double threshold, QObject *parent = 0);
    double getThreshold() { return threshold; }

public slots:
    void setThreshold(int sliderValue);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    double threshold;
};

class RawVideoDataProxyModel : public VideoDataProxyModel {
    Q_OBJECT
public:
    RawVideoDataProxyModel(double threshold, QObject *parent = 0);
    double getThreshold() { return threshold; }

public slots:
    void setThreshold(int sliderValue);

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

private:
    double threshold;
};

#endif // CLUSTERVIDEODATAPROXYMODEL_H
