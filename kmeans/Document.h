#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <algorithm>


typedef std::ifstream F;
typedef std::string S;
typedef std::map<S,float> bMap;


class Document{
    // Attributes
    private:
        static long int x; // static variable which works has an autoincremented key 
        // 'mutable' keyword remove const qualification from variable
        mutable long int id; // the document id attribute identifies each document uniquely
        // 'mutable' keyword remove const qualification from variable
        mutable bMap bagOfWords; // map which has a string as key and the occurrence of each key as value
        
    // Methods    
    private:
        S& sanitize(S& word_); // sanitize a string removing punctuation
        void setId(const int& id_); // changes the 'id' attribute of a Document
    public:
        Document(const S& input_file_); // parametric constructor
        Document(const Document& doc_); // copy constructor
        explicit Document(); // default constructor
        ~Document(); // distructor
        bool operator < (const Document right) const{ // operator '<' overloading provide .find() map's method between Documents
            if(this->getId()<right.getId()){
                return true;
            }else{
                return false;
            }
        }
        float operator[](S& word) const{ // operator '[]' overloading provide access to a value of the bagOfWords attribute of a Document given the key
            float ret = -1.0;
            if(this->bagOfWords.find(word)!=this->bagOfWords.end()){
                return this->bagOfWords[word];
            }else{
                return ret;
            }
        }
        bool operator==(const Document& right){ // operator '==' overloading compare two Documents
            if(this->getId()!=right.getId()){
                return false;
            }
            else if (this->bagOfWords!=right.bagOfWords){
                return false;
            }
            else{
                return true;
            }
        }
        void swap(bMap& bag_); // removes the old bagOfWords map of a Document and adds the new bag_ map
        inline static void getCurrX() { std::cout<<Document::x<<std::endl; }; // static method which provide the current status of the static attribute 'x' 
        inline bMap& getBagOfWords() const { return this->bagOfWords; }; // return the Document's 'bagOfWords' attribute as reference
        float getModule() const; // calculates the module of a Document by returning the square root of the sum of each squared occurrence 
        void addWord(const S& word_,const float occurrence_); // adds a key (word) and his value (occurence) to the map 
        inline long int getId() const { return this->id; } // returns the 'id' attribute of an istance as const long int
        inline const int getSize() const { return this->bagOfWords.size(); } // returns the size of the 'bagOfWords' attribute of an istance
        void printDocument() const; // writes the 'id' and the 'bagOfWords' attributes to the stdout
};


long int Document::x = 0; // initializing static x variable

Document::Document(){
    Document::x++;
    this->id = Document::x;
}

Document::~Document(){};

S& Document::sanitize(S& word_){
    std::transform(word_.begin(), word_.end(), word_.begin(), ::tolower);
    word_.erase(std::remove_if(word_.begin(),word_.end(),ispunct),word_.end()); 
    return word_;
}

Document::Document(const S& input_file_){
    F file;
    S word;
    
    try{
        //std::cout<<"Loading File content..."<<std::endl;
        
        file.open(input_file_.c_str());
        
        while(file >> word){
            S sanitizedWord = this->sanitize(word);
            if ((!sanitizedWord.empty())&&(this->bagOfWords.find(sanitizedWord)!=this->bagOfWords.end())){
                this->bagOfWords[sanitizedWord]+=1.0;
            }
            else{
                this->bagOfWords.insert(std::make_pair(sanitizedWord,1.0));
            }
        }
        
        Document::x++;
        this->id = Document::x;
        
        //std::cout<<"File loaded successfuly!"<<std::endl;
    }catch (F::failure e) {
        std::cerr<<"Exception occurred loading file"<<std::endl;
        exit(1);
    }
    
    file.close();

}

Document::Document(const Document& doc_){
    this->bagOfWords.erase(this->bagOfWords.begin(),this->bagOfWords.end()); // svuoto la mappa
    this->bagOfWords.insert(doc_.bagOfWords.begin(),doc_.bagOfWords.end());
        
    this->id = doc_.getId();
}

void Document::addWord(const S& word_,const float occurrence_){
    this->bagOfWords.insert(std::make_pair(word_,occurrence_));
}

float Document::getModule() const {
    float module=0.0;
    bMap::iterator iter =this->bagOfWords.begin();
    for(;iter!=this->bagOfWords.end();iter++){
        module+=pow(iter->second,2.0);
    }
    
    return sqrt(module);
}

void Document::swap(bMap& bag_){
    this->bagOfWords.erase(this->bagOfWords.begin(),this->bagOfWords.end()); // svuoto la mappa
    this->bagOfWords.insert(bag_.begin(),bag_.end());
}

void Document::setId(const int& id_){
    this->id = id_;
}

void Document::printDocument() const{
    std::cout<<"- ID: "<<this->getId()<<std::endl;
    bMap::iterator mIter = this->bagOfWords.begin();
    for(;mIter!=this->bagOfWords.end();mIter++){
        std::cout<<mIter->first<<" occurs: "<<mIter->second<<" | ";
    }
}

#endif