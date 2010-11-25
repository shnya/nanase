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

  typedef size_t DocumentID;
  typedef size_t Position;
  typedef std::multimap<DocumentID, Position> IdxType;
  typedef std::pair<IdxType::const_iterator,
                    IdxType::const_iterator> IdxTypeRange;

private:

  IdxType
  Deserialize(const void *data, size_t len) {
    IdxType m;
    const size_t *val = reinterpret_cast<const size_t *>(data);
    for(size_t i = 0; i <= len; i++){
      size_t docid = *val++;
      size_t pos = *val++;
      m.insert(std::make_pair(docid, pos));
      i += sizeof(size_t) * 2;
    }
    return m;
  }

  void extract_connected(const IdxType &a, IdxType &b, int distance){
    IdxType::iterator itr = b.begin();
    while(itr != b.end()){
      IdxTypeRange found = a.equal_range(itr->first);
      bool connected = false;
      for(IdxType::const_iterator itr2 = found.first;
          itr2 != found.second; ++itr2){
        if(itr->second == itr2->second + distance){
          connected = true;
        }
      }
      if(connected)
        ++itr;
      else
        b.erase(itr++);
    }
  }

  // Argument vector will be destroyed.
  IdxType& exact_match(std::vector<IdxType> &v, size_t char_num){
    for(size_t i = 1; i < v.size(); i++){
      extract_connected(v[i-1], v[i], (char_num == 2 * i + 1) ? 1 : 2);
    }
    return v[v.size() - 1];
  }

  std::map<size_t, double>
  _search(const char* query){
    std::vector<IdxType> v;

    const char *p = query, *q;
    size_t char_num = 0;

    // This code a bit complicated due to performance.
    // I will search with the query splitted into each two-letters,
    // but the last two-letter maybe overlap previous two-letter.
    // ex)
    // input abc => search {ab, bc}  // overlapped
    // input abcd => search {ab, cd} // not overlapped
    // input abcde => search {ab, cd, de} // overlapped
    while(*p != '\0'){
      const char *sub = utf8substr(p, 2);
      // dealing with that the number of character is odd.
      if(*(utf8nextchar(sub)) == '\0'){
        delete[] sub;
        p = utf8nextchar(q);
        char_num -= 1;
        continue;
      }
      void *data;
      int n;
      idxdb.read(sub, strlen(sub) + 1, &data, &n);

      delete[] sub;
      if(data == NULL) break;

      IdxType v2 = Deserialize(data, n);
      free(data);

      v.push_back(v2);
      q = p;
      p = utf8nextchar(utf8nextchar(p));
      char_num += 2;
    }
    std::map<size_t, double> results;

    if(v.size() == 0) return results;
    IdxType &matched = exact_match(v, char_num);
    for(IdxType::iterator itr = matched.begin(); itr != matched.end(); ++itr){
      results[itr->first] += 1.0;
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

  struct CompareResult {
    bool operator()(const ResultType &a, const ResultType &b){
      return a.score > b.score;
    }
  };

  std::vector<ResultType>
  search(const char* query){
    std::vector<ResultType> results;

    std::map<size_t, double> scores = _search(query);
    int max_document_num  = docdb.inc("seq", 4, 0);
    double idf = log(static_cast<double>(1 + max_document_num)
                     / static_cast<double>(scores.size()));


    for(std::map<size_t, double>::iterator itr = scores.begin();
        itr != scores.end(); ++itr){
      DocInfo docinfo(itr->first);
      get_doc_info(docinfo);
      results.push_back(ResultType(itr->first,
                                   idf * itr->second
                                   / static_cast<double>(docinfo.wordnum),
                                   docinfo.url));
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
