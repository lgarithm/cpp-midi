#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "Composer.hpp"
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
        printf("\t-c [-b <bpm>], create sample.mid file from standard input\n");
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
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
            return 1;
        }

        return 0;
    }

    if (strcmp(argv[1], "-c") == 0) {
        int bpm = 56;
        if (argc > 3 && strcmp(argv[2], "-b") == 0) {
            sscanf(argv[3], "%d", &bpm);
        }
        Composer composer(bpm);
        midiFile midi = composer.compose();
        midi.write_file("sample.mid");

        return 0;
    }

    printf("Unknow parameters\n");

    return 0;
}
