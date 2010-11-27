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

namespace nanase {
  class Nanase {
    TCManager idxdb, docdb;

    Nanase(){};
    Nanase(const Nanase&);
    Nanase& operator=(const Nanase&);

  public:

    Nanase(std::string app_name, std::string dbdir = "./"){
      std::string idxdb_name(dbdir + app_name + ".idxdb");
      std::string docdb_name(dbdir + app_name + ".docdb");
      open(idxdb_name, docdb_name);
    }

    void open(std::string idxdb_name, std::string docdb_name){
      idxdb.open(idxdb_name.c_str());
      docdb.open(docdb_name.c_str());
    }

    void close(){
      idxdb.close();
      docdb.close();
    }

    Searcher get_searcher(){
      return Searcher(docdb, idxdb);
    }

    Indexer get_indexer(){
      return Indexer(docdb, idxdb);
    }

  };
};
#endif /* NANASE_HPP */
