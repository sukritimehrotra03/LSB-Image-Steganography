#ifndef ENCODE_H
#define ENCODE_H

#include "types.h" // Contains user defined types

/* 
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname;      // Source file name (beautiful.bmp)
    FILE *fptr_src_image;       // File pointer of beautiful.bmp
    uint image_capacity;        // (unsigned int) Source file size
    // uint bits_per_pixel;     // 24 bits per pixel (not used)
    char image_data[MAX_IMAGE_BUF_SIZE];   // To store image data (8 byte at once)
    uint src_image_size;        // Source Image size

    /* Secret File Info */
    char *secret_fname;         // Secret file name (secret.txt)
    FILE *fptr_secret;          // File pointer of secret.txt
    char extn_secret_file[MAX_FILE_SUFFIX];   // Extension of secret file
    char secret_data[MAX_SECRET_BUF_SIZE];    // To store secret data (1 byte at a time)
    long size_secret_file;      // Secret file size

    /* Stego Image Info */
    char *stego_image_fname;    // Output Image file name
    FILE *fptr_stego_image;     // File pointer of output image
    uint output_image_size;     // Stego Image size

} EncodeInfo;  // Datatype of the structure


/* Encoding function prototype */

/* Validate number of args entered */
Status validate_args(int argc, char *argv[]);

/* Check operation type */
OperationType check_operation_type(char *argv);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo *encInfo);

/* check capacity */
Status check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Get secret file extension size */
uint get_secret_extension_size(EncodeInfo *encInfo);

/* Copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode size to LSB of source image data*/
void encode_size_to_lsb(int size, char *image_buffer);

/* Encode the size of extension of secret file */
Status encode_secret_file_extn_size(int extn_size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode secret file data*/
Status encode_secret_file_data(EncodeInfo *encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte into LSB of image data array */
void encode_byte_to_lsb(char data, char *image_buffer);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(EncodeInfo *encInfo);

/* Check if encoding was successful */
Status check_successful_encoding(EncodeInfo *encInfo);

#endif