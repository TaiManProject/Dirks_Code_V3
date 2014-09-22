#include "VideoDataTableModel.h"

void VideoDataTableModel::updateTable() {
    this->endResetModel();
}

void VideoDataTableModel::updateTable(int index) {
    //commonTool.log(QString("QFaceRecognitionResultTableModel::updateTable(int index)"));
    emit this->dataChanged(createIndex(index, 0), createIndex(index, 6));
}

ClusterVideoDataTableModel::ClusterVideoDataTableModel(QObject *parent) {
}

ClusterVideoDataTableModel::ClusterVideoDataTableModel(QObject *parent,
    ClusterVideoData* videoProcessingData) {
    this->d = videoProcessingData;
}

int ClusterVideoDataTableModel::rowCount(const QModelIndex & /*parent*/) const {
    return d->videoIDs.size();
}

int ClusterVideoDataTableModel::columnCount(const QModelIndex & /*parent*/) const {
    return 8;
}

QVariant ClusterVideoDataTableModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    if (role == Qt::DisplayRole){
        if (index.column() == 0){
            return QVariant(row);
        } else if (index.column() == 1) {
            int videoID = -1;
            if ( (int)d->videoIDs.size() > row) {
                videoID = d->videoIDs.at(row);
            }
            return QVariant(videoID);
        } else if (index.column() == 2) {
            int startFrameID = -1;
            if ((int)d->clusterLocations.size() > row) {
                Cluster* cluster = d->clusterLocations.at(row);
                startFrameID = cluster->startFrameID;
            }
            return QVariant(startFrameID);
        } else if (index.column() == 3) {
            int stopFrameID = -1;
            if ((int)d->clusterLocations.size() > row) {
                Cluster* cluster = d->clusterLocations.at(row);
                stopFrameID = cluster->stopFrameID;
            }
            return QVariant(stopFrameID);
        } else if (index.column() == 4) {
            int suspectID = -1;
            if ( (int)d->suspectIDs.size() > row) {
                suspectID = d->suspectIDs.at(row);
            }
            return QVariant(suspectID);
        } else if (index.column() == 5) {
            double landmarkDistance =-1;
            if ((int)d->clusterLocations.size() > row) {
                Cluster* cluster = d->clusterLocations.at(row);
                landmarkDistance = cluster->avgLandmarkDistances;
            }
            return landmarkDistance;
        } else if (index.column() == 6) {
            double photoDistance =-1;
            if ((int)d->clusterLocations.size() > row) {
                Cluster* cluster = d->clusterLocations.at(row);
                photoDistance = cluster->avgPhotoDistance;
            }
            return photoDistance;
        } else if (index.column() == 7) {
            double photoSimilarity =-1;
            if ((int)d->clusterLocations.size() > row) {
                Cluster* cluster = d->clusterLocations.at(row);
                photoSimilarity = cluster->avgPhotoSimilarity;
            }
            return photoSimilarity;
        }
    }
    return QVariant();
}

QVariant ClusterVideoDataTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QVariant("ID");
            case 1:
                return QVariant("Video");
            case 2:
                return QVariant("Start\nFrame");
            case 3:
                return QVariant("Stop\nFrame");
            case 4:
                return QVariant("Suspect");
            case 5:
                return QVariant("Landmark\ndistance");
            case 6:
                return QVariant("Photo\ndistance");
            case 7:
                return QVariant("Photo\nsimilarity");
            }
        }
    }
    else if(role == Qt::SizeHintRole) {
        if (orientation == Qt::Horizontal) {
            QSize size(100, 50);
            if (section == 0) {
                size.setWidth(60);
            } else if (section == 1) {
                size.setWidth(60);
            } else if (section == 2) {
                size.setWidth(60);
            } else if (section == 3) {
                size.setWidth(60);
            } else if (section == 4) {
                size.setWidth(60);
            } else if (section == 5) {
                size.setWidth(80);
            } else if (section == 6) {
                size.setWidth(80);
            } else if (section == 7) {
                size.setWidth(80);
            }

            return QVariant(size);
        }
    }
    return QVariant();
}

