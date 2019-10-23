#include "LASS.h"

/**
 *    This sample file loads and displays a library of previously
 *    created Envelopes, see samplelib1.cpp.
 *    Note: a valid library with the file name "library1" needs
 *    to exist in order to use this program.
 **/

int main(int argc, const char* argv[]) {
    EnvelopeLibrary envlib;
    int number_of_envelopes;

    // loadLibrary returns the number of Envelopes in the library
    number_of_envelopes = envlib.loadLibrary("library1");

    cout << "Library entries (-1 = ERROR): " << number_of_envelopes << "\n";

    // display library entries
    if (number_of_envelopes != -1)
        for (int loop = 1; loop < (number_of_envelopes + 1); loop++) {
            cout << "Envelope #" << loop << ":\n";
            envlib.showEnvelope(loop);
        }
}
