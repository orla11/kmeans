#ifndef CLUSTERING_H
#define CLUSTERING_H

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include "Document.h"
#include "DocumentCollection.h"

typedef std::string S;
typedef std::vector<Document> vecD;
typedef std::vector<int> vecI;
typedef std::map<Document,vecD> cMap;
typedef DocumentCollection dC;


class Clustering{ // define the abstract interface for the Kmeans Class
    protected:
        dC dc; // document collection attribute used by Clustering as sample to execute Kmeans algorithm
    public:
        Clustering(const S& dirname_):dc(dirname_){ }; // parametric constructor
        Clustering(){}; // default constructor
        Clustering(DocumentCollection& dc_){ // parametric constructor (overloaded)
            this->dc=dc_;
        }
        virtual ~Clustering(); // distructor
        virtual std::vector<vecI> cluster(const int& K_) = 0; // virtual cluster method, compute the clustering algorithm and returns std::vector<std::vector<int>>
};

Clustering::~Clustering(){};

#endif