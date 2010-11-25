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

#ifndef TCMANAGER_HPP
#define TCMANAGER_HPP

#include <tchdb.h>
#include <cassert>
#include <exception>
#include <string>


#define TCMANAGER_ERROR_CHECK(pred) do{             \
    if((pred)){                                     \
      int ecode = tchdbecode(hdb);                  \
      throw TCManagerException(tchdberrmsg(ecode)); \
    }                                               \
  } while(0)

// This class is simple wrapper class for tokyo cabinet.
class TCManager {
  TCHDB *hdb;

  void CheckInitialized() const throw() {
    assert(hdb != NULL);
    // I handle only debug time checking.
    // If you want to check dynamically, please uncomment below.
    // if(hdb == NULL)
    // throw std::logic_error("tchdb has not yet initialized.");
  }

public:
  class TCManagerException : std::exception {
    std::string error;
  public:
    TCManagerException(const char *err) throw() : error(err) {}
    const char *what() const throw() { return error.c_str(); }
    ~TCManagerException() throw() {}
  };

  TCManager(void) : hdb(NULL) {}
  TCManager(const char *fname) throw (TCManagerException) {
    open(fname);
  }

  // Because closing DB may cause exception,
  // you must close db explicitly.
  ~TCManager() throw() { assert(hdb == NULL); }

  void read(const void *key, int ksiz, void **val, int *vsiz)
    const throw (TCManagerException) {
    CheckInitialized();
    *val = tchdbget(hdb, key, ksiz, vsiz);
    TCMANAGER_ERROR_CHECK(tchdbecode(hdb) != TCESUCCESS
                          && tchdbecode(hdb) != TCENOREC);
  }

  void append(const void *key, int ksiz, const void *val, int vsiz)
    const throw (TCManagerException) {
    CheckInitialized();
    TCMANAGER_ERROR_CHECK(!tchdbputcat(hdb, key, ksiz, val, vsiz));
  }

  int inc(const void *key, int ksiz, int increment)
    const throw(TCManagerException) {
    int ret;
    CheckInitialized();
    TCMANAGER_ERROR_CHECK((ret = tchdbaddint(hdb, key, ksiz, increment))
                          == INT_MIN);
    return ret;
  }

  void write(const void *key, int ksiz, const void *val, int vsiz)
    const throw (TCManagerException) {
    CheckInitialized();
    TCMANAGER_ERROR_CHECK(!tchdbput(hdb, key, ksiz, val, vsiz));
  }

  void close() throw (TCManagerException) {
    CheckInitialized();
    TCMANAGER_ERROR_CHECK(!tchdbclose(hdb));
    tchdbdel(hdb);
    hdb = NULL;
  }

  void open(const char *fname)  throw (TCManagerException) {
    TCMANAGER_ERROR_CHECK((hdb = tchdbnew()) == NULL);
    TCMANAGER_ERROR_CHECK(!tchdbopen(hdb, fname, HDBOWRITER | HDBOCREAT));
  }
};

#undef TCMANAGER_ERROR_CHECK
#endif /* TCMANAGER_HPP */

// // usage example
// #include <iostream>
// using namespace std;
// int main(int argc, char *argv[])
// {
//   TCManager tc("test.hdb");
//   tc.write("aaa", 4, "bbb", 4);
//   tc.append("aaa", 4, "ccc", 4);
//   const char *val;
//   int siz;
//   tc.read("aaa", 4, reinterpret_cast<const void **>(&val), &siz);
//   cout << val << " " << val + 4 << " " << siz << endl;
//   free(val);
//   int n = tc.inc("ddd", 4, 1);
//   cout << n << endl;
//   n = tc.inc("ddd", 4, 5);
//   cout << n << endl;
//   tc.close();
//   return 0;
// }
