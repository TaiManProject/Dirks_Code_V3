#include "QInputSuspectTableModel.h"


QInputSuspectTableModel::QInputSuspectTableModel(QObject *parent)
	:QAbstractTableModel(parent)
{

}

QInputSuspectTableModel::QInputSuspectTableModel(QObject *parent, std::vector<cv::Mat>& inputSuspectSketchList, std::vector<cv::Mat>& inputSuspectImageList)
	:QAbstractTableModel(parent)
{
    this->inputSuspectSketchList = &inputSuspectSketchList;
    this->inputSuspectImageList = &inputSuspectImageList;
}



int QInputSuspectTableModel::rowCount(const QModelIndex & /*parent*/) const
{
    if (inputSuspectSketchList->size() != inputSuspectImageList->size()){
		return -1;
	}
    return inputSuspectSketchList->size();
}

int QInputSuspectTableModel::columnCount(const QModelIndex & /*parent*/) const
{
	return 3;
}

QVariant QInputSuspectTableModel::data(const QModelIndex &index, int role) const
{
    //commonTool.log("QInputSuspectTableModel::data");
    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
			return QString::number(index.row());
		}
	}
	
	if (role == Qt::DecorationRole)
	{
         if (index.column() == 1) {
            QImage image = Mat2QImage(inputSuspectSketchList->at(index.row()));
            return QPixmap::fromImage(image);
        } else if (index.column() == 2) {
            QImage image = Mat2QImage(inputSuspectImageList->at(index.row()));
			QImageColor2Gray(image);
            return QPixmap::fromImage(image);
		}
	}
	return QVariant();
}

QVariant QInputSuspectTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role == Qt::DisplayRole)
	{
		if (orientation == Qt::Horizontal) {
			switch (section)
			{
			case 0:
                return QString("ID");
			case 1:
				return QString("Sketch");
			case 2:
				return QString("Photo");
			}

		}
    }
    else if(role == Qt::SizeHintRole) {
		if (orientation == Qt::Horizontal) {
			QSize size(100, 30);
				if (section == 0){
                    size.setWidth(30);
				} else if (section == 1){
					size.setWidth(200);
				} else if (section == 2){
					size.setWidth(200);
				}
 
				return QVariant(size);
		}
    }

	return QVariant();
}


void QInputSuspectTableModel::updateTable(){
    //this->reset();
    //Dirk edit:
    this->endResetModel();
    commonTool.log("QInputSuspectTableModel::updateTable()");
}

void QInputSuspectTableModel::updateTable(int index) {
commonTool.log("QInputSuspectTableModel::updateTable(int index)");
    emit this->dataChanged(createIndex(index, 0), createIndex(index, 1));
}
