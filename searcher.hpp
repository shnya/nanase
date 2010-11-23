// Copyright (C) 2010 Masahiko Higashiyama
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef SEARCHER_HPP
#define SEARCHER_HPP

#include "utf8.hpp"
#include "tcmanager.hpp"
#include "docinfo.hpp"
#include <vector>
#include <map>
#include <algorithm>

class Searcher {

  TCManager docdb, idxdb;

  typedef std::vector<std::pair<size_t, size_t> > IdxType;
public:
  struct ResultType {
    size_t docid;
    double score;
    std::string url;
    std::string text;

    ResultType(size_t _docid, double _score, 
               const std::string &_url = "", const std::string &_text = "")
      : docid(_docid), score(_score), url(_url), text(_text) {}

    ~ResultType(){}
  };

private:

  std::vector<std::pair<size_t, size_t> >
  Deserialize(const void *data, size_t len) {
    std::vector<std::pair<size_t, size_t> > v;
    const size_t *val = reinterpret_cast<const size_t *>(data);
    for(size_t i = 0; i <= len; i++){
      size_t docid = *val++;
      size_t pos = *val++;
      v.push_back(std::make_pair(docid, pos));
      i += sizeof(size_t) * 2;
    }
    return v;
  }

  void find_sibling(const IdxType &a, const IdxType &b, IdxType &result){
    for(IdxType::const_iterator itr = a.begin(); itr != a.end(); ++itr){
      size_t current_docid = itr->first; size_t current_pos = itr->second;
      for(IdxType::const_iterator itr2 = b.begin(); itr2 != b.end(); ++itr2){
        size_t next_docid = itr2->first;  size_t next_pos = itr2->second;
        if(current_docid == next_docid && current_pos + 1 == next_pos){
          result.push_back(*itr2);
        }
      }
    }
  }

  IdxType exact_match(const std::vector<IdxType> &v){
    IdxType matched = v[0];
    for(size_t i = 1; i < v.size(); i++){
      IdxType cand;
      find_sibling(matched, v[i], cand);
      matched = cand;
    }
    return matched;
  }


  struct CompareResult {
    bool operator()(const ResultType &a, const ResultType &b){
      return a.score > b.score;
    }
  };

  std::map<size_t, double>
  _search(const char* query){
    std::vector<IdxType> v;

    const char *p = query;
    while(*p != '\0'){
      const char *sub = utf8substr(p, 2);
      if(*(utf8nextchar(sub)) == '\0'){
        delete[] sub;
        break;
      }
      void *data;
      int n;
      idxdb.read(sub, strlen(sub) + 1, &data, &n);
      delete[] sub;
      if(data == NULL) break;
      IdxType v2 = Deserialize(data, n);
      free(data);
      v.push_back(v2);
      p = utf8nextchar(p);
    }
    std::map<size_t, double> results; 

    if(v.size() == 0) return results;
    IdxType matched = exact_match(v);
    for(IdxType::iterator itr = matched.begin(); itr != matched.end(); ++itr){
      results[itr->first] += 10.0;
    }

    return results;
  }


  void get_doc_info(DocInfo &docinfo){
    void *data;
    int data_size;
    docdb.read(&docinfo.docid, sizeof(docinfo.docid), &data, &data_size);
    docinfo.deserialize(reinterpret_cast<unsigned char *>(data),
                        data_size, false);
    free(data);
  }

public:

  std::vector<ResultType>
  search(const char* query){
    std::vector<ResultType> results;

    std::map<size_t, double> scores = _search(query);
    for(std::map<size_t, double>::iterator itr = scores.begin();
        itr != scores.end(); ++itr){
      DocInfo docinfo(itr->first);
      get_doc_info(docinfo);
      results.push_back(ResultType(itr->first, itr->second, docinfo.url));
    }
    std::sort(results.begin(), results.end(), CompareResult());
    return results;
  }

  void open(){
    idxdb.open("idx.db");
    docdb.open("doc.db");
  }

  void close(){
    idxdb.close();
    docdb.close();
  }
};


#endif /* SEARCHER_HPP */
