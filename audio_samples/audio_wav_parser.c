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

#define CLI_INPUT_FLAG "--input"
#define CLI_INPUT_FLAG_SHORT "--i"
#define CLI_GEN_H_FLAG "--gen-header"
#define CLI_GEN_H_FLAG_SHORT "--gh"

#define MAX_FILE_NAME_LEN 20u // 21 including null char
#define MIN_FILE_NAME_LEN 5u

#define ID_SIZE 4u

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

typedef struct {
    CHUNK_fmt_t fmt_desc;
    CHUNK_info_t data_chunk_info;
    uint32_t data_start_pos;
} WAV_HEADER_t;

typedef enum {
    FMT_PCM = 0x0001,
    //FMT_IMA_ADPCM = 0x0011,
} VALID_FMT_TAG;

static bool parse_header(FILE *restrict file_stream, WAV_HEADER_t *wav_header);
static bool generate_header_file(const char *file_name, uint32_t file_name_len, FILE *restrict file_stream, WAV_HEADER_t *wav_header);
static bool verify_file_name(const char *file_name, uint32_t *file_name_len);
static void log_id(const uint8_t *id, const char *str);
static void log_fmt_chunk(CHUNK_fmt_t *fmt);
static bool check_id(const uint8_t *id, const char *str);

int main(int argc, char **argv) {
    if (argc < 4) {
        printf("\r\n     Usage: ./audio_wav_parser --input <file> --gen-header\r\n        or: ./audio_wav_parser --i <file> --gh\r\n");
        return 0;
    }

    bool cli_input = false;
    bool cli_gen_header = false;
    const char *file_name = argv[2];
    uint32_t file_name_len = 0;
    if (!verify_file_name(file_name, &file_name_len)) {
        printf("\r\n     Usage: error with provided file name. Must be %u characters or under, and ending in `.wav`\r\n", MAX_FILE_NAME_LEN);
        return 1;
    }

    if (!strcmp(argv[1], "--input") || !strcmp(argv[1], "--i")) {
        cli_input = true;
    }
    if (!cli_input) {
        printf("\r\n     Usage: ./audio_wav_parser --input <file> --gen-header\r\n        or: ./audio_wav_parser --i <file> --gh\r\n");
        return 0;
    }

    if (!strcmp(argv[3], "--gen-header") || !strcmp(argv[3], "--gh")) {
        cli_gen_header = true;
    }

    FILE *file_stream;
    file_stream = fopen(file_name, "rb");

    if (!file_stream) {
        printf("\r\nexit: could not open file\r\n");
        return 1;
    }

    WAV_HEADER_t wav_header;
    if (!parse_header(file_stream, &wav_header)) {
        printf("\r\nexit: header parsing error\r\n");
        fclose(file_stream);
        return 1;
    }

    if (cli_gen_header) {
        if (!generate_header_file(file_name, file_name_len, file_stream, &wav_header)) {
            printf("\r\nexit: header generation error\r\n");
            fclose(file_stream);
            return 1;
        }
    }

    fclose(file_stream);
    return 0;
}

