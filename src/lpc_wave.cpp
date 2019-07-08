#include "lpc_wave.h"
#include "lpc_utils.h"

void encoderWav(short *pcm_buf, int length, const char *filename)
{
    FILE *fwav;
    wav_header_t wheader;

    memset(&wheader, '\0', sizeof(wav_header_t));
    get_wav_header(length * sizeof(pcm_buf[0]), &wheader);
    // dump_wav_header(&wheader);
    fwav = fopen(filename, "wb");
    fwrite(&wheader, 1, sizeof(wheader), fwav);
    fwrite(pcm_buf, sizeof(pcm_buf[0]), length, fwav);
    fclose(fwav);
}

void encodeMP3(short *y, int len, int fs, const char *filename)
{
    int read = 0, write, remainLen = len;
    FILE *mp3 = fopen(filename, "wb");

    const int BUFFER_SIZE = 2040;
    unsigned char mp3_buffer[BUFFER_SIZE];
    short *pcm_buffer = y;

    // init lame
    lame_t lame = lame_init();
    lame_set_in_samplerate(lame, fs);
    lame_set_num_channels(lame, 1); // mono
    lame_set_VBR(lame, vbr_default);
    lame_init_params(lame);
    lame_set_quality(lame, 2);

    // lame loop
    do
    {
        //        read = fread(pcm_buffer, numChans*sizeof(short int), PCM_SIZE, pcm);
        pcm_buffer += read;
        read = intMin(remainLen, BUFFER_SIZE);
        remainLen -= read;

        if (read == 0)
            write = lame_encode_flush(lame, mp3_buffer, BUFFER_SIZE);
        else
            //            write = lame_encode_buffer_ieee_double(lame, pcm_buffer, NULL, read, mp3_buffer, BUFFER_SIZE);
            //        	write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, BUFFER_SIZE);
            write = lame_encode_buffer(lame, pcm_buffer, NULL, read, mp3_buffer, BUFFER_SIZE);

        fwrite(mp3_buffer, write, 1, mp3);
    } while (read != 0);

    // clean up
    lame_close(lame);
    fclose(mp3);
}

void get_wav_header(int raw_sz, wav_header_t *wh)
{
    // RIFF chunk
    strncpy(wh->chunk_id, "RIFF", strlen("RIFF"));
    wh->chunk_size = 36 + raw_sz;
    // fmt sub-chunk (to be optimized)
    strncpy(wh->sub_chunk1_id, "WAVEfmt ", strlen("WAVEfmt "));
    wh->sub_chunk1_size = 16;
    wh->audio_format = 1;
    wh->num_channels = 1;
    wh->sample_rate = 16000;
    wh->bits_per_sample = 16;
    wh->block_align = wh->num_channels * wh->bits_per_sample / 8;
    wh->byte_rate = wh->sample_rate * wh->num_channels * wh->bits_per_sample / 8;

    // data sub-chunk
    strncpy(wh->sub_chunk2_id, "data", strlen("data"));
    wh->sub_chunk2_size = raw_sz;
}

