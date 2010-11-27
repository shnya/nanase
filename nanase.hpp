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

#ifndef NANASE_HPP
#define NNAASE_HPP

#include "searcher.hpp"
#include "indexer.hpp"
#include "indexdb.hpp"

namespace nanase {
  class Nanase {
    IndexDB idxdb;

    Nanase(const Nanase&);
    Nanase& operator=(const Nanase&);

  public:

    Nanase(const std::string &db_path) : idxdb(db_path) {}

    void open(const std::string &db_path) {
      idxdb.open(db_path);
    }

    void close(){
      idxdb.close();
    }

    Searcher get_searcher(){
      return Searcher(idxdb);
    }

    Indexer get_indexer(){
      return Indexer(idxdb);
    }

  };
};
#endif /* NANASE_HPP */
