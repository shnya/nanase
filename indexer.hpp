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
#include "indexdb.hpp"
#include "docinfo.hpp"
#include <vector>
#include <cstring>

namespace nanase {
  class Indexer {
    IndexDB &idxdb;

    Indexer();
  public:
    void add(const char *url, const char *text) const {
      int docid = idxdb.get_new_docid();
      DocInfo docinfo(docid, url, text);

      const char *p = text;
      size_t pos = 0;
      while(*p != '\0'){
        const char *sub = utf8substr(p, 2);
        idxdb.append_index(sub, docid, pos);
        delete[] sub;
        p = utf8nextchar(p);
        pos++;
      }

      docinfo.wordnum = pos;
      idxdb.write_docinfo(docinfo);
    }

    Indexer(IndexDB &_idxdb)
      : idxdb(_idxdb) {
    }
  };
}
#endif /* INDEXER_HPP */
