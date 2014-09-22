#include "VideoDataProxyModel.h"

ClusterVideoDataProxyModel::ClusterVideoDataProxyModel(double threshold, QObject *parent) {
    this->threshold = threshold;
}

void ClusterVideoDataProxyModel::setThreshold(int sliderValue) {
    this->threshold = (double)sliderValue;
    invalidateFilter();
}

bool ClusterVideoDataProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex scoreColIndex = sourceModel()->index(sourceRow, 7, sourceParent);
    QVariant similarity = sourceModel()->data(scoreColIndex);
    double d = similarity.toDouble();
    if (d > this->threshold){
      return true;
    } else {
        return false;
    }
    return true;
}

RawVideoDataProxyModel::RawVideoDataProxyModel(double threshold, QObject *parent) {
    this->threshold = threshold;
}

void RawVideoDataProxyModel::setThreshold(int sliderValue) {
    this->threshold = (double)sliderValue;
    invalidateFilter();
}

bool RawVideoDataProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const {
    QModelIndex scoreColIndex = sourceModel()->index(sourceRow, 7, sourceParent);
    QVariant similarity = sourceModel()->data(scoreColIndex);
    double d = similarity.toDouble();
    if (d > this->threshold){
      return true;
    } else {
        return false;
    }
}
