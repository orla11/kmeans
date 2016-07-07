#ifndef KMEANS_H_
#define KMEANS_H_

#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<ctime>
#include "Document.h"
#include "DocumentCollection.h"
#include "Clustering.h"

typedef std::string S;
typedef std::vector<Document> vecD;
typedef std::vector<int> vecI;
typedef std::map<Document,vecD> cMap;
typedef std::map<S,float> bMap;

class Kmeans: public Clustering{
    private:
        // std::map<int,std::vector<int>> prevState attribute contains the previous status of the clusters 
        // for each iteration step, to check the convergence
        std::map<int,vecI> prevState;
        cMap centers; // 'centers' attribute, std::map<Document,std::vector<Document>> center as key and vector of associated Documents as value˜
        bool checkConvergence; // bool checkConvergence attribute used to break the while loop, when convergence is reached
        double elapsed_secs; // elapsed_secs attribute, used by Kmeans internal timer to evaluate elapsed time
        clock_t begin; // private begin attribute used by internal kmeans timer
        clock_t end; // private end attribute used by internal kmeans timer
    public:
        Kmeans(const S& dirname_):Clustering(dirname_){}; // parametric constructor (argument: const S& dirname_)
        Kmeans(DocumentCollection& dc_):Clustering(dc_){}; // parametric constructor overloaded (argument: DocumentCollection& dc_)
        virtual std::vector<vecI> cluster(const int& K_); // implementation of the inherited cluster method (kmeans algorithm)
        virtual const double getElaspedTime() const; // getter: returns the elapsed time attribute
    private:
        float cosSimilarity(Document& d1,Document& d2); // private method which computes the cosine similarity between two documents
        void computeCentroids(); // private method which computes new centroids and computes the similarity to reassing documents in the colleciton to their new centroid
        void startTimer(); // startTimer() start counting time when cluster method is executed
        void stopTimer(); // stopTimer() stop timer when cluster method is finished
};

void Kmeans::startTimer(){
    this->begin = clock();
}

void Kmeans::stopTimer(){
    this->end = clock();
    this->elapsed_secs = double(this->end-this->begin)/CLOCKS_PER_SEC;
    
    this->elapsed_secs = this->elapsed_secs * 1000;
}

const double Kmeans::getElaspedTime() const {
    return this->elapsed_secs;
}

float Kmeans::cosSimilarity(Document& d1,Document& d2){
    float sim = 0.0;

    if(d1.getBagOfWords().size()>=d2.getBagOfWords().size()){
        bMap::iterator iter = d1.getBagOfWords().begin();
        for(;iter!=d1.getBagOfWords().end();++iter){
            if(d2.getBagOfWords().find(iter->first)!=d2.getBagOfWords().end()){
                float auxF = (float)d2.getBagOfWords()[iter->first];
                sim += auxF * (float)iter->second;
            }
        }
    }
    
    if(d2.getBagOfWords().size()>d1.getBagOfWords().size()){
        bMap::iterator iter = d2.getBagOfWords().begin();
        for(;iter!=d2.getBagOfWords().end();++iter){
            if(d1.getBagOfWords().find(iter->first)!=d1.getBagOfWords().end()){
                float auxF = (float)d1.getBagOfWords()[iter->first];
                sim += auxF * (float)iter->second;
            }
        }
    }    
    
    return sim / (d1.getModule() * d2.getModule());
}

