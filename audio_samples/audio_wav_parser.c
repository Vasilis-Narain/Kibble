/*
 * SPDX-FileCopyrightText: 2026 Vasilis Narain
 *
 * SPDX-License-Identifier: MIT
 */

#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

const char *file_name = "laserShoot.wav";

#define ID_SIZE 4u

#define EXIT(stream, num) \
    do {                  \
        fclose(stream);   \
        return num;       \
    } while (0)

typedef struct {
    // RIFF CHUNK
    uint8_t chunkID[ID_SIZE];    // RIFF
    uint32_t chunkDataSize;      // 36 + Chunk2 data size
    uint8_t RIFFtypeID[ID_SIZE]; // WAVE
} CHUNK_RIFF_t;

typedef struct {
    uint8_t chunkID[ID_SIZE]; //eg: fmt, data
    uint32_t chunkSize;       // size of current chunk
} CHUNK_info_t;

typedef struct {
    uint16_t formatTag;     // format tag: 1 = PCM
    uint16_t numChannels;   // 1=Mono 2=Stereo
    uint32_t sampleRate;    // Sample frequency in Hz (44100)
    uint32_t byteRate;      // byte rate
    uint16_t blockAlign;    // 4
    uint16_t bitsPerSample; // 16
} CHUNK_fmt_t;

typedef enum {
    FMT_PCM = 0x0001,
    FMT_IMA_ADPCM = 0x0011,
} VALID_FMT_TAG;

static void log_id(const uint8_t *id, const char *str) {
    printf("%s: %c%c%c%c\r\n", str, id[0], id[1], id[2], id[3]);
}

bool check_id(const uint8_t *id, const char *str) {
    if (memcmp(id, str, ID_SIZE) != 0) {
        return false;
    }
    return true;
}

int main(void) {
    FILE *file_stream;
    file_stream = fopen(file_name, "rb");

    if (!file_stream) {
        printf("Could not open file\r\n");
        return 1;
    }

    // RIFF header
    CHUNK_RIFF_t riff;

    if (fread(&riff, 1, sizeof(riff), file_stream) != sizeof(riff)) {
        printf("Failed reading header\r\n");
        EXIT(file_stream, 1);
    }
    log_id(riff.chunkID, "Chunk ID");
    if (!check_id(riff.chunkID, "RIFF")) {
        printf("Bad chunk id!\r\n");
        EXIT(file_stream, 1);
    }
    log_id(riff.RIFFtypeID, "RIFF Type ID");
    if (!check_id(riff.RIFFtypeID, "WAVE")) {
        printf("Bad RIFF type id!\r\n");
        EXIT(file_stream, 1);
    }

    CHUNK_fmt_t fmt;
    bool fmt_found = false;
    CHUNK_info_t chunk_info;
    int32_t data_start = 0;

    while (1) {
        if (fread(&chunk_info, 1, sizeof(chunk_info), file_stream) != sizeof(chunk_info)) {
            printf("Failed reading chunk\r\n");
            EXIT(file_stream, 1);
        }
        uint32_t skip = chunk_info.chunkSize + (chunk_info.chunkSize & 1); // padding byte
        if (check_id(chunk_info.chunkID, "fmt ")) {
            if (chunk_info.chunkSize < 16) {
                printf("Fmt size cant be less than 16!\r\n");
                EXIT(file_stream, 1);
            }
            if (fread(&fmt, 1, sizeof(fmt), file_stream) != sizeof(fmt)) {
                printf("Failed reading fmt\r\n");
                EXIT(file_stream, 1);
            }
            if (!((fmt.formatTag == FMT_PCM) || (fmt.formatTag == FMT_IMA_ADPCM))) {
                printf("Unsupported fmt tag: %u\r\n", fmt.formatTag);
                EXIT(file_stream, 1);
            }
            if (!(fmt.numChannels == 1)) {
                printf("Unsupported num channels: %u\r\n", fmt.numChannels);
                EXIT(file_stream, 1);
            }
            if (!(fmt.bitsPerSample == 16 || fmt.bitsPerSample == 8 || fmt.bitsPerSample == 4)) {
                printf("Unsupported bits per sample: %u\r\n", fmt.bitsPerSample);
                EXIT(file_stream, 1);
            }
            if (fmt.sampleRate > 48000u || fmt.sampleRate < 8000u) {
                printf("Unsupported sample rate: %u\r\n", fmt.sampleRate);
                EXIT(file_stream, 1);
            }
            // skip whatever's left (size - 16)
            skip -= 16; //skip guaranteed >= 16 from above check
            fmt_found = true;
        }
        if (check_id(chunk_info.chunkID, "data")) {
            if (!fmt_found) {
                printf("Found data before fmt!\r\n");
                EXIT(file_stream, 1);
            }
            data_start = ftell(file_stream);
            if (data_start < 0) {
                printf("Something went horribly wrong..\r\n");
                EXIT(file_stream, 1);
            }
            break;
        }

        if (fseek(file_stream, skip, SEEK_CUR) != 0) {
            printf("Something bad happened..\r\n");
            EXIT(file_stream, 1);
        }
    }
    log_id(chunk_info.chunkID, "Data ID");
    printf("Data size: %u\r\n", chunk_info.chunkSize);
    printf("Data start position: %u\r\n", data_start);

    EXIT(file_stream, 0);
}
