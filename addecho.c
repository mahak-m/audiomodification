#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

/*
 * This program takes an existing wav file and 
 * creates a new wav file that has echo added to it.
 */
int main(int argc, char *argv[]) {
    
    /*Reading Input*/

    int opt;
    int d = 0; 
    int v = 0;
    int delay = 8000; 
    int volume_scale = 4;
    while ((opt = getopt(argc, argv, "d:v:")) != -1) 
    {
        switch (opt) 
        {
            case 'd':
                delay = strtol(optarg, NULL, 10);
                d = 1;
                break;
            case 'v': 
                volume_scale = strtol(optarg, NULL, 10);
                v = 1;
                break;
        }
    }
    
    /*Checking Input*/

    if (delay < 0) {
        d = 8000;
    }
    if (volume_scale < 1) {
        volume_scale = 4;
    }

    int source_file_index;
    int dest_file_index;

    if (d == 0 && v == 0) {
        if (argc < 3) {
            printf("Error: wrong number of arguments \n");
            printf("Usage: addecho [-d delay] [-v volume_scale] sourcewav destwav \n");
            return 1;
        }
        source_file_index = 1;
        dest_file_index = 2;
    }

    else if ((d == 0 && v == 1) || (d == 1 && v == 0)) {
        if (argc < 4) {
            printf("Error: wrong number of arguments \n");
            printf("Usage: addecho [-d delay] [-v volume_scale] sourcewav destwav \n");
            return 1;
        }
        source_file_index = 3;
        dest_file_index = 4;
    }
    else {
        if (argc < 5) {
            printf("Error: wrong number of arguments \n");
            printf("Usage: addecho [-d delay] [-v volume_scale] sourcewav destwav \n");
            return 1;
        }
        source_file_index = 5;
        dest_file_index = 6;
    }

    /*Opening Files*/

    FILE *source_wav1, *source_wav2, *dest_wav;

    source_wav1 = fopen(argv[source_file_index], "rb");
    if (source_wav1 == NULL) {
        fprintf(stderr, "Error: could not open sourcewav file 1 \n");
        return 1;
    }
    source_wav2 = fopen(argv[source_file_index], "rb");
    if (source_wav2 == NULL) {
        fprintf(stderr, "Error: could not open sourcewav file 2 \n");
        return 1;
    }
    dest_wav = fopen(argv[dest_file_index], "wb");
    if (dest_wav == NULL) {
        fprintf(stderr, "Error: could not open destwav file \n");
        return 1;
    }

    /* Read and Write Header*/

    /* We will read the header in samples of 4 bytes, as that is the size of an int. */ 
    char header1[4];
    char header2[32];
    int error2;
    int size_int;
    
    /* Read and write the first 4 bytes normally at the beginning of the header. */
    fread(header1, sizeof(char) * 4, 1, source_wav1);
    error2 = fwrite(header1, 4, 1, dest_wav);
    if (error2 != 1) {
        fprintf(stderr, "Error: could not write to header \n");
        return 1;
    }

    /* The next 4 bytes are a size int so they need to be updated before writing. */
    fread(&size_int, sizeof(int), 1, source_wav1);
    size_int = size_int + (delay * 2);
    error2 = fwrite(&size_int, sizeof(int), 1, dest_wav);
    if (error2 != 1) {
        fprintf(stderr, "Error: could not write to header \n");
        return 1;
    }

    /* Read and write the next 32 bytes normally till the next size int. */
    fread(header2, 32, 1, source_wav1);
    error2 = fwrite(header2, 32, 1, dest_wav);
    if (error2 != 1) {
        fprintf(stderr, "Error: could not write to header \n");
        return 1;
    }

    /* The last 4 bytes are a size int so they need to be updated before writing. */
    fread(&size_int, sizeof(int), 1, source_wav1);
    size_int = size_int + (delay * 2);
    error2 = fwrite(&size_int, sizeof(int), 1, dest_wav);
    if (error2 != 1) {
        fprintf(stderr, "Error: could not write to header \n");
        return 1;
    }

    fseek(source_wav2, 44, 0);

    /* Now read the rest of the samples in the the file and add echo. */

    /* Read and write samples before delay normally. */
    short sample;
    int delay_counter = 0;
    while ((fread(&sample, sizeof(short), 1, source_wav1) == 1) && (delay_counter < delay)) {
        error2 = fwrite(&sample, sizeof(short), 1, dest_wav);
        delay_counter = delay_counter + 1;
        if (error2 != 1) {
            fprintf(stderr, "Error: could not write a sample \n");
            return 1;
        }
    }
    
    delay = delay - delay_counter;

    if (delay > 0) {
        for (int i = 0; i < delay; i++) {
            short no_sound = 0;
            error2 = fwrite(&no_sound, sizeof(short), 1, dest_wav);
            if (error2 != 1) {
                fprintf(stderr, "Error: could not write a sample \n");
                return 1;
            }
        }
    }
    
    /* Read and write samples in between mixed with the echo. */
    short echo_sample;
    while (fread(&sample, sizeof(short), 1, source_wav1) == 1) {
        fread(&echo_sample, sizeof(short), 1, source_wav2);
        sample = sample + (echo_sample / volume_scale);
        error2 = fwrite(&sample, sizeof(short), 1, dest_wav);
        if (error2 != 1) {
            fprintf(stderr, "Error: could not write a sample \n");
            return 1;
        }
    }

    /* Finish writing the delayed echo. */
    while (fread(&echo_sample, sizeof(short), 1, source_wav2) == 1) {
        echo_sample = echo_sample / volume_scale;
        error2 = fwrite(&echo_sample, sizeof(short), 1, dest_wav);
        if (error2 != 1) {
            fprintf(stderr, "Error: could not write a sample \n");
            return 1;
        }
    }
    
    /*Closing Files*/

    int error3;
    error3 = fclose(source_wav1);
    if (error3 != 0) {
        fprintf(stderr, "Error: fclose failed for sourcewav 1");
        return 1;
    }
    error3 = fclose(source_wav2);
    if (error3 != 0) {
        fprintf(stderr, "Error: fclose failed for sourcewav 1");
        return 1;
    }
    error3 = fclose(dest_wav);
    if (error3 != 0) {
        fprintf(stderr, "Error: fclose failed for destwav");
        return 1;
    }

    return 0;
}
