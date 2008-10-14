#ifndef __PKF_MANIP_H__
#define __PKF_MANIP_H__

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <assert.h>

// using namespace std;
using std::cout;
using std::cerr;
using std::istream;
using std::endl;
using std::ostream;

static const char szPKFString[]="PKF 0.2";

class PKFmanip {

  char* _NameString;
  char* _EticString;
  char* _CiteString;
  char* _HistoryString;

  void init();
  void clear();

  inline char *setString(const char *string);
  char *readString(istream &is,const char *szLengthTag,
		   const char *szTag);
  void writeString(ostream &os,const char *szLengthTag,
		   const char *szTag,const char *szString);

 public:

  PKFmanip();
  PKFmanip(const PKFmanip &h);
  PKFmanip &operator= (const PKFmanip &h);
  ~PKFmanip();

  void header(const char* name="No name",const char* etic="",
	      const char *cite="", const char *history="");

  void setName(const char *name);
  void setEtic(const char *etic);
  void setCite(const char *cite);
  void setHistory(const char *history);
  const char *getName() const;
  const char *getEtic() const;
  const char *getCite() const;
  const char *getHistory() const;

  int readHeader(istream& in);
  int writeHeader(ostream& out);

  friend ostream & operator<<(ostream &out, PKFmanip &c);
};

inline ostream &operator<<(ostream &out, PKFmanip &pkf) {
  
  // Spit out PKF header
  pkf.writeHeader(out);
  return out;
}

inline char* PKFmanip::setString(const char *string) {
  char *tmp = new char[strlen(string)+1];
  strcpy(tmp,string);
  return tmp;
}


#endif // __PKF_MANIP_H__
