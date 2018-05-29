#include <cstdio>

#include "Composer.hpp"
#include "Exception.hpp"
#include "midiAnalyser.hpp"
#include "midiFile.hpp"
#include "midiFileReader.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Midi Version: 3\n\n");
        printf("Usage:\n");
        printf("\t%s -a <filename>\n", argv[0]);
        printf("\t%s -c\n", argv[0]);
        printf("Note:\n");
        printf("\t-c, create sample.mid file from standard input\n");
        printf("\t-a, analysis .mid file\n");
        return 0;
    }

    if (strcmp(argv[1], "-a") == 0) {
        if (argc < 3) {
            printf("missing file name\n");
            return 0;
        }

        midiFileReader reader;
        midiAnalyser analyser;

        try {
            midiFile mfile = reader.read(argv[2]);
            analyser.analyse(mfile);
        } catch (Exception e) {
            e.print();
        }

        return 0;
    }

    if (strcmp(argv[1], "-c") == 0) {
        Composer composer(56);
        midiFile midi = composer.compose();
        midi.write_file("sample.mid");

        return 0;
    }

    printf("Unknow parameters\n");

    return 0;
}
