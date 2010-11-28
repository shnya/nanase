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

#ifndef INDEXDB_HPP
#define INDEXDB_HPP

#include <string>
#include <cstring>
#include "debug.hpp"

#include <map>
#include "serializer.hpp"
#include "tcmanager.hpp"
#include "docinfo.hpp"


namespace nanase {
  class IndexDB {
    typedef int DocumentID;
    typedef size_t Position;
    typedef std::multimap<DocumentID, Position> IdxType;

    TCManager tcm;

    IndexDB(const IndexDB &);
    IndexDB& operator=(const IndexDB &);

  public:
    IndexDB(const std::string &db_path){
      open(db_path);
    }

    void open(const std::string &db_path){
      tcm.open(db_path.c_str());
    }

    void close(){
      tcm.close();
    }

    void append_index(const char *sub, int docid, size_t pos,
                      const char *ns = "") const {
      using namespace serializer;
      Serializer key(strlen(ns) + strlen(sub));
      Serializer value(sizeof(int) + sizeof(size_t));
      key << PtrCon(ns, strlen(ns)) << PtrCon(sub, strlen(sub));
      value << docid << pos;
      tcm.append(key.data(), key.size(), value.data(), value.size());
    }

    IdxType read_index(const char *sub, const char *ns = "") const {
      using namespace serializer;
      IdxType m;
      void *data;
      int n;
      Serializer key(strlen(ns) + strlen(sub));
      key << PtrCon(ns, strlen(ns)) << PtrCon(sub, strlen(sub));
      tcm.read(key.data(), key.size(), &data, &n);
      if(data == NULL) return m;

      DeSerializer des(data, n);
      while(!des.eof()){
        int docid; size_t pos;
        des >> docid >> pos;
        m.insert(std::make_pair(docid, pos));
      }
      free(data);

      return m;
    }

    void write_docinfo(const DocInfo &docinfo) const {
      using namespace serializer;

      Serializer key(sizeof(unsigned char) * 2 + sizeof(int));
      unsigned char c = 0xFF;
      key << c << c << docinfo.docid;

      unsigned char *data;
      size_t data_size;
      docinfo.serialize(&data, &data_size);

      tcm.write(key.data(), key.size(), data, data_size);

      delete[] data;
    }


    bool read_docinfo(DocInfo &docinfo) const {
      using namespace serializer;

      Serializer key(sizeof(unsigned char) * 2 + sizeof(int));
      unsigned char c = 0xFF;
      key << c << c << docinfo.docid;

      void *data;
      int data_size;
      tcm.read(key.data(), key.size(), &data, &data_size);
      if(data == NULL) return false;
      docinfo.deserialize(reinterpret_cast<unsigned char *>(data), data_size);
      free(data);
      return true;
    }

    int get_new_docid() const {
      return tcm.inc("seq", 3, 1);
    }

    int get_current_docid() const {
      return tcm.inc("seq", 3, 0);
    }
  };
};

#endif /* INDEXDB_HPP */
