
#include "wk/geometry-debug-handler.h"
#include "wk/wkb-reader.h"
#include "wk/wkt-reader.h"

#include <Rcpp.h>
#include "wk/io-rcpp.h"
using namespace Rcpp;

// [[Rcpp::export]]
void cpp_debug_wkb(List wkb) {
  WKRawVectorListProvider input(wkb);
  WKGeometryDebugHandler handler(Rcout);
  WKBReader reader(input, handler);

  while (reader.hasNextFeature()) {
    reader.iterateFeature();
  }
}

// [[Rcpp::export]]
void cpp_debug_wkt(CharacterVector input) {
  WKCharacterVectorProvider provider(input);
  WKGeometryDebugHandler handler(Rcout);
  WKTReader reader(provider, handler);

  while (reader.hasNextFeature()) {
    reader.iterateFeature();
  }
}

// [[Rcpp::export]]
void cpp_debug_wkt_streamer(CharacterVector input) {
  WKCharacterVectorProvider provider(input);
  WKGeometryDebugHandler handler(Rcout);
  WKTStreamer reader(provider, handler);

  while (reader.hasNextFeature()) {
    reader.iterateFeature();
  }
}
