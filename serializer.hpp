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