static bool generate_header_file(const char *file_name, uint32_t file_name_len, FILE *restrict file_stream, WAV_HEADER_t *wav_header) {
    char buf[file_name_len];
    uint32_t i = 0;
    for (; i < file_name_len - 3; ++i) {
        buf[i] = file_name[i];
    }
    buf[i++] = 'h';
    buf[i] = '\0';

    const char *out_file_name = buf;
    uint32_t out_file_name_len = file_name_len - 2;

    char out_lowered[out_file_name_len - 1];
    strncpy(out_lowered, out_file_name, sizeof(out_lowered));
    out_lowered[sizeof(out_lowered) - 1] = '\0';
    for (i = 0; i < sizeof(out_lowered); ++i) {
        char c = out_lowered[i];
        if (c >= 'A' && c <= 'Z') {
            out_lowered[i] = c + 32;
        }
    }
    const char *array_name = out_lowered;

    char out_capitalised[out_file_name_len];
    strncpy(out_capitalised, out_file_name, sizeof(out_capitalised));
    out_capitalised[sizeof(out_capitalised) - 1] = '\0';

    for (i = 0; i < sizeof(out_capitalised); ++i) {
        char c = out_capitalised[i];
        if (c >= 'a' && c <= 'z') {
            out_capitalised[i] = c - 32;
        }
        if (c == '.') {
            out_capitalised[i] = '_';
        }
    }
    const char *header_def = out_capitalised;

    FILE *write_stream = fopen(out_file_name, "w");
    if (!write_stream) {
        printf("\r\ngenerate_header_file: failed to open write file\r\n");
        return false;
    }

    /* REUSE-IgnoreStart */
    fprintf(write_stream, "/*\n");
    fprintf(write_stream, " * SPDX-FileCopyrightText: 2026 Vasilis Narain\n");
    fprintf(write_stream, " *\n");
    fprintf(write_stream, " * SPDX-License-Identifier: MIT\n");
    fprintf(write_stream, " */\n");
    /* REUSE-IgnoreEnd */

    fprintf(write_stream, "\n/* This file was automatically generated from %s */\n", file_name);
    fprintf(write_stream, "\n#ifndef %sH\n", header_def);
    fprintf(write_stream, "#define %sH\n", header_def);

    fprintf(write_stream, "\n#include <stdint.h>\n");
    fprintf(write_stream, "\n#define %sNUM_CHANNELS %uu\n", header_def, wav_header->fmt_desc.numChannels);
    fprintf(write_stream, "#define %sRATE %uu\n", header_def, wav_header->fmt_desc.sampleRate);
    fprintf(write_stream, "#define %sBYTE_RATE %uu\n", header_def, wav_header->fmt_desc.byteRate);
    fprintf(write_stream, "#define %sBLOCK_ALIGN %uu\n", header_def, wav_header->fmt_desc.blockAlign);

    uint16_t bits = wav_header->fmt_desc.bitsPerSample;
    fprintf(write_stream, "#define %sBITS_PER_SAMPLE %uu\n", header_def, bits);
    fprintf(write_stream, "\nstatic const %sint%u_t %s[] = {\n    ", (bits == 8) ? "u" : "", bits, array_name);

    //make sure pointer at the start of data
    if (fseek(file_stream, wav_header->data_start_pos, SEEK_SET) != 0) {
        printf("generate_header_file: something went horribly wrong..\r\n");
        fclose(write_stream);
        return false;
    }

    uint32_t loop_end = wav_header->data_chunk_info.chunkSize / (bits / 8);
    for (i = 0; i < loop_end; ++i) {
        if (bits == 8) {
            uint8_t byte;
            if (fread(&byte, 1, sizeof(byte), file_stream) != sizeof(byte)) {
                printf("generate_header_file: something went horribly wrong..\r\n");
                fclose(write_stream);
                return false;
            }
            fprintf(write_stream, "%u, ", byte);
        } else if (bits == 16) {
            int16_t num;
            if (fread(&num, 1, sizeof(num), file_stream) != sizeof(num)) {
                printf("generate_header_file: something went horribly wrong..\r\n");
                fclose(write_stream);
                return false;
            }
            fprintf(write_stream, "%i, ", num);
        } else {
            printf("\r\ngenerate_header_file: unsupported bits_per_sample value of %u\n", bits);
            fclose(write_stream);
            return false;
        }
        if (i == loop_end - 1) {
            fprintf(write_stream, "\n};");
            break;
        }
        if (i != 0 && i % 12 == 0) {
            fprintf(write_stream, "\n    ");
        }
    }

    fprintf(write_stream, "\n\n#define %sLEN (sizeof(%s) / sizeof(*%s))\n", header_def, array_name, array_name);

    fprintf(write_stream, "\n#endif /* %sH */\n", header_def);
    fclose(write_stream);
    return true;
}

