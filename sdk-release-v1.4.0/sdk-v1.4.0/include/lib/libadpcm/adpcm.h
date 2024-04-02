/*
** adpcm.h - include file for adpcm coder.
**
support:
SampleRate： 16000
BitsPerSample: 16bits

** Version 1.0, 7-Jul-92.
*/

#ifndef ADPCM_H
#define ADPCM_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct adpcm_state {
    short	valprev;	/* Previous output value */
    char	index;		/* Index into stepsize table */
} adpcm_state_t;

int adpcm_encoder(const short *in_pcm_data, unsigned char *out_adpcm_data, int pcm_data_len, adpcm_state_t *state);
int adpcm_decoder(const unsigned char *in_adpcm_data, short *out_pcm_data, int adpcm_data_len, adpcm_state_t *state);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* ADPCM_H*/

