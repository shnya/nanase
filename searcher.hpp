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

#include <vector>
#include <map>
#include <algorithm>
#include "utf8.hpp"
#include "indexdb.hpp"
#include "docinfo.hpp"

namespace nanase {
  class Searcher {

    IndexDB &idxdb;

    typedef int DocumentID;
    typedef size_t Position;
    typedef std::multimap<DocumentID, Position> IdxType;
    typedef std::pair<IdxType::const_iterator,
                      IdxType::const_iterator> IdxTypeRange;

  public:
    struct ResultType {
      int docid;
      double score;
      std::string url;
      std::string title;

      ResultType(int _docid, double _score,
                 const std::string &_url = "", const std::string &_title = "")
        : docid(_docid), score(_score), url(_url), title(_title) {}

      ~ResultType(){}
    };

    struct CompareResult {
      bool operator()(const ResultType &a, const ResultType &b) const throw() {
        return a.score > b.score;
      }
    };

  private:

    static void _CheckConnection(const IdxType &a, IdxType &b, int distance){
      IdxType::iterator itr = b.begin();
      while(itr != b.end()){
        IdxTypeRange found = a.equal_range(itr->first);
        bool connected = false;
        for(IdxType::const_iterator itr2 = found.first;
            itr2 != found.second; ++itr2){
          if(itr->second + distance == itr2->second){
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
    static IdxType& CheckConnection(std::vector<IdxType> &v, size_t char_num){
      for(size_t i = v.size() - 1; i > 0; i--){
        _CheckConnection(v[i], v[i-1], (char_num == 2 * i + 1) ? 1 : 2);
      }
      return v[0];
    }

    std::map<size_t, double> ExactMatch(const char* query,
                                        const char* ns = "") const {
      std::vector<IdxType> v;

      // This code is a bit complicated due to performance.
      // I will search with the query splitted into each two-letters,
      // but the last two-letter maybe overlap previous two-letter.
      // ex)
      // input abc => search {ab, bc}  // overlapped
      // input abcd => search {ab, cd} // not overlapped
      // input abcde => search {ab, cd, de} // overlapped
      std::vector<const char *> str_idx = utf8index(query);
      size_t i = 0;
      size_t char_num = str_idx.size();
      while(i < char_num){
        const char *sub = utf8substr(str_idx[i], 2);
        v.push_back(idxdb.read_index(sub, ns));
        delete[] sub;
        i += (i + 3 == char_num) ? 1 : 2;
      }

      std::map<size_t, double> results;

      if(v.size() == 0) return results;
      IdxType &cand = CheckConnection(v, char_num);
      for(IdxType::iterator itr = cand.begin(); itr != cand.end(); ++itr){
        results[itr->first] += 1.0;
      }

      return results;
    }

    void _Search(const char* query, std::vector<ResultType> &results) const {
      std::map<size_t, double> scores = ExactMatch(query, "");
      int max_document_num  = idxdb.get_current_docid();
      double idf = log(static_cast<double>(1 + max_document_num)
                       / static_cast<double>(scores.size()));

      for(std::map<size_t, double>::iterator itr = scores.begin();
          itr != scores.end(); ++itr){
        DocInfo docinfo(itr->first);
        if(!idxdb.read_docinfo(docinfo)) continue;
        results.push_back(ResultType(itr->first,
                                     idf * itr->second
                                     / static_cast<double>(docinfo.wordnum),
                                     docinfo.url,
                                     docinfo.title));
      }
    }

    Searcher();

  public:

    std::vector<ResultType>
    search(const char* query) const {
      std::vector<ResultType> results;
      _Search(query, results);
      std::sort(results.begin(), results.end(), CompareResult());
      return results;
    }

    Searcher(IndexDB &_idxdb)
      : idxdb(_idxdb) {
    }
  };
};

#endif /* SEARCHER_HPP */
