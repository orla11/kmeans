#ifndef DOCUMENT_COLLECTION_H_
#define DOCUMENT_COLLECTION_H_

#include<vector>
#include<iostream>
#include<fstream>
#include<string>
#include<sstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Document.h"

typedef std::string S;
typedef std::vector<Document> vecD;
typedef std::vector<S> vecS;
typedef std::ifstream F;

class DocumentCollection{
    private:
        vecD collection; // 'collection' attribute is a vector of Documents
    public:
        DocumentCollection(const S& dirname); // parametric constructor
        DocumentCollection(); // default constructor
        ~DocumentCollection(); // distructor
        void addDocumentByName(const S& input_file_); // adds a document to the collection giving the filename (absolute path included)
        void addDocument(const Document& doc_); // adds a document to the collection passing a Document instance to the method
        int getSize(); // returns the size of the collection
        vecD& getCollection(); // return the DocumentCollection's 'collection' attribute as reference
        Document& operator[](const int& index){ // operator '[]' overloading provide access to a Document in the collection giving and index
                return this->collection[index];
        }
        void operator=(DocumentCollection& right){ // operator '=' overloading changes an instance's collection attribute with the right one 
            vecD vecptr = right.getCollection();
            vecD::iterator iter= vecptr.begin();
            this->collection.clear();
            for(; iter!= vecptr.end();iter++){
                this->collection.push_back(*iter);
            }
        }
};

DocumentCollection::DocumentCollection(const S& dirname){
    DIR *dir;
    struct dirent *ent;
    if((dir = opendir(dirname.c_str())) != NULL) {
        while((ent = readdir(dir)) != NULL) {
            std::stringstream ss;
            S s;
            ss << ent->d_name;
            ss >> s;
            if(s.find(".txt")!=S::npos){
                Document d(dirname+s);
                this->collection.push_back(d);
            }
        }
      closedir (dir);
    }else{
      /* could not open directory */
      perror("Error opening folder");
      exit(1);
    }
}

DocumentCollection::~DocumentCollection(){};
DocumentCollection::DocumentCollection(){};

void DocumentCollection::addDocumentByName(const S& input_file_){
    Document d(input_file_);
    this->collection.push_back(d);
}

void DocumentCollection::addDocument(const Document& doc_){
    this->collection.push_back(doc_);
}

int DocumentCollection::getSize(){
    return this->collection.size();
}

vecD& DocumentCollection::getCollection() {
    return this->collection;
}

#endif