#include <Rcpp.h>
#include "wkheaders/wkb-iterator.h"
#include "wkheaders/io-utils.h"
#include <iostream>
using namespace Rcpp;

class RawVectorBinaryReader: public BinaryReader {
public:
  bool hasNext;
  RawVector data;
  R_xlen_t offset;

  RawVectorBinaryReader(RawVector data) {
    this->data = data;
    this->offset = 0;
    this->hasNext = true;
  }

protected:
  unsigned char readCharRaw() {
    return readBinary<unsigned char>();
  }

  double readDoubleRaw() {
    return readBinary<double>();
  }

  uint32_t readUint32Raw() {
    return readBinary<uint32_t>();
  }

  bool seekNextFeature() {
    if (this->hasNext) {
      this->hasNext = false;
    }

    return this->hasNext;
  }

private:
  template<typename T>
  T readBinary() {
    // Rcout << "Reading " << sizeof(T) << " starting at " << this->offset << "\n";
    if ((this->offset + sizeof(T)) > this->data.size()) {
      stop("Reached end of RawVector input");
    }

    T dst;
    memcpy(&dst, &(this->data[this->offset]), sizeof(T));
    this->offset += sizeof(T);
    return dst;
  }
};

class WKTTranslateIterator: public WKBIterator {
public:

  WKTTranslateIterator(BinaryReader* reader, std::ostream& out): WKBIterator(reader), out(out) {

  }

  void nextFeature() {
    WKBIterator::nextFeature();
    this->out << "\n";
  }

  void nextGeometryType(GeometryType geometryType) {
    if (!geometryType.hasSRID) {
      this->out << geometryType.wktType();
    }
  }

  void nextSRID(GeometryType geometryType, uint32_t srid) {
    this->out << "SRID=" << srid << ";" << geometryType.wktType();
  }

  void nextEmpty(GeometryType geometryType) {
    this->out << " EMPTY";
  }

  void nextGeometry(GeometryType geometryType, uint32_t size) {
    this->out << " (";
    WKBIterator::nextGeometry(geometryType, size);
    this->out << ")";
  }

  void nextXY(double x, double y) {
    this->out << x << " " << y;
  }

  void nextXYZ(double x, double y, double z) {
    this->out << x << " " << y << " " << z;
  }

  void nextXYM(double x, double y, double m) {
    this->out << x << " " << y << " " << m;
  }

  void nextXYZM(double x, double y, double z, double m) {
    this->out << x << " " << y << " " << z << " " << m;
  }

private:
  std::ostream& out;
};



// [[Rcpp::export]]
void test_basic_reader(RawVector data) {
  WKTTranslateIterator iter(new RawVectorBinaryReader(data), Rcout);
  iter.nextFeature();
}
