#include "QInputVideoTableModel.h"


QInputVideoTableModel::QInputVideoTableModel(QObject *parent)
	:QAbstractTableModel(parent)
{

}

QInputVideoTableModel::QInputVideoTableModel(QObject *parent,
                                             std::vector<QString>& videoFileNames,
                                             std::vector<int>& middleColumnVideoIDs,
                                             std::vector<int>& totalFramesList)
	:QAbstractTableModel(parent)
{
    this->videoFileNames = &videoFileNames;
    this->middleColumnVideoIDs = &middleColumnVideoIDs;
    this->totalFramesList = &totalFramesList;
}

int QInputVideoTableModel::rowCount(const QModelIndex & /*parent*/) const
{
    return videoFileNames->size();
}

int QInputVideoTableModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 3;
}

QVariant QInputVideoTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            int videoID = -1;
            if ((int)middleColumnVideoIDs->size() > index.row()) {
                videoID = middleColumnVideoIDs->at(index.row());
            }
            return videoID;
        } else if (index.column() == 1) {
            QString name = "";
            if ((int)videoFileNames->size() > index.row()) {
                name = videoFileNames->at(index.row());
                QFileInfo fileInfo(name);
                name = fileInfo.fileName();
            }
            return name;
        } else if (index.column() == 2) {
            int progress = -1;
            int totalFrames = -1;
            if ((int)progressList.size() > index.row()) {
                progress = progressList.at(index.row());
            }
            if ((int)totalFramesList->size() > index.row()) {
                totalFrames = totalFramesList->at(index.row());
            }
            return QString::number(progress) + "/"+ QString("%1").arg(totalFrames);
		}
	}
	return QVariant();
}

QVariant QInputVideoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("ID");
            case 1:
                return QString("Name");
            case 2:
                return QString("Progress");
            }
        }
    }
    else if(role == Qt::SizeHintRole) {
        if (orientation == Qt::Horizontal) {
            QSize size(100, 30);
            if (section == 0){
                size.setWidth(30);
            } else if (section == 1){
                size.setWidth(150);
            } else if (section == 2){
                size.setWidth(80);
            }

            return QVariant(size);
        }
    }
    return QVariant();
}


void QInputVideoTableModel::updateTable(){
    //this->reset();
    //Dirk edit:
    this->endResetModel();
}

void QInputVideoTableModel::updateTable(int index, std::vector<int> progressList) {
    this->progressList = progressList;
	emit this->dataChanged(createIndex(index, 0), createIndex(index, 1));
}
