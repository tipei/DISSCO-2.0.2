#include "LASS.h"

int main (int argc, const char * argv[]) {

    XmlReader xml;
    if(!xml.openFile((char*)argv[1]))
    {
         printf("unable to open file\n");
         exit(1);
    } 

    XmlReader::xmltagset *root_tags=xml.readXMLDocument();
    xml.closeFile();

    // Note that there could actually be many root objects,
    // such as multiple scores
    XmlReader::xmltag *scoretag=root_tags->find("score");
    if(!scoretag)
    {
         printf("Document does not contain a score\n");
         exit(1);
    }

    Score *score = new Score();
    score->xml_read(scoretag);
    cout << "Done reading file!" << endl;
    score->xml_print("testout.xml");

    // render in one channel, 44000 Hz
    cout << "Done Parsing! Now creating sound." << endl;
    
    MultiTrack* renderedScore = score->render(2,44100);

    cout << "Done with render" << endl;
    // write to file
    AuWriter::write(*renderedScore, (char*) argv[2]);
    
    delete renderedScore;
    delete score;
}



