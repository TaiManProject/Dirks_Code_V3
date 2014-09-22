#ifndef CLUSTERVIDEODATA_H
#define CLUSTERVIDEODATA_H

#include "commonTool.h"

class VideoData {

};

class Cluster {
public:
    Cluster() {
        avgLandmarkDistances = 0.0;
        avgPhotoDistance = 0.0;
        avgPhotoSimilarity = 0.0;
    }
    std::vector<cv::Point2d> locationList; //just for reference
    int startFrameID; //inclusive
    int stopFrameID; //inclusive
    int suspectID;
    std::vector<double> sumOfLandmarkDistances; //just for reference
    std::vector<double> photoDistance; //just for reference
    std::vector<double> photoSimilarity; //just for reference
    //TODO: use max, not average ?
    double avgLandmarkDistances;
    double avgPhotoDistance;
    double avgPhotoSimilarity;
};

class ClusterVideoData : public VideoData {
public:
    std::vector <int> videoIDs;
    std::vector <int> clusterIDs;
    std::vector <int> suspectIDs;

    std::vector <Cluster*> clusterLocations;

    //for each video, for each suspect, a list of clusters
    std::vector<std::vector< std::vector <Cluster*> > > clusters;


    int getLocationOfCluster(Cluster* c) {
        //commonTool.log(QString("Looking for -> %1").arg(c->id));
        for (int i=0;i<(int)clusterLocations.size();i++) {
            //commonTool.log(QString("Found -> %1").arg(clusterPointers.at(i).id));
            if (c == clusterLocations.at(i)) return i;
        }
        //could not find
        return -1;
    }

    void closeAllOpenClusters(int videoID, int suspectID, int frameID) {
        std::vector<Cluster*> openClusters = getOpenClusters(videoID, suspectID);
        for (int i=0;i<(int)openClusters.size();i++) {
            Cluster* c = openClusters.at(i);
            c->stopFrameID = frameID;
        }
    }

    //remove clusters which are smaller than the threshold
    void removeSmallClusters(int videoID, int suspectID, int threshold) {
        for (int i=0;i<(int)clusters.at(videoID).at(suspectID).size();i++) {
            if ((int)clusters.at(videoID).at(suspectID).at(i)->locationList.size() < threshold) {
                int locationOfCluster = getLocationOfCluster(clusters.at(videoID).at(suspectID).at(i));
                commonTool.log(QString("at location --> %1").arg(locationOfCluster));
                videoIDs.erase(videoIDs.begin()+locationOfCluster);
                clusterIDs.erase(clusterIDs.begin()+locationOfCluster);
                suspectIDs.erase(suspectIDs.begin()+locationOfCluster);
                clusterLocations.erase(clusterLocations.begin()+locationOfCluster);
                Cluster* byeBye = clusters.at(videoID).at(suspectID).at(i);
                clusters.at(videoID).at(suspectID).erase(clusters.at(videoID).at(suspectID).begin()+i);
                delete byeBye;
                i--;
            }
        }
    }

    void joinNerbyClusters () {
        //TODO
    }

    void addCluster(int& videoID, int& suspectID, int& frameID, cv::Point2d& location) {
        Cluster* c = new Cluster();
        c->suspectID = suspectID;
        c->startFrameID = frameID;
        c->stopFrameID = -1;
        c->locationList.push_back(location);
        clusterIDs.push_back(clusters.at(videoID).at(suspectID).size());
        videoIDs.push_back(videoID);
        suspectIDs.push_back(suspectID);
        clusters.at(videoID).at(suspectID).push_back(c);
        clusterLocations.push_back(c);
    }

    void remove() {
        //TODO
    }

    std::vector<Cluster*> getOpenClusters(int videoID, int suspectID) {
        std::vector<Cluster*> allOpenClusters;
        for (int i=0;i<(int)clusters.at(videoID).at(suspectID).size();i++) {
            if (clusters.at(videoID).at(suspectID).at(i)->stopFrameID==-1) {
                allOpenClusters.push_back(clusters.at(videoID).at(suspectID).at(i));
            }
        }
        return allOpenClusters;
    }

    std::vector<Cluster*> getMarkedClusters(int videoID, int suspectID) {
        std::vector<Cluster*> allOpenClusters;
        for (int i=0;i<(int)clusters.at(videoID).at(suspectID).size();i++) {
            if (clusters.at(videoID).at(suspectID).at(i)->stopFrameID==-2) {
                allOpenClusters.push_back(clusters.at(videoID).at(suspectID).at(i));
            }
        }
        return allOpenClusters;
    }

    std::vector<Cluster*> getOpenClustersWithinBounds(int videoID, int suspectID, cv::Point2d p, double threshold) {
        std::vector<Cluster*> openClustersWithinBounds;
        std::vector<Cluster*> allOpenClusters = getOpenClusters(videoID, suspectID);
        for (int i=0;i<(int)allOpenClusters.size();i++) {
            Cluster* currentOpenCluster = allOpenClusters.at(i);
            int numberOfFacesInCurrentOpenCluster = currentOpenCluster->locationList.size();
            if (numberOfFacesInCurrentOpenCluster > 0) {
                //there is at least one face in there!
                cv::Point2d q = currentOpenCluster->locationList.back();
                double distance = sqrt((p.x-q.x)*(p.x-q.x) + (p.y-q.y)*(p.y-q.y));
                if (distance < threshold) {
                    openClustersWithinBounds.push_back(currentOpenCluster);
                }
            }
        }
        return openClustersWithinBounds;
    }
};



class RawVideoData : public VideoData {
public:
    //for each video for each frame a list of faces
    std::vector<std::vector< std::vector< cv::Mat > > > videoFaces;
    //for each video for reach frame a list of landmarks
    std::vector<std::vector< std::vector< std::vector< cv::Point2d > > > > videoLandmarkPointsList;
    std::vector <int> suspectIDs;
    std::vector <int> videoIDs;
    std::vector <int> frameIDs;
    std::vector <int> faceIDs;
    std::vector <std::vector <std::vector< std::vector<double> > > > sumOfLandmarkDistances;
    std::vector <std::vector <std::vector< std::vector<double> > > > photoDistance;
    //similarity is a normalized distance!
    std::vector <std::vector <std::vector< std::vector<double> > > > photoSimilarity;
};

#endif // CLUSTERVIDEODATA_H