RawVideoDataTableModel::RawVideoDataTableModel(QObject *parent) {
}

RawVideoDataTableModel::RawVideoDataTableModel(QObject *parent,
    RawVideoData* videoProcessingData) {
    this->d = videoProcessingData;
}

int RawVideoDataTableModel::rowCount(const QModelIndex & /*parent*/) const {
    return d->suspectIDs.size();
}

int RawVideoDataTableModel::columnCount(const QModelIndex & /*parent*/) const {
    return 8;
}

QVariant RawVideoDataTableModel::data(const QModelIndex &index, int role) const {
    int row = index.row();
    if (role == Qt::DisplayRole){
        if (index.column() == 0){
            return QVariant(row);
        } else if (index.column() == 1) {
            int suspectID = -1;
            if ( (int)d->videoIDs.size() > row) {
                suspectID = d->videoIDs.at(row);
            }
            return QVariant(suspectID);
        } else if (index.column() == 2) {
            int videoID = -1;
            if ((int)d->frameIDs.size() > row) {
                videoID = d->frameIDs.at(row);
            }
            return QVariant(videoID);
        } else if (index.column() == 3) {
            int frameID = -1;
            if ( (int)d->faceIDs.size() > row) {
                frameID = d->faceIDs.at(row);
            }
            return QVariant(frameID);
        } else if (index.column() == 4) {
            int faceID = -1;
            if ( (int)d->suspectIDs.size() > row) {
                faceID = d->suspectIDs.at(row);
            }
            return QVariant(faceID);
        } else if (index.column() == 5) {
            double dist = -1;
            if ( (int)d->suspectIDs.size() > row) {
                dist = d->sumOfLandmarkDistances.at(d->videoIDs.at(row)).at(d->frameIDs.at(row)).at(d->faceIDs.at(row)).at(d->suspectIDs.at(row));
            }
            return QVariant(dist);
        } else if (index.column() == 6) {
            double distance = -1;
            if ((int)d->suspectIDs.size() > row) {
                distance = d->photoDistance.at(d->videoIDs.at(row)).at(d->frameIDs.at(row)).at(d->faceIDs.at(row)).at(d->suspectIDs.at(row));
            }
            return QVariant(distance);
        } else if (index.column() == 7) {
            double similarity = -1;
            if ((int)d->suspectIDs.size() > row) {
                similarity = d->photoSimilarity.at(d->videoIDs.at(row)).at(d->frameIDs.at(row)).at(d->faceIDs.at(row)).at(d->suspectIDs.at(row));
            }
            return QVariant(similarity);
        }
    }
    return QVariant();
}

QVariant RawVideoDataTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            switch (section) {
            case 0:
                return QVariant("ID");
            case 1:
                return QVariant("Video\nID");
            case 2:
                return QVariant("Frame\nID");
            case 3:
                return QVariant("Face\nID");
            case 4:
                return QVariant("Suspect\nID");
            case 5:
                return QVariant("Landmark\ndistance");
            case 6:
                return QVariant("Photo\ndistance");
            case 7:
                return QVariant("Photo\nsimilarity");
            }
        }
    }
    else if(role == Qt::SizeHintRole) {
        if (orientation == Qt::Horizontal) {
            QSize size(100, 50);
            if (section == 0) {
                size.setWidth(50);
            } else if (section == 1) {
                size.setWidth(50);
            } else if (section == 2) {
                size.setWidth(50);
            } else if (section == 3) {
                size.setWidth(50);
            } else if (section == 4) {
                size.setWidth(50);
            } else if (section == 5) {
                size.setWidth(90);
            } else if (section == 6) {
                size.setWidth(70);
            } else if (section == 7) {
                size.setWidth(70);
            }
            return QVariant(size);
        }
    }
    return QVariant();
}