static bool parse_header(FILE *restrict file_stream, WAV_HEADER_t *header) {
    // RIFF header
    CHUNK_fmt_t *out_fmt_desc = &header->fmt_desc;
    CHUNK_info_t *out_data_info = &header->data_chunk_info;
    uint32_t *data_start = &header->data_start_pos;
    CHUNK_RIFF_t riff;

    if (fread(&riff, 1, sizeof(riff), file_stream) != sizeof(riff)) {
        printf("Failed reading header\r\n");
        return false;
    }
    log_id(riff.chunkID, "Chunk ID");
    if (!check_id(riff.chunkID, "RIFF")) {
        printf("Bad chunk id!\r\n");
        return false;
    }
    log_id(riff.RIFFtypeID, "RIFF Type ID");
    if (!check_id(riff.RIFFtypeID, "WAVE")) {
        printf("Bad RIFF type id!\r\n");
        return false;
    }

    bool fmt_found = false;
    int32_t data_start_signed = 0;

    while (1) {
        if (fread(out_data_info, 1, sizeof(*out_data_info), file_stream) != sizeof(*out_data_info)) {
            printf("parse_header: failed reading chunk\r\n");
            return false;
        }
        uint32_t skip = out_data_info->chunkSize + (out_data_info->chunkSize & 1); // padding byte
        if (check_id(out_data_info->chunkID, "fmt ")) {
            if (out_data_info->chunkSize < 16) {
                printf("parse_header: fmt size cant be less than 16!\r\n");
                return false;
            }
            if (fread(out_fmt_desc, 1, sizeof(*out_fmt_desc), file_stream) != sizeof(*out_fmt_desc)) {
                printf("parse_header: failed reading fmt desc\r\n");
                return false;
            }
            if (!(out_fmt_desc->formatTag == FMT_PCM) /*|| (out_fmt_desc->formatTag == FMT_IMA_ADPCM))*/) {
                printf("parse_header: unsupported out_fmt_desc tag: %u\r\n", out_fmt_desc->formatTag);
                return false;
            }
            if (!(out_fmt_desc->numChannels == 1)) {
                printf("parse_header: Unsupported num channels: %u\r\n", out_fmt_desc->numChannels);
                return false;
            }
            if (!(out_fmt_desc->bitsPerSample == 16 || out_fmt_desc->bitsPerSample == 8 || out_fmt_desc->bitsPerSample == 4)) {
                printf("parse_header: unsupported bits per sample: %u\r\n", out_fmt_desc->bitsPerSample);
                return false;
            }
            if (out_fmt_desc->sampleRate > 48000u || out_fmt_desc->sampleRate < 8000u) {
                printf("parse_header: unsupported sample rate: %u\r\n", out_fmt_desc->sampleRate);
                return false;
            }
            // skip whatever's left (size - 16)
            skip -= 16; //skip guaranteed >= 16 from above check
            fmt_found = true;
        }
        if (check_id(out_data_info->chunkID, "data")) {
            if (!fmt_found) {
                printf("parse_header: found data before out_fmt_desc!\r\n");
                return false;
            }
            data_start_signed = ftell(file_stream);
            if (data_start_signed < 0) {
                printf("parse_header: something went horribly wrong..\r\n");
                return false;
            }
            *data_start = (uint32_t)data_start_signed;
            break;
        }

        if (fseek(file_stream, skip, SEEK_CUR) != 0) {
            printf("parse_header: something went horribly wrong..\r\n");
            return false;
        }
    }
    log_fmt_chunk(out_fmt_desc);
    log_id(out_data_info->chunkID, "Data ID");
    printf("Data size: %u\r\n", out_data_info->chunkSize);
    printf("Data start position: %u\r\n", *data_start);
    return true;
}

static bool verify_file_name(const char *file_name, uint32_t *file_name_len) {
    uint32_t n = strnlen(file_name, MAX_FILE_NAME_LEN + 1);
    if (n > MAX_FILE_NAME_LEN) { // without null char
        return false;
    }
    if (n < MIN_FILE_NAME_LEN) {
        return false;
    }
    if (file_name[n - 1] != 'v' || file_name[n - 2] != 'a' || file_name[n - 3] != 'w' || file_name[n - 4] != '.') {
        return false;
    }

    *file_name_len = n;
    return true;
}

static void log_id(const uint8_t *id, const char *str) {
    printf("%s: %c%c%c%c\r\n", str, id[0], id[1], id[2], id[3]);
}

static void log_fmt_chunk(CHUNK_fmt_t *fmt) {
    printf("Format Tag: %u\r\n", fmt->formatTag);
    printf("Num Channels: %u\r\n", fmt->numChannels);
    printf("Sample Rate (Hz): %u\r\n", fmt->sampleRate);
    printf("Byte Rate: %u\r\n", fmt->byteRate);
    printf("Block Align: %u\r\n", fmt->blockAlign);
    printf("Bits per Sample: %u\r\n", fmt->bitsPerSample);
}

static bool check_id(const uint8_t *id, const char *str) {
    if (memcmp(id, str, ID_SIZE) != 0) {
        return false;
    }
    return true;
}