void encodeAAC(short *y, int len, int fs, const char *filename)
{
    int bitrate = 64000;
    int channels = 1; // mono

    HANDLE_AACENCODER handle;
    CHANNEL_MODE mode;
    AACENC_InfoStruct info = {0};

    switch (channels)
    {
    case 1:
        mode = MODE_1;
        break;
    case 2:
        mode = MODE_2;
        break;
    case 3:
        mode = MODE_1_2;
        break;
    case 4:
        mode = MODE_1_2_1;
        break;
    case 5:
        mode = MODE_1_2_2;
        break;
    case 6:
        mode = MODE_1_2_2_1;
        break;
    default:
        // printf(stderr, "Unsupported WAV channels %d\n", channels);
        return;
    }

    // params
    int aot = 2;
    int afterburner = 1;
    int eld_sbr = 0;
    int vbr = 0;

    // init
    if (aacEncOpen(&handle, 0, channels) != AACENC_OK)
    {
        fprintf(stderr, "Unable to open encoder\n");
        return;
    }
    if (aacEncoder_SetParam(handle, AACENC_AOT, aot) != AACENC_OK)
    {
        fprintf(stderr, "Unable to set the AOT\n");
        return;
    }
    if (aot == 39 && eld_sbr)
    {
        if (aacEncoder_SetParam(handle, AACENC_SBR_MODE, 1) != AACENC_OK)
        {
            fprintf(stderr, "Unable to set SBR mode for ELD\n");
            return;
        }
    }
    if (aacEncoder_SetParam(handle, AACENC_SAMPLERATE, fs) != AACENC_OK)
    {
        fprintf(stderr, "Unable to set the AOT\n");
        return;
    }
    if (aacEncoder_SetParam(handle, AACENC_CHANNELMODE, mode) != AACENC_OK)
    {
        fprintf(stderr, "Unable to set the channel mode\n");
        return;
    }
    if (aacEncoder_SetParam(handle, AACENC_CHANNELORDER, 1) != AACENC_OK)
    {
        fprintf(stderr, "Unable to set the wav channel order\n");
        return;
    }
    if (vbr)
    {
        if (aacEncoder_SetParam(handle, AACENC_BITRATEMODE, vbr) != AACENC_OK)
        {
            fprintf(stderr, "Unable to set the VBR bitrate mode\n");
            return;
        }
    }
    else
    {
        if (aacEncoder_SetParam(handle, AACENC_BITRATE, bitrate) != AACENC_OK)
        {
            fprintf(stderr, "Unable to set the bitrate\n");
            return;
        }
    }
    if (aacEncoder_SetParam(handle, AACENC_TRANSMUX, TT_MP4_ADTS) != AACENC_OK)
    {
        fprintf(stderr, "Unable to set the ADTS transmux\n");
        return;
    }
    if (aacEncoder_SetParam(handle, AACENC_AFTERBURNER, afterburner) != AACENC_OK)
    {
        fprintf(stderr, "Unable to set the afterburner mode\n");
        return;
    }
    if (aacEncEncode(handle, NULL, NULL, NULL, NULL) != AACENC_OK)
    {
        fprintf(stderr, "Unable to initialize the encoder\n");
        return;
    }
    if (aacEncInfo(handle, &info) != AACENC_OK)
    {
        fprintf(stderr, "Unable to get the encoder info\n");
        return;
    }

    // convert double-array to short-array
    int nRemainBytes = len * sizeof(int16_t);

    // start the encoding
    FILE *out = fopen(filename, "wb");
    int input_size = channels * 2 * info.frameLength;
    uint8_t *input_buf = (uint8_t *)y;
    int16_t *convert_buf = (int16_t *)malloc(input_size);

    // main loop
    int read = 0;
    while (1)
    {
        AACENC_BufDesc in_buf = {0}, out_buf = {0};
        AACENC_InArgs in_args = {0};
        AACENC_OutArgs out_args = {0};
        int in_identifier = IN_AUDIO_DATA;
        int in_size, in_elem_size;
        int out_identifier = OUT_BITSTREAM_DATA;
        int out_size, out_elem_size;
        void *in_ptr, *out_ptr;
        uint8_t outbuf[20480];
        AACENC_ERROR err;

        //read = fread(input_buf, sizeof(*input_buf), input_size, wav);
        input_buf += read;
        read = intMin(nRemainBytes, sizeof(uint8_t) * input_size);
        nRemainBytes -= read;

        for (int i = 0; i < read / 2; i++)
        {
            const uint8_t *in = &input_buf[2 * i];
            convert_buf[i] = in[0] | (in[1] << 8);
        }

        if (read <= 0)
        {
            in_args.numInSamples = -1;
        }
        else
        {
            in_ptr = convert_buf;
            in_size = read;
            in_elem_size = 2;

            in_args.numInSamples = read / 2;
            in_buf.numBufs = 1;
            in_buf.bufs = &in_ptr;
            in_buf.bufferIdentifiers = &in_identifier;
            in_buf.bufSizes = &in_size;
            in_buf.bufElSizes = &in_elem_size;
        }
        out_ptr = outbuf;
        out_size = sizeof(outbuf);
        out_elem_size = 1;
        out_buf.numBufs = 1;
        out_buf.bufs = &out_ptr;
        out_buf.bufferIdentifiers = &out_identifier;
        out_buf.bufSizes = &out_size;
        out_buf.bufElSizes = &out_elem_size;

        if ((err = aacEncEncode(handle, &in_buf, &out_buf, &in_args, &out_args)) != AACENC_OK)
        {
            if (err == AACENC_ENCODE_EOF)
                break;
            printf("Encoding failed\n");
            exit(-1);
        }

        if (out_args.numOutBytes == 0)
            continue;

        fwrite(outbuf, 1, out_args.numOutBytes, out);
    }

    // clean up
    free(convert_buf);
    fclose(out);
    aacEncClose(&handle);
}