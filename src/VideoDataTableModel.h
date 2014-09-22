#ifndef CLUSTERVIDEODATATABLEMODEL_H
#define CLUSTERVIDEODATATABLEMODEL_H

#include <QAbstractTableModel>
#include "commonTool.h"
#include "VideoData.h"

class VideoDataTableModel :  public QAbstractTableModel {
public:
    void updateTable();
    void updateTable(int index);
};

class ClusterVideoDataTableModel : public VideoDataTableModel {
public:
    ClusterVideoDataTableModel(QObject *parent,
        ClusterVideoData* videoProcessingData);

    ClusterVideoDataTableModel(QObject *parent);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    ClusterVideoData* d;
};

class RawVideoDataTableModel : public VideoDataTableModel {
public:
    RawVideoDataTableModel(QObject *parent,
        RawVideoData* videoProcessingData);

    RawVideoDataTableModel(QObject *parent);

    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    RawVideoData* d;
};

#endif // CLUSTERVIDEODATATABLEMODEL_H
