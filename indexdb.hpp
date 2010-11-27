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

    void append_index(const char *sub, int docid, size_t pos){
      using namespace serializer;
      Serializer sr(sizeof(int) + sizeof(size_t));
      sr << docid << pos;
      tcm.append(sub, strlen(sub), sr.data(), sr.size());
    }

    IdxType read_index(const char *sub){
      IdxType m;
      void *data;
      int n;
      tcm.read(sub, strlen(sub), &data, &n);
      if(data == NULL) return m;

      using namespace serializer;
      DeSerializer des(data, n);
      while(!des.eof()){
        int docid; size_t pos;
        des >> docid >> pos;
        m.insert(std::make_pair(docid, pos));
      }
      free(data);

      return m;
    }

    void write_docinfo(const DocInfo &docinfo){
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


    bool read_docinfo(DocInfo &docinfo){
      using namespace serializer;

      Serializer key(sizeof(unsigned char) * 2 + sizeof(int));
      unsigned char c = 0xFF;
      key << c << c << docinfo.docid;

      void *data;
      int data_size;
      tcm.read(key.data(), key.size(), &data, &data_size);
      if(data == NULL) return false;
      docinfo.deserialize(reinterpret_cast<unsigned char *>(data),
                          data_size, false);
      free(data);
      return true;
    }

    int get_new_docid(){
      return tcm.inc("seq", 3, 1);
    }

    int get_current_docid(){
      return tcm.inc("seq", 3, 0);
    }
  };
};

#endif /* INDEXDB_HPP */