void Kmeans::computeCentroids(){
    
    cMap::iterator iter = this->centers.begin();
    
    bMap sum; //this bagOfWords contains all the words with their occururrence of all the documents related to a center. 
    std::map<Document,Document> swapper;
    
    for(;iter!=this->centers.end();++iter){
        int n = iter->second.size(); 
        
        if(n!=0){ //only if n!=0 because if n=0 we will divide for 0 at line 119
                // if n=0 the new centroid will be the current centroid 
        
            sum.erase(sum.begin(),sum.end()); 
            sum.insert(iter->second[0].getBagOfWords().begin(),iter->second[0].getBagOfWords().end()); //we insert the first document in the sum bMap
                
            vecD::iterator iterD = iter->second.begin() + 1; // skipping the first document, because it is already loaded
            for(;iterD!=iter->second.end();++iterD){ // iterating through documents and sum word occurences in sum bMap
                bMap::iterator iterS = iterD->getBagOfWords().begin();
                    
                for(;iterS!= iterD->getBagOfWords().end();++iterS){
                    if(sum.find(iterS->first) != sum.end()){ 
                        sum[iterS->first] += iterS->second;
                    }else{ 
                        sum.insert(std::make_pair(iterS->first,iterS->second)); //if there is a new word a new record in sum map i created
                        }
                }
            }
            
            // The bagOfWord attribute of the new centroid
            // is built by dividing each occurrence of each word
            // by n (number of documents associated with the current center)
            bMap::iterator iterB = sum.begin();
            for(;iterB!=sum.end();iterB++){
                iterB->second /=n;
            }
      
            // creating a new Document with the sum bMap: this is 
            // the new centroid
            Document centroid; 
            centroid.swap(sum); 
            swapper.insert(std::make_pair(iter->first,centroid)); //swapper maps the old centroids with the new ones
        }
            
    }
    
    vecD empVec;
    vecI elVecI;
    
    cMap::iterator iterCentersForPrev = this->centers.begin();
    for(;iterCentersForPrev!=this->centers.end();++iterCentersForPrev){
        vecD::iterator itr = iterCentersForPrev->second.begin();
        for(;itr!=iterCentersForPrev->second.end();++itr){
            elVecI.push_back(itr->getId());
        }
         //we fill prevState with centroidID->vector<DocumentIDs>    
        this->prevState.insert(std::make_pair(iterCentersForPrev->first.getId(),elVecI));
    }
    
    //we delete old centroid from centers and we insert the new ones associated with an empty vector of documents
    std::map<Document,Document>::iterator iterSwapper = swapper.begin();
    for(;iterSwapper!=swapper.end();++iterSwapper){
        cMap::iterator iterSearch = this->centers.find(iterSwapper->first); 
        this->centers.erase(iterSearch);
        this->centers.insert(std::make_pair(iterSwapper->second,empVec));
    }
    
    //we re-calculate the cosine similarity to re-fill the empty vectors of documents in centers
    vecD::iterator iterDC2 = this->dc.getCollection().begin();
    
    int center_id2=0;
    
    for(;iterDC2!= this->dc.getCollection().end(); iterDC2++){
        int dcid2 = iterDC2->getId();
            cMap::iterator iterC2 = this->centers.begin();
            float cosS2=0;

            for(;iterC2!=this->centers.end();iterC2++){
                Document unConstDoc2(iterC2->first);
                float thisCosS2 = cosSimilarity(*iterDC2,unConstDoc2);
                if(thisCosS2>cosS2){
                    cosS2=thisCosS2;
                    center_id2 = iterC2->first.getId();
                }
            } 
            
            cMap::iterator iterCC2 = this->centers.begin();
            for(;iterCC2!=this->centers.end(); iterCC2++){
                if(center_id2 == iterCC2->first.getId()){ 
                    iterCC2->second.push_back(*iterDC2);
                    break;
                }
            }
    }

    std::map<int,vecI> docsInd;
    
    // we create a map with centerID->vector<DocumentIDs>
    cMap::iterator iterCent = centers.begin();
    for(;iterCent!=centers.end();iterCent++){
        vecD::iterator iterCentV = iterCent->second.begin();
        vecI vectorI;
        for(;iterCentV!=iterCent->second.end();iterCentV++){
            vectorI.push_back(iterCentV->getId());
        }
        
        docsInd.insert(std::make_pair(iterCent->first.getId(),vectorI));
    }
    
    // we sort documets contained in every center in the just created map
    std::map<int,vecI>::iterator iterDocsInd = docsInd.begin();
    for(;iterDocsInd!=docsInd.end();++iterDocsInd){
        std::sort(iterDocsInd->second.begin(),iterDocsInd->second.begin());
    }
    
    // we sort documents contained in every center in prevState map
    std::map<int,vecI>::iterator iterPrevS = prevState.begin();
    for(;iterPrevS!=prevState.end();iterPrevS++){
        std::sort(iterPrevS->second.begin(),iterPrevS->second.end());
    }
        
    // now we check if the previous arrays of documents are equal to the current arrays
    // if they are all equal we break because convergence is reached
    this->checkConvergence=true;
    std::map<int,vecI>::iterator iterDI=docsInd.begin();
    iterPrevS=prevState.begin();
    
    while(iterDI!=docsInd.end() && iterPrevS!=prevState.end()){
        if(iterDI->second == iterPrevS->second){
            this->checkConvergence=false;
            break;
        }else{
            this->checkConvergence=true;
        }
        iterDI++;
        iterPrevS++;
    }
}


