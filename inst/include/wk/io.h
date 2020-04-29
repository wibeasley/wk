
#ifndef WK_IO
#define WK_IO

class WKProvider {
public:
  virtual bool seekNextFeature() = 0;
  virtual bool featureIsNull() = 0;
  virtual size_t nFeatures() = 0;
  virtual ~WKProvider() {}
};

class WKExporter {
public:
  WKExporter(size_t size): size(size) {}
  virtual bool seekNextFeature() = 0;
  virtual void writeNull() = 0;
  size_t nFeatures() {
    return this->size;
  }

  virtual ~WKExporter() {}

private:
  size_t size;
};

#endif
