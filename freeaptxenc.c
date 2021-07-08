/*
 * aptX encoder utility
 * Copyright (C) 2018-2020  Pali Rohár <pali.rohar@gmail.com>
 * Copyright (C) 2021       Hunter Wardlaw <wardlawhunter@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include <freeaptx.h>

static unsigned char input_buffer[512*3*2*4];
static unsigned char output_buffer[512*6];

int main(int argc, char *argv[])
{
    int i;
    int hd;
    int ret;
    size_t length;
    size_t processed;
    size_t written;
    struct aptx_context *ctx;

#ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    hd = 0;

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            fprintf(stderr, "aptX encoder utility %d.%d.%d (using libfreeaptx %d.%d.%d)\n", freeaptx_MAJOR, freeaptx_MINOR, freeaptx_PATCH, aptx_major, aptx_minor, aptx_patch);
            fprintf(stderr, "\n");
            fprintf(stderr, "This utility encodes a raw 24 bit signed stereo\n");
            fprintf(stderr, "samples from stdin to aptX or aptX HD on stdout\n");
            fprintf(stderr, "\n");
            fprintf(stderr, "Usage:\n");
            fprintf(stderr, "        %s [options]\n", argv[0]);
            fprintf(stderr, "\n");
            fprintf(stderr, "Options:\n");
            fprintf(stderr, "        -h, --help   Display this help\n");
            fprintf(stderr, "        --hd         Encode to aptX HD\n");
            fprintf(stderr, "\n");
            fprintf(stderr, "Examples:\n");
            fprintf(stderr, "\n");
            fprintf(stderr, "        %s < sample.s24le > sample.aptx\n", argv[0]);
            fprintf(stderr, "\n");
            fprintf(stderr, "        %s --hd < sample.s24le > sample.aptxhd\n", argv[0]);
            fprintf(stderr, "\n");
            fprintf(stderr, "        sox sample.wav -t raw -r 44.1k -L -e s -b 24 -c 2 - | %s > sample.aptx\n", argv[0]);
            return 1;
        } else if (strcmp(argv[i], "--hd") == 0) {
            hd = 1;
        } else {
            fprintf(stderr, "%s: Invalid option %s\n", argv[0], argv[i]);
            return 1;
        }
    }

    ctx = aptx_init(hd);
    if (!ctx) {
        fprintf(stderr, "%s: Cannot initialize aptX encoder\n", argv[0]);
        return 1;
    }

    ret = 0;

    while (!feof(stdin)) {
        length = fread(input_buffer, 1, sizeof(input_buffer), stdin);
        if (ferror(stdin)) {
            fprintf(stderr, "%s: aptX encoding failed to read input data\n", argv[0]);
            ret = 1;
        }
        if (length == 0)
            break;
        processed = aptx_encode(ctx, input_buffer, length, output_buffer, sizeof(output_buffer), &written);
        if (processed != length) {
            fprintf(stderr, "%s: aptX encoding stopped in the middle of the sample, dropped %lu byte%s\n", argv[0], (unsigned long)(length-processed), (length-processed != 1) ? "s" : "");
            ret = 1;
        }
        if (fwrite(output_buffer, 1, written, stdout) != written) {
            fprintf(stderr, "%s: aptX encoding failed to write encoded data\n", argv[0]);
            ret = 1;
            break;
        }
        if (processed != length)
            break;
    }

    if (aptx_encode_finish(ctx, output_buffer, sizeof(output_buffer), &written)) {
        if (fwrite(output_buffer, 1, written, stdout) != written) {
            fprintf(stderr, "%s: aptX encoding failed to write encoded data\n", argv[0]);
            ret = 1;
        }
    }

    aptx_finish(ctx);
    return ret;
}
