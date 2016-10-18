#include "IndexSearch.h"

IndexSearch::IndexSearch()
{

}


IndexSearch::~IndexSearch()
{
	delete reader;
}


void IndexSearch::open(char *indexPath)
{
    path = string(indexPath);
	hitCounts=0;
	reader=new IndexReader();
	reader->open((char *)path.c_str());
}


void  IndexSearch::doc(int64_t docId,Document & doc)
{
    reader->doc(docId, doc);
}


void  IndexSearch::SearchTerm(string term, vector<HitDoc> &hitVo)
{
    reader->getTermDocs(term, hitVo);
    this->hitCounts = hitVo.size();
}


void  IndexSearch::SearchAndString(string str,vector<HitDoc> &hitVo)
{
    //priority_queue<int64_t,vector<HitDoc>> Pqueue;
	PriQueue pQueue;

    m_tokeniser.tokenise(str);
    map <string, int> termMap = m_tokeniser.getTermMap();

    if(termMap.size()==1){
        string term(termMap.begin()->first);
		SearchTerm(term,hitVo);
		return;
	}
    for (map<string,int32_t>::iterator it=termMap.begin(); it
            !=termMap.end(); it++) {
        string term(it->first);
        vector<HitDoc> tempHits;
		reader->getTermDocs(term,tempHits);
		pQueue.put(tempHits);
	}
	pQueue.getResult(hitVo);
	this->hitCounts=hitVo.size();
}

void  IndexSearch::SearchOrString(string str,vector<HitDoc> &hitVo)
{
    m_tokeniser.tokenise(str);
    map <string, int> termMap = m_tokeniser.getTermMap();

    if(termMap.size()==1){
        string term(termMap.begin()->first);
		SearchTerm(term,hitVo);
		return;
	}

    vector<HitDoc> tempHits;
    map<int64_t,HitDoc> hitMap;
    for (map<string,int32_t>::iterator it=termMap.begin(); it
            !=termMap.end(); it++) {
        string term(it->first);
		reader->getTermDocs(term,tempHits);
        for(vector<HitDoc>::iterator vit=tempHits.begin();vit!=tempHits.end();vit++){
				HitDoc  hdoc=(* vit);
                map<int64_t,HitDoc>::iterator mapit=hitMap.find(hdoc.getDocId());
				if(mapit!=hitMap.end()){
					(mapit->second).setHits((mapit->second).getHits()+hdoc.getHits());
				}
				else{
					hitMap[hdoc.getDocId()]=hdoc;
				}
		}
		tempHits.clear();
	}
    map<int32_t,HitDoc,greater<int32_t> > tempMap;
    priority_queue<int64_t,vector<HitDoc> > Pqueue;
    for(map<int64_t,HitDoc>::iterator mit=hitMap.begin();mit!=hitMap.end();mit++){// Sequence
		Pqueue.push(mit->second);
	}
		 int size=Pqueue.size();
		for(int i=0;i<size;i++){
			HitDoc  hdocx=Pqueue.top();
			hitVo.push_back(hdocx);
			Pqueue.pop();
		}
	this->hitCounts=hitVo.size();
}
