/*!
  \class PKFmanip include/PKFmanip.h include/PKFmanip.h
  \ingroup BiarcLibGroup
  \brief The PKFmanip class for storing and manipulating biarc curves
  
*/
 
//
// documentation of inlined methods
//
 
/*!
  \fn ostream & PKFmanip::operator<<(ostream &out, PKFmanip &c)

  Overloaded left shift operator. Writes the current PKFmanip object \a c
  to the ostream object \a out. If there is an interpolated curve,
  this function prints point/tangent,matching point/tangent of all
  the biarcs of the curve. For non valid biarcs only the point/tangent
  data is written to the stream.
*/

/*!
  \fn const char* PKFmanip::setString(const char *string)

  Constructs a new string and returns a pointer to it.
*/

#include "../include/PKFmanip.h"

/*!
  Internal function to initialize a PKFmanip object. Sets
  The header strings to NULL.
*/
void PKFmanip::init() {
  _NameString = NULL; _EticString = NULL;
  _CiteString = NULL; _HistoryString = NULL;
}

/*!
  Change the current header. \a name is the name
  of the curve. \a etic is citation of people at
  the origin of this particular curve. \a cite is
  the people to cite if this curve is used further.
  \a history, comments or other infos about the curve.

  Default values : "No name","","",""

  \sa setName(),setEtic(),setCite(),setHistory()
*/
void PKFmanip::header(const char* name,const char* etic,
		      const char* cite,const char* history) {
  setName(name); setEtic(etic);
  setCite(cite); setHistory(history);
}

/*!
  For the destructor, destroys the header strings
  and sets all to NULL.
*/
void PKFmanip::clear() {
  delete _NameString;    _NameString = NULL;
  delete _EticString;    _EticString = NULL;
  delete _CiteString;    _CiteString = NULL;
  delete _HistoryString; _HistoryString = NULL;
}

/*!
  Constructs an empty curve and sets the header to
  "No name","","",""
*/
PKFmanip::PKFmanip() { init(); header("No name","","",""); }

/*!
  Copy constructor.

  \sa operator=
*/
PKFmanip::PKFmanip(const PKFmanip &h) {
  *this = h;
}

/*!
  Assign operator. Copies the header strings from \a h.
*/
PKFmanip& PKFmanip::operator= (const PKFmanip &h) {
  init();
  header(h.getName(),h.getEtic(),h.getCite(),h.getHistory());
  return *this;
}

/*!
  Delete the header string and destroy the PKFmanip instance.
*/
PKFmanip::~PKFmanip() {
  clear();
}

/*!
  Set the name of the curve.

  \sa setEtic(),setCite(),setHistory()
*/
void PKFmanip::setName(const char *name) {
  delete _NameString;
  _NameString = setString(name);
}

/*!
  Set the etic string of the curve.

  \sa setName(),setCite(),setHistory()
*/
void PKFmanip::setEtic(const char *etic) {
  delete _EticString;
  _EticString = setString(etic);
}

/*!
  Set the cite string of the curve.

  \sa setName(),setEtic(),setHistory()
*/
void PKFmanip::setCite(const char *cite) {
  delete _CiteString;
  _CiteString = setString(cite);
}

/*!
  Set the history string of the curve.

  \sa setName(),setEtic(),setCite()
*/
void PKFmanip::setHistory(const char *history) {
  delete _HistoryString;
  _HistoryString = setString(history);
}

/*!
  Returns a pointer to the name of the curve.

  \sa getEtic(),getCite(),getHistory()
*/
const char* PKFmanip::getName() const {return _NameString;}

/*!
  Returns a pointer to the etic string.

  \sa getName(),getCite(),getHistory()
*/
const char* PKFmanip::getEtic() const {return _EticString; }

/*!
  Returns a pointer to the cite string.

  \sa getName(),getEtic(),getHistory()
*/
const char* PKFmanip::getCite() const {return _CiteString;}

/*!
  Returns a pointer to the history string.

  \sa getName(),getEtic(),getCite()
*/
const char* PKFmanip::getHistory() const {return _HistoryString;}


