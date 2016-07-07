#include <iostream>
#include "Kmeans.h"
#include "DocumentCollection.h"
#include "Clustering.h"

using namespace std;

typedef vector<int> vecI;

typedef std::vector<Document> vecD;

int main(){
    
    cout<<"------------------------------"<<endl;
    cout<<"| KMEANS DOCUMENT CLUSTERING  |"<<endl;
    cout<<"------------------------------"<<endl<<endl;
    
    cout<<"Initialization: Loading documents..."<<endl;
    cout<<"---------------------------------------"<<endl;
    
    DocumentCollection dc("./doc_lunghi/");
    
    if(dc.getCollection().size()>0){
        cout<<dc.getCollection().size()<<" documents loaded successfully"<<endl<<endl;
    }else{
        cerr<<"An error occurred: no documents available in collection"<<endl;
        exit(1);
    }
    
    int cln;
    int i = 1;
    
    cout<<"Insert the number of clusters you prefer: "; 
    cin>>cln;
    
    cout<<endl<<endl;
    cout<<"--------------------------"<<endl;
    cout<<"|   Start clustering...  |"<<endl;
    cout<<"--------------------------"<<endl;
    
    Clustering * k = new Kmeans(dc);
    
    vector<vecI> result;
    result = k->cluster(cln);
    
    vector<vecI>::iterator iterResult= result.begin();
    for(;iterResult!=result.end();iterResult++){
        vecI::iterator iterResultInt = iterResult->begin();
        
        cout<<"Cluster ("<<i<<") -> [";
        for(;iterResultInt!=iterResult->end();iterResultInt++){
             cout<<*iterResultInt;
             if(iterResultInt!=iterResult->end()-1){
                 cout<<"    ";
             }
        }
        cout<<"]"<<endl;
        i++;
    }
    
    cout<<endl;
    
    cout<<"Elapsed time: "<<k->getElaspedTime()<<" milliseconds"<<endl;

    return 0;
}