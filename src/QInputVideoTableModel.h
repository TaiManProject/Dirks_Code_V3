#pragma once

#include <QAbstractTableModel>
#include <QProgressBar>
#include "commonTool.h"

 class QInputVideoTableModel : public QAbstractTableModel {
     Q_OBJECT
 public:
    QInputVideoTableModel(QObject *parent,
                          std::vector<QString>& videoFileNames,
                          std::vector<int>& middleColumnVideoIDs,
                          std::vector<int>& totalFramesList);
	QInputVideoTableModel(QObject *parent);

	int rowCount(const QModelIndex &parent = QModelIndex()) const ;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;	

	void updateTable();
    void updateTable(int index, std::vector<int> progressList);

 private:
    std::vector<QString>* videoFileNames;
    std::vector<int>* middleColumnVideoIDs;
    std::vector<int>* totalFramesList;

    std::vector<int> progressList;


 };


