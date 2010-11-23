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

#ifndef DOCINFO_HPP
#define DOCINFO_HPP

#include <cstring>

struct DocInfo {
  int docid;

  size_t wordnum;
  size_t urllen;
  size_t textlen;
  const char *url;
  const char *text;

  DocInfo(int _docid)
    : docid(_docid),  wordnum(0), urllen(0), textlen(0),
      url(NULL), text(NULL)  { }

  ~DocInfo(){
    if(url != NULL) delete[] url;
    if(text != NULL) delete[] text;
  }

  void serialize(unsigned char **_data, size_t *_data_size) const throw() {
    size_t data_size = sizeof(size_t) * 3 + sizeof(char) * (urllen + textlen);
    unsigned char *data = new unsigned char[data_size];

    size_t offset = 0;
    memcpy(data + offset, &wordnum, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(data + offset, &urllen, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(data + offset, &textlen, sizeof(size_t));
    offset += sizeof(size_t);
    if(urllen > 0){
      memcpy(data + offset, url, sizeof(char) * urllen);
      offset += sizeof(char) * urllen;
    }
    if(textlen > 0){
      memcpy(data + offset, text, sizeof(char) * textlen);
    }

    *_data_size = data_size;
    *_data = data;
  }

  void deserialize(unsigned char *data, const size_t data_size,
                   bool with_text) throw() {

    if(data_size < sizeof(size_t) * 3 + sizeof(char) * (urllen + textlen))
      return;

    size_t offset = 0;
    memcpy(&wordnum, data + offset, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(&urllen,  data + offset, sizeof(size_t));
    offset += sizeof(size_t);
    memcpy(&textlen, data + offset, sizeof(size_t));
    offset += sizeof(size_t);

    if(urllen > 0){
      char *urll = new char[urllen];
      memcpy(urll, data + offset, sizeof(char) * urllen);
      url = urll;
      offset += sizeof(urllen * sizeof(char));
    }

    if(with_text && textlen > 0){
      char *textl = new char[textlen];
      memcpy(textl, data + offset, sizeof(char) * textlen);
      text = textl;
    }
  }
};


#endif /* DOCINFO_HPP */
