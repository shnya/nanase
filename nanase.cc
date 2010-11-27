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

#include "nanase.hpp"

#include <iostream>
#include <iomanip>
using namespace std;
using namespace nanase;
int main(int argc, char *argv[])
{
  Nanase nanase("test");

  Indexer idx = nanase.get_indexer();
  idx.add("http://www.yahoo.co.jp/", "aaa bbb dddd dddccc");
  idx.add("http://www.google.co.jp/", "aaa bbb ddd ccc");


  Searcher sch = nanase.get_searcher();
  std::vector<Searcher::ResultType> v = sch.search("dddd");

  if(v.size() == 0)
    cout << "No results found" << endl;
  for(std::vector<Searcher::ResultType>::iterator itr = v.begin();
      itr != v.end(); ++itr){
    cout << itr->docid << " " << itr->url << " " << itr->score << endl;
  }

  nanase.close();

  return 0;
}
