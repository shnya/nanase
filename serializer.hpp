#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <stdexcept>
#include <cstring>

namespace nanase {
  namespace serializer {

    template <typename T>
    struct PtrContainer {
      T *ptr;
      unsigned int len;
    private:
      PtrContainer(){}
      PtrContainer &operator=(const PtrContainer &);
    public:
      PtrContainer(T *_ptr, unsigned int _len)
        : ptr(_ptr), len(_len) { }
    };

    template <typename T>
    PtrContainer<T> PtrCon(T *ptr, unsigned int len){
      return PtrContainer<T>(ptr, len);
    }

    class Serializer {
      unsigned int maxlen;
      unsigned int curlen;
      unsigned char* data_ptr;

      Serializer(){}
      Serializer(const Serializer &);
      Serializer &operator=(const Serializer &);

      template <typename T>
      Serializer& PtrCopy(const PtrContainer<T> &obj) throw(std::length_error) {
        if(maxlen < curlen + sizeof(T) * obj.len) throw std::length_error("error");
        memcpy(data_ptr + curlen, obj.ptr, sizeof(T) * obj.len);
        curlen += sizeof(T) * obj.len;
        return *this;
      }

    public:

      template <typename T>
      Serializer& operator <<(T obj) {
        if(maxlen < curlen + sizeof(T)) throw std::length_error("error");
        memcpy(data_ptr + curlen, &obj, sizeof(obj));
        curlen += sizeof(T);
        return *this;
      }

      template <typename T>
      Serializer& operator <<(const PtrContainer<T> &obj) throw(std::length_error) {
        return PtrCopy(obj);
      }

      template <typename T>
      Serializer& operator <<(PtrContainer<T> &obj) throw(std::length_error) {
        return PtrCopy(obj);
      }

      Serializer(unsigned int _maxlen)
        : maxlen(_maxlen), curlen(0), data_ptr(new unsigned char[_maxlen]) {}

      ~Serializer(){ delete[] data_ptr; }

      void *data(){ return reinterpret_cast<void *>(data_ptr); }
      unsigned int size(){ return maxlen; }
    };


    class DeSerializer {
      unsigned int maxlen;
      unsigned int curlen;
      unsigned char* data_ptr;

      DeSerializer(){}
      DeSerializer(const Serializer &);
      DeSerializer &operator=(const Serializer &);

      template <typename T>
      DeSerializer& PtrCopy(const PtrContainer<T> &obj) throw(std::length_error) {
        if(maxlen < curlen + sizeof(T) * obj.len) throw std::length_error("error");
        memcpy(obj.ptr, data_ptr + curlen, sizeof(T) * obj.len);
        curlen += sizeof(T) * obj.len;
        return *this;
      }

    public:

      template <typename T>
      DeSerializer& operator >>(T &obj) throw(std::length_error) {
        if(maxlen < curlen + sizeof(T)) throw std::length_error("error");
        memcpy(&obj, data_ptr + curlen, sizeof(obj));
        curlen += sizeof(T);
        return *this;
      }

      template <typename T>
      DeSerializer& operator>>(const PtrContainer<T> &obj) throw(std::length_error){
        return PtrCopy(obj);
      }
      template <typename T>
      DeSerializer& operator >>(PtrContainer<T> &obj) throw(std::length_error) {
        return PtrCopy(obj);
      }

      DeSerializer(void *_data, unsigned int _maxlen)
        : maxlen(_maxlen), curlen(0),
          data_ptr(reinterpret_cast<unsigned char*>(_data)) {}

      size_t eof() {
        return maxlen == curlen;
      }

    };
  }
};

#ifdef DEBUG
#include <cstdio>
namespace nanase {
  namespace serializer {
    void PrintBinary(const void *str, size_t size){
      for(size_t i = 0; i < size; i++){
        printf("%02x", *(reinterpret_cast<const unsigned char *>(str) + i));
      }
      printf("\n");
    }
  }
}
#endif /* DEBUG */


#endif /* SERIALIZER_HPP */
