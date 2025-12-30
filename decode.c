#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// User-defined header files
#include "decode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

// Function to open the source image file for decoding
Status open_source_file(DecodeInfo *decInfo)
{
    // Open the source image
    decInfo->fptr_enc_image = fopen(decInfo->enc_image_fname, "r");
    // If the file could not be opened, return e_failure
    if (decInfo->fptr_enc_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->enc_image_fname);

    	return e_failure;
    }
    // If the file opened successfully, return e_success
    return e_success;
}

// Function to open the output file for storing decoded data
Status open_output_file(DecodeInfo *decInfo)
{
    // Open the secret output file
    decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
    // If the file could not be opened, return e_failure
    if (decInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);

    	return e_failure;
    }
    // If the file opened successfully, return e_success
    return e_success;
}

// Function to read and validate command line arguments entered by user after -d
Status read_and_validate_decode_args(int argc, char *argv[], DecodeInfo *decInfo)
{
    // STEP1: Check argv[2] is .bmp or not
    // STEP2: If yes -> Goto STEP3, No -> Print error then return e_failure
    char *ptr = strstr(argv[2], ".bmp");
    if(ptr == NULL)
    {
        return e_failure;
    }

    // STEP3: Allocate memory dynamically and store the arguments into respective structure member
    decInfo->enc_image_fname = malloc(strlen(argv[2]) + 1);
    if (!decInfo->enc_image_fname)
    {
        // If memory allocation fails. return e_failure
        return e_failure;
    }
    strcpy(decInfo->enc_image_fname, argv[2]);

    // STEP4: Assign default name to output file if not provided and store it in structure
    // STEP5: If output file name provided, GoTo STEP6
    char default_name[] = "Decoded_file";
    if(argc < 4)
    {
        printf("INFO: 'Decoded_file' has been taken as default name for output file.\n\n");
        sleep(1);
        decInfo->secret_fname = malloc(strlen(default_name) + 1);
        strcpy(decInfo->secret_fname, default_name);
    }
    else
    {
        // STEP6: Allocate memory according to the provided output file name
        // STEP7: Store the name into the structure
        decInfo->secret_fname = malloc(strlen(argv[3]) + 1);
        if (!decInfo->secret_fname)
        {
            // If memory allocation fails. return e_failure
            return e_failure;
        }
        strcpy(decInfo->secret_fname, argv[3]);
    }

    // If all validations pass, return e_success
    return e_success;
}

// Function to decode a byte from 8 least significant bits 
void decode_lsb_to_byte(char *data, char *image_buffer)
{
    // Temporary variable to hold the decoded byte
    char temp = 0;
    for(int i = 0; i < 8; i++)
    {
        // Get the LSB of the current byte
        int get = image_buffer[i] & 01;
        // Shift the get bit to its correct position
        get = get << i;
        // Replace the bit in temp
        temp = temp | get;
    }
    // Store the decoded byte in the data variable
    *data = temp;
}

// Function to decode hidden data from the encoded image file into the output file
Status decode_image_to_data(DecodeInfo *decInfo)
{
    char arr[8]; 
    char *data;

    for(int i = 0; i < decInfo->size_secret_file; i++)
    {
        // STEP1: Read 8 byte of data from source file and store it one array
        fread(arr, 1, 8, decInfo->fptr_enc_image);

        // STEP2: Call decode_lsb_to_byte(data, arr);
        decode_lsb_to_byte(decInfo->secret_data, arr);

        // STEP3: Write the decoded data to destination file (output.bmp)
        fwrite(decInfo->secret_data, 1, 1, decInfo->fptr_secret);
    }
    // STEP4: Repeat this process for length of data times (size) 

    // Return e_success if all functions are completed successfully
    return e_success;
}

