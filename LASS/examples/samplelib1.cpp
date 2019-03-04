#include "LASS.h"

/**
*    This sample file demonstrates how to create an envelope library
*    using two Collections (xy_point and envelope_segment). Bound checking
*    occurs in Envelope to insure that for n+1 points, 
*    there are n corresponding segments connecting them.
*    
*    This program constructs 3 sample envelopes and creates a new library
*    file using the EnvelopeLibrary class. An alternative method for creating
*    an Envelope using a Envelope * rather than Collections 
*    is also shown in comments
**/ 

int main (int argc, const char * argv[]) {

  EnvelopeLibrary envlib;

  Collection<xy_point> xyPointCollection;
  Collection<envelope_segment> segmentCollection;

  xy_point xy;
  envelope_segment seg;



  // first envelope, an ADSR-type 
  xy.x = 0.0; xy.y = 0.0;
  xyPointCollection.add(xy);
  xy.x = 0.5; xy.y = 1.0;
  xyPointCollection.add(xy);
  xy.x = 1.0; xy.y = 0.75;
  xyPointCollection.add(xy);
  xy.x = 1.5; xy.y = 0.75;
  xyPointCollection.add(xy);
  xy.x = 2.0; xy.y = 0.0;
  xyPointCollection.add(xy);

  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
  segmentCollection.add(seg);

  // add the envelope as a pair of Collections
  // addEnvelope() returns the index of the library entry
  cout << "Adding library entry: " << envlib.addEnvelope(xyPointCollection,
							 segmentCollection) <<
    "\n";

  /* one can also use addEnvelope with a Envelope *,
     the previous code becomes:

  Envelope * env;
  env = new Envelope (xyPointCollection, segmentCollection);
  cout << "Adding library entry: " << envlib.addEnvelope(env) << "\n"; 

  */

  // clear the collections in order to construct a fresh envelope
  xyPointCollection.clear();
  segmentCollection.clear();



  // second envelope, example from sampleenv7.cpp
  xy.x = 0.0; xy.y = 0.0;
  xyPointCollection.add(xy);
  xy.x = 0.5; xy.y = 0.8;
  xyPointCollection.add(xy);
  xy.x = 0.8; xy.y = 0.8;
  xyPointCollection.add(xy);
  xy.x = 1.0; xy.y = 0.0;
  xyPointCollection.add(xy);
  xy.x = 1.2; xy.y = 0.0;
  xyPointCollection.add(xy);
  xy.x = 1.7; xy.y = 0.8;
  xyPointCollection.add(xy);
  xy.x = 2.0; xy.y = 0.8;
  xyPointCollection.add(xy);
  xy.x = 2.2; xy.y = 0.0;
  xyPointCollection.add(xy);

  seg.interType = EXPONENTIAL; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .3;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .2;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .2;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .3;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .2;
  segmentCollection.add(seg);

  // add the envelope as a pair of Collections
  // addEnvelope() returns the index of the library entry
  cout << "Adding library entry: " << envlib.addEnvelope(xyPointCollection,
							 segmentCollection) <<
    "\n";

  /* one can also use addEnvelope with a Envelope *,
     the previous code becomes:

  Envelope * env;
  env = new Envelope (xyPointCollection, segmentCollection);
  cout << "Adding library entry: " << envlib.addEnvelope(env) << "\n"; 

  */

  // clear the collections in order to construct a fresh envelope
  xyPointCollection.clear();
  segmentCollection.clear();



  // third envelope, a simple spike with flexible attack 
  xy.x = 0.0; xy.y = 0.0;
  xyPointCollection.add(xy);
  xy.x = 0.5; xy.y = 1.0;
  xyPointCollection.add(xy);
  xy.x = 1.0; xy.y = 0.0;
  xyPointCollection.add(xy);

  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
  segmentCollection.add(seg);
  seg.interType = LINEAR; seg.lengthType = FLEXIBLE; seg.timeValue = .5;
  segmentCollection.add(seg);

  // add the envelope as a pair of Collections
  // addEnvelope() returns the index of the library entry
  cout << "Adding library entry: " << envlib.addEnvelope(xyPointCollection,
							 segmentCollection) <<
    "\n";

  /* one can also use addEnvelope with a Envelope *,
     the previous code becomes:

  Envelope * env;
  env = new Envelope (xyPointCollection, segmentCollection);
  cout << "Adding library entry: " << envlib.addEnvelope(env) << "\n"; 

  */



  // save the library with a given filename
  bool verify = envlib.saveLibrary("library1");

  // saveLibrary() returns true if the library is saved properly, false otherwise
  cout << "Library written (1=OK): " << verify << "\n";

  // display the added envelopes using size() to find the upper library index bound
  for (int loop = 1; loop < (envlib.size () + 1); loop++)
    {
      cout << "Envelope #" << loop << ":\n";
      envlib.showEnvelope (loop);
    }
}
