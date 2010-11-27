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

#ifndef INDEXER_HPP
#define INDEXER_HPP

#include "utf8.hpp"
#include "tcmanager.hpp"
#include "docinfo.hpp"
#include <vector>
#include <cstring>

namespace nanase {
  class Indexer {
    TCManager &idxdb;

    void *
    Serialize(int docid, size_t pos) const throw() {
      size_t *str = new size_t[2];
      str[0] = docid;
      str[1] = pos;
      return str;
    }

    void *
    Serialize(std::vector<std::pair<size_t, size_t> > v) const throw() {
      size_t *str = new size_t[v.size() * 2];
      for(std::vector<std::pair<size_t, size_t> >::iterator itr = v.begin();
          itr != v.end(); ++itr){
        memcpy(str, &(itr->first), sizeof(size_t));
        memcpy(str + sizeof(size_t), &(itr->second), sizeof(size_t));
        str = str + sizeof(size_t) * 2;
      }
      return reinterpret_cast<void *>(str);
    }

    Indexer();
  public:
    void add(const char *url, const char *doc) const {
      int docid = idxdb.inc("seq", 3, 1);
      DocInfo docinfo(docid);
      docinfo.url = strdup(url);
      docinfo.urllen = strlen(url) + 1;
      docinfo.text = strdup(doc);
      docinfo.textlen = strlen(doc) + 1;

      const char *p = doc;
      size_t pos = 0;
      while(*p != '\0'){
        const char *sub = utf8substr(p, 2);
        idxdb.append(sub, strlen(sub),
                     Serialize(docid, pos), sizeof(size_t) * 2);
        delete[] sub;
        p = utf8nextchar(p);
        pos++;
      }

      docinfo.wordnum = pos;
      unsigned char *data;
      size_t data_size;
      docinfo.serialize(&data, &data_size);
      unsigned char *key_data = new unsigned char[sizeof(docid) + 2];
      key_data[0] = 0xFF; key_data[1] = 0xFF;
      memcpy(key_data + 2, &docid, sizeof(docid));
      idxdb.write(key_data, sizeof(docid) + 2, data, data_size);
      delete[] key_data;
      delete[] data;
    }

    Indexer(TCManager &_idxdb)
      : idxdb(_idxdb) {
    }
  };
}
#endif /* INDEXER_HPP */