// Function to decode and validate the magic string 
Status decode_magic_string(const char *magic_string, FILE *fptr_enc_image)
{
    // Array to hold 8 bytes read from the image
    char arr[8];
    // Array to store the decoded magic string
    char mgc_string[3];
    char data;

    // Loop to decode each character of the magic string
    for(int i = 0; i < 2; i++)
    {
        // Read 8 bytes from the encoded image, if not successful print error
        if(fread(arr, 1, 8, fptr_enc_image) != 8)
        {
            fprintf(stderr, "Failed to read data from the file!");
            return e_failure;
        }
        // Decode the LSBs of arr into a single character and store in data
        decode_lsb_to_byte(&data, arr);
        // Store decoded character in mgc_string
        mgc_string[i] = data;
    }
    // Add null to the decoded magic string
    mgc_string[2] = '\0';

    // Compare decoded magic string with the original string
    // If matches -> return e_success, else -> e_failure
    if(strcmp(magic_string, mgc_string) == 0)
    {
        return e_success;
    } 
    else
    {
        return e_failure;
    }
}

// Function to decode a 32 bit size value from the LSB of 32 bytes
void decode_size_from_lsb(long *size, char *image_buffer)
{
    // Temporary variable to hold decoded size
    int temp = 0;
    for(int i = 0; i < 32; i++)
    {
        // Extract the LSB from each byte
        int get = image_buffer[i] & 01;
        // Shift the get bit to its correct position
        get = get << i;
        // Replace the get bit in temp variable
        temp = temp | get;
    }
    // Store the decoded size
    *size = temp;
}

// Function to decode the extension size of the secret file
Status decode_extn_size(DecodeInfo *decInfo)
{
    // Array to hold 32 bytes read from the image
    char arr[32];
    // Read 32 bytes from the encoded image; if unsuccessful, print error
    if(fread(arr, 1, 32, decInfo->fptr_enc_image) != 32)
    {
        fprintf(stderr, "Failed to read data from the file!");
        return e_failure;
    }
    // Decode the LSBs from arr to obtain the extension size
    decode_size_from_lsb(&decInfo->extn_file_size, arr);
    // If all operations are done return e_success
    return e_success;
}

// Function to decode the file extension of the secret file
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    // Arrays to hold read bytes and char to store decoded data
    char arr[8], data;
    int read, write;

    // Allocate memory for the decoded extension based on its size
    decInfo->extn_secret_file = malloc(decInfo->extn_file_size);

    // Loop to decode each character of the file extension
    for(int i = 0; i < decInfo->extn_file_size; i++)
    {
        // STEP1: Read 8 byte of data from source file and store it one array
        read = fread(arr, 1, 8, decInfo->fptr_enc_image);
        // STEP2: Call decode_byte_to_lsb(data[0], arr);
        decode_lsb_to_byte(&data, arr);
        // STEP3: Store decoded character in extension structure variable
        decInfo->extn_secret_file[i] = data;
    }

    // Resize the filename to add the decoded file extension
    decInfo->secret_fname = realloc(decInfo->secret_fname, strlen(decInfo->secret_fname) + decInfo->extn_file_size);

    // Add the decoded file extension to the secret filename if not already present
    if(strstr(decInfo->secret_fname, decInfo->extn_secret_file) == NULL)
    {
        decInfo->secret_fname = strcat(decInfo->secret_fname, decInfo->extn_secret_file);
    }

    // Return e_success if all functions are completed
    return e_success;
}

// Function to decode the size of the secret file from the encoded image
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    // Array to hold 32 bytes read from the image
    char arr[32];

    // Read 32 bytes from the encoded image; if unsuccessful, print error
    if(fread(arr, 1, 32, decInfo->fptr_enc_image) != 32)
    {
        fprintf(stderr, "Failed to read data from the file!");
        return e_failure;
    }
    // Decode the LSBs from arr to obtain the secret file size
    decode_size_from_lsb(&decInfo->size_secret_file, arr);

    // Return e_success if all functions are completed
    return e_success;
}

// Function to check if decoding was successful by comparing decoded and original file sizes
Status check_successful_decoding(DecodeInfo *decInfo)
{
    // Set the output secret file pointer to the end of the file
    fseek(decInfo->fptr_secret, 0, SEEK_END);
    // Use ftell to get the end position of the file 
    decInfo->size_output_file = ftell(decInfo->fptr_secret);

    // If the decoded file size matches the original secret file size, return e_success
    if(decInfo->size_secret_file == decInfo->size_output_file)
    {
        return e_success;
    }
    // Return e_failure if sizes do not match
    return e_failure;
}


Status do_decoding(DecodeInfo *decInfo)
{
    // STEP1: Call open_dfiles function
    sleep(1); // Use sleep to delay the display of next printf
    if(open_source_file(decInfo) == e_success)
    {
        printf("INFO: Source file is opened successfully.\n\n");
    }
    else
    {
        printf("INFO: Source file could not be opened!\n\n");
        return e_failure;
    }

    sleep(1);
    // Set the file pointer to encoded image to after the header part
    fseek(decInfo->fptr_enc_image, 54, SEEK_SET);
    // Call decode_magic_string()
    // Check returned e_success or e_failure
    // if not e_success print error msg, then return e_failure
    if(decode_magic_string(MAGIC_STRING, decInfo->fptr_enc_image) == e_success)
    {
        printf("INFO: The magic string has successfully matched.\n\n");
    }
    else
    {
        printf("INFO: The magic string does not match!\n\n");
        return e_failure;
    }

    sleep(1);
    // Call decode_extn_size()
    // Check returned e_success or e_failure
    // if not e_success print error msg, then return e_failure
    if(decode_extn_size(decInfo) == e_success)
    {
        printf("INFO: The size of extension has successfully been decoded.\n\n");
    }
    else
    {
        printf("INFO: The size of extension could not be decoded!\n\n");
        return e_failure;
    }

    sleep(1);
    // Call decode_secret_file_extn()
    // Check returned e_success or e_failure
    // if not e_success print error msg, then return e_failure
    if(decode_secret_file_extn(decInfo) == e_success)
    {
        printf("INFO: The extension has successfully been decoded.\n\n");
    }
    else
    {
        printf("INFO: The extension could not be decoded!\n\n");
        return e_failure;
    }

    sleep(1);
    // Call open_output_file()
    // Check returned e_success or e_failure
    // if not e_success print error msg, then return e_failure
    if(open_output_file(decInfo) == e_success)
    {
        printf("INFO: Output file is opened successfully.\n\n");
    }
    else
    {
        printf("INFO: Output file could not be opened!\n\n");
        return e_failure;
    }

    sleep(1);
    // Call decode_secret_file_size()
    // Check returned e_success or e_failure
    // if not e_success print error msg, then return e_failure
    if(decode_secret_file_size(decInfo) == e_success)
    {
        printf("INFO: The size of secret file has successfully been decoded.\n\n");
    }
    else
    {
        printf("INFO: The size of secret file could not be decoded!\n\n");
        return e_failure;
    }

    sleep(1);
    // Call decode_image_to_data()
    // Check returned e_success or e_failure
    // if not e_success print error msg, then return e_failure
    if(decode_image_to_data(decInfo) == e_success)
    {
        printf("INFO: The data of secret file has successfully been decoded.\n\n");
    }
    else
    {
        printf("INFO: The data of secret file could not be decoded!\n\n");
        return e_failure;
    }

    sleep(1);
    // Call check_successful_decoding()
    // Check returned e_success or e_failure
    // if not e_success print error msg, then return e_failure
    if(check_successful_decoding(decInfo) == e_success)
    {
        char str[] = "INFO: Decoding Completed Successfully.";
        int width = strlen(str);
        for(int i = 0; i < width; i++)
        {
            printf("-");
        }
        printf("\n");

        printf("%s\n", str);

        for(int i = 0; i < width; i++)
        {
            printf("-");
        }
        printf("\n");
    }
    else
    {
        return e_failure;
    }

    // return e_success if all functions have been executed successfully
    return e_success;
}