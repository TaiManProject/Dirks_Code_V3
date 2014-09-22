#pragma once

#include <QAbstractTableModel>
#include "commonTool.h"

 class QInputSuspectTableModel : public QAbstractTableModel
 {
     Q_OBJECT
 public:
    QInputSuspectTableModel(QObject *parent, std::vector<cv::Mat>& inputSuspectSketchList, std::vector<cv::Mat>& inputSuspectImageList);
	QInputSuspectTableModel(QObject *parent);

	int rowCount(const QModelIndex &parent = QModelIndex()) const ;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;	

	void updateTable();
	void updateTable(int index);

 private:
    std::vector<cv::Mat>* inputSuspectSketchList;
    std::vector<cv::Mat>* inputSuspectImageList;
 };