// (c) Ben Laurie PKF Methods
/*!
  Used to read the PKF header strings name,etic,cite and history.

  \sa writeString()
*/
char* PKFmanip::readString(istream &is,
			const char *szLengthTag,
			const char *szTag) {
  char buf[256];
  is.getline(buf,sizeof buf);
  if(strncmp(buf,szLengthTag,strlen(szLengthTag))) {
    cerr << "Expected " << szLengthTag << ": " << buf << endl;
    exit(1);
  }
  int nLength=atoi(buf+strlen(szLengthTag)+1);
  int nTagLength=strlen(szTag);
  char *szString=new char[nLength+1];
  *szString='\0';
  for(int n=0 ; n < nLength ; ) {
    is.getline(buf,sizeof buf);
    if(strncmp(buf,szTag,nTagLength) || buf[nTagLength] != ' ') {
      cerr << "Expected " << szTag << ": " << buf << endl;
      exit(1);
    }
    for(int l=nTagLength+1 ; buf[l] ; ++l,++n) {
      assert(n < nLength);
      if(buf[l] == '%') {
	char num[3];
	num[0]=buf[l+1];
	num[1]=buf[l+2];
	num[2]='\0';
	szString[n]=strtol(num,NULL,16);
	l+=2;
      }
      else
	szString[n]=buf[l];
    }
  }
  is.getline(buf,sizeof buf);
  if(strncmp(buf,"END",3)) {
    cerr << "Expected END: " << buf << endl;
    exit(1);
  }
  return szString;
}

#define MAX_LINE_LENGTH		255
/*!
  Used to write the PKF header strings name,etic,cite and history.

  \sa readString()
*/
void PKFmanip::writeString(ostream &os,
			const char *szLengthTag,
			const char *szTag,
			const char *szString) {

  int nMax=MAX_LINE_LENGTH-strlen(szTag)-4;

  os << szLengthTag << ' ' << strlen(szString);
  for(int n=MAX_LINE_LENGTH ; *szString ; ++szString) {

    if(n >= nMax) {
      os << '\n';
      n=0;
    }
    if(!n)
      os << szTag << ' ';
    if(*szString < 0x20 || *szString > 0x7e || *szString == '%') {
      char buf[4];
      sprintf(buf,"%%%02x",(unsigned char)*szString);
      os << buf;
      n+=3;
    } else {
      os << *szString;
      ++n;
    }
  }
  os << "\nEND\n";
}

/*!
  Read the PKF header from a stream object \a in.
  Returns 1 if all is ok, 0 otherwise.
  More details about the PKF format are given in classes
  that inherit the PKFmanip class. I.e. Curve, CurveBundle ...

  \sa writeHeader()
*/
int PKFmanip::readHeader(istream &in) {

  char tmp[1024];
  in.getline(tmp,sizeof tmp);

  // Header begin
  if(strncmp(tmp,"PKF 0.2",7)) {
    cerr << "ERROR : readkPKF() : Not in PKF 0.2 format!\n";
    return 0;
  }
  in.getline(tmp,sizeof tmp);
  if(strncmp(tmp,"BIARC_KNOT",10)) {
    cerr << "Expected BIARC_KNOT. Only Biarc Format is supported.\n";
    return 0;
  }
  _NameString    = setString(tmp+11);
  _EticString    = readString(in,"ETICL","ETIC");
  _CiteString    = readString(in,"CITEL","CITE");
  _HistoryString = readString(in,"HISTL","HIST");
  //Header End

  return 1;
}

/*!
  Writes the PKF header to an ostream object \a out.
  Returns 1 if all went well, zero otherwise.

  \sa readPKF()
*/
int PKFmanip::writeHeader(ostream &out) {

  out << szPKFString << '\n';

  assert(!!_NameString);
  assert(!!_EticString);
  assert(!!_CiteString);
  assert(!!_HistoryString);

  out << "BIARC_KNOT ";

  out << _NameString << '\n';
  writeString(out,"ETICL","ETIC",_EticString);
  writeString(out,"CITEL","CITE",_CiteString);
  writeString(out,"HISTL","HIST",_HistoryString);

  return 1;
}