std::vector<vecI> Kmeans::cluster(const int& K_){
    // Kmeans Algorithm
    
    // 1 step) K Documents are chosen randomly from the collection:
        // - the old centers map is been erased
        // - the centers map is filled with new centers
    
    this->startTimer();    
    
    srand(time(0));
    const int k = K_;
        
    if(k>=this->dc.getSize()){
        std::cerr<<"Error: K cannot be greater or equal than documents number"<<std::endl;
        exit(1);
    }
    
    std::vector<int> indices;
    
    indices.reserve(k);
    int n = this->dc.getSize();
    
    
    while(indices.size()<k){ 
        int index = rand()%n;
        
        if(indices.empty()){
            indices.push_back(index);
        }else if(std::find(indices.begin(),indices.end(),index)==indices.end()){
           indices.push_back(index); 
        }
    } 
    
    this->centers.erase(this->centers.begin(),this->centers.end());
    
    vecD emptyVec;
    
    std::vector<int> checkCenters;
    
    for(int i=0;i<indices.size();i++){
        this->centers.insert(std::make_pair(this->dc[indices[i]],emptyVec));
        checkCenters.push_back(this->dc[indices[i]].getId());
    }
    
    // 2) Iterating the DocumentCollection
        // - the cosine similarity between every center and every document is calculated
        // - a document is assigned to the center with wich it has cosine similarity closer to 1 
            
    vecD::iterator iterDC = this->dc.getCollection().begin();
    
    // we associate documents to the centers using cosine similarity
    int center_id=0;
    
    for(;iterDC!= this->dc.getCollection().end(); iterDC++){
        int dcid = iterDC->getId();
        if(std::find(checkCenters.begin(),checkCenters.end(),dcid)==checkCenters.end()){ 
            cMap::iterator iterC = this->centers.begin();
            float cosS=0;

            for(;iterC!=this->centers.end();iterC++){
                Document unConstDoc(iterC->first);
                float thisCosS = cosSimilarity(*iterDC,unConstDoc);
                if(thisCosS>cosS){
                    cosS=thisCosS;
                    center_id = iterC->first.getId();
                }
            } 
            
            cMap::iterator iterCC = this->centers.begin();
            for(;iterCC!=this->centers.end(); iterCC++){
                if(center_id == iterCC->first.getId()){ 
                    iterCC->second.push_back(*iterDC);
                    break;
                }
            } 
        }
    }
        
    // 3) recalculating the centers with computeCentroid methood
        // checking if old centers are equal to the new ones
        // if they are equal -> break and return the result
    
    
    while(!this->checkConvergence){
        this->computeCentroids();
    }
    
    this->stopTimer();
        
    // generating the final vector that must be returned
    cMap::iterator iterNew2 = this->centers.begin();
    std::vector<vecI> final;
    for(;iterNew2!=this->centers.end();iterNew2++){
        vecD::iterator iterDocs= iterNew2->second.begin();
        vecI provv;
        for(;iterDocs!=iterNew2->second.end();iterDocs++){
            provv.push_back(iterDocs->getId());
        }
        final.push_back(provv);
        
    }
    
    return final;
}

#endif