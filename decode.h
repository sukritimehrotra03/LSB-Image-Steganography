#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * decoding secret file from encoded Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Source Image info */
    char *enc_image_fname;      // Source file name (encoded_image.bmp)
    FILE *fptr_enc_image;       // File pointer of encoded_image.bmp
    char image_data[MAX_IMAGE_BUF_SIZE];   // To store image data (8 byte at once)

    /* Secret File Info */
    char *secret_fname;         // Output decoded file name (secret.txt)
    FILE *fptr_secret;          // File pointer of secret.txt
    long extn_file_size;        // Extension size of secret file  
    char *extn_secret_file;     // Extension of secret file
    char secret_data[MAX_SECRET_BUF_SIZE];    // To store secret data (1 byte at a time)
    long size_secret_file;      // Original Secret file size
    long size_output_file;      // Size of decoded output file

} DecodeInfo;  // Datatype of the structure


/* Decoding function prototype */

/* Read and validate decode args from argv */
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo);

/* Get File pointers for encoded file */
Status open_source_file(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(const char *magic_string, FILE *fptr_enc_image);

/* Decode LSB from a byte of image data array */
void decode_lsb_to_byte(char *data, char *image_buffer);

/* Decode function, which does the real decoding */
Status decode_image_to_data(DecodeInfo *decInfo);

/* Decode extension size */
Status decode_extn_size(DecodeInfo *decInfo);

/* Decode size from LSB of source image data */
void decode_size_from_lsb(long *size, char *image_buffer);

/* Decode secret file extenstion */
Status decode_secret_file_extn(DecodeInfo *decInfo);

/* Open output dedcoding file and collect file pointers*/
Status open_output_file(DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Check if decoding was successful */
Status check_successful_decoding(DecodeInfo *decInfo);

#endif