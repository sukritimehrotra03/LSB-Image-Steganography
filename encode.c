#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// User-defined header files
#include "encode.h" 
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    // printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    // printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// Function to read and validate command line arguments entered by user after -e
Status read_and_validate_encode_args(int argc, char *argv[], EncodeInfo *encInfo)
{
    // STEP1: Check argv[2] is .bmp or not
    // STEP2: If yes -> Goto STEP3, No -> Print error then return e_failure
    char *ptr = strstr(argv[2], ".bmp");
    if(ptr == NULL)
    {
        return e_failure;
    }

    // STEP3: Check argv[3] is .txt file or not
    // STEP4: If yes -> Goto STEP5, No -> Print error then return e_failure
    ptr = strstr(argv[3], ".txt");
    if(ptr == NULL)
    {
        return e_failure;
    }
    
    // Allocate memory dynamically
    encInfo->src_image_fname = malloc(strlen(argv[2]) + 1);
    encInfo->secret_fname = malloc(strlen(argv[3]) + 1);

    if (!encInfo->src_image_fname || !encInfo->secret_fname)
    {
        // If memory allocation fails
        return e_failure;
    }

    // STEP5: Store all the arguments into respective structure member
    strcpy(encInfo->src_image_fname, argv[2]);
    strcpy(encInfo->secret_fname, argv[3]);

    // STEP6: Assign default name to output file if not provided and store it in structure
    // STEP7: If output file name provided, GoTo STEP8
    char default_bmp_name[] = "Encoded_Image.bmp";
    if(argc < 5)
    {
        printf("INFO: 'Encoded_Image.bmp' has been taken as default name for output file.\n\n");
        sleep(1);
        encInfo->stego_image_fname = malloc(strlen(default_bmp_name) + 1);
        if (!encInfo->stego_image_fname)
        {
            // If memory allocation fails
            return e_failure;
        }  
        strcpy(encInfo->stego_image_fname, default_bmp_name);
    }
    else
    {
        // STEP8: Check argv[4] is .bmp file or not
        // STEP9: If yes -> Goto STEP10, No -> Print error then return e_failure
        ptr = strstr(argv[4], ".bmp");
        if(ptr == NULL)
        {
            return e_failure;
        }

        // STEP10: Allocate memory according to argv[4]
        encInfo->stego_image_fname = malloc(strlen(argv[4]) + 1);

        if (!encInfo->stego_image_fname)
        {
            // If memory allocation fails return e_failure
            return e_failure;
        } 
        // STEP11: Store argv[4] into structure
        strcpy(encInfo->stego_image_fname, argv[4]);
    }

    // If all steps are done correctly return e_success
    return e_success;
}

// Function to find the size of any file
uint get_file_size(FILE *fptr)
{
    // Set the file pointer to the last character
    fseek(fptr, 0, SEEK_END);
    // Use ftell to find the position of the pointer
    return ftell(fptr);
}

// Function to find the size of extension of the secret data file
uint get_secret_extension_size(EncodeInfo *encInfo)
{
    // Store the address where the file extension starts in a pointer
    char *extn = strchr(encInfo->secret_fname, '.');
    // Store the file extension into the structure variable using previous pointer
    strcpy(encInfo->extn_secret_file, extn);
    // Return the length of the extension
    return strlen(encInfo->extn_secret_file);
}

// Function to check if the input image has enough capacity to store the data that needs to be encoded
Status check_capacity(EncodeInfo *encInfo)
{
    // STEP1: Call the get_image_size_for_bmp() function for getting the size of the bmp image
    if(encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image))
    {
        printf("INFO: Source Image size obtained successfully.\n\n");
    }
    else
    {
        printf("INFO: Source Image size could not be obtained!\n\n");
        return e_failure;
    }
    // STEP2: Call the get_file_size() function to get the size of secret file 
    sleep(1);
    if(encInfo->size_secret_file = get_file_size(encInfo->fptr_secret))
    {
        fseek(encInfo->fptr_secret, 0, SEEK_SET);
        printf("INFO: Secret File size obtained successfully.\n\n");
    }
    else
    {
        fseek(encInfo->fptr_secret, 0, SEEK_SET);
        printf("INFO: Secret File size could not be obtained!\n\n");
        return e_failure;
    }

    // STEP3: Find the size of secret file extension (from . extract string then find size)
    sleep(1);
    uint extn_size;
    if(extn_size = get_secret_extension_size(encInfo))
    {
        printf("INFO: Extension size obtained successfully.\n\n");
    }
    else
    {
        printf("INFO: Extension size could not be obtained!\n\n");
        return e_failure;
    }

    // STEP4: Check if the bmp file has enough capacity to hold all the data
    // size_of_bmp_file > (16 + 32 + (size_of_extn * 8) + 32 + (size_of_secret_file * 8) + 54 + 1)
    uint total_size = 16 + 32 + (extn_size * 8) + 32 + (encInfo->size_secret_file * 8) + 54 + 1;

    // STEP5: If enough capacity -> return e_success, else -> return e_failure
    if(encInfo->image_capacity > total_size)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Function to copy the header from the input bmp file to output bmp file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // Bring the file pointer of source image to the first character
    rewind(fptr_src_image);
    // STEP1: Read 54 bytes of data from source image
    unsigned char header[54];
    int read = fread(header, 1, 54, fptr_src_image); 

    // STEP2: Write the data to destination image
    int write = fwrite(header, 1, 54, fptr_stego_image);

    // STEP3: Check if fread and fwrite return 54 or not (data copied successfully)
    // If yes -> return e_success, else -> return e_failure
    if(read == 54 && write == 54)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Generic function to encode each byte of secret data to LSB of 8 bytes of data from the source file
void encode_byte_to_lsb(char data, char *image_buffer)
{
    // encode_process
    // Repeat this process for 8 times
    for(int i = 0; i < 8; i++)
    {
        // STEP1: Clear LSB bit of image_buffer[loop_var]
        image_buffer[i] = image_buffer[i] & (~01);
        // STEP2: Get one bit from data
        int get = data & (1 << i); 
        // STEP3: Shift the get bits to the required position
        get = (unsigned)get >> i;
        // STEP4: Replace the bit
        image_buffer[i] = image_buffer[i] | get;
    }
}

// Generic function to encode data to image
Status encode_data_to_image(const char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    /* Execute as many times as the length of data */
    // Read 8 bytes of data from beautiful.bmp
    // 8 byte and # pass to the function -> encode_byte_to_lsb()
    // Write 8 byte of data to output.bmp

    char arr[8];
    int read, write;

    for(int i = 0; i < size; i++)
    {
        // STEP1: Read 8 byte of data from source file and store it one array
        read = fread(arr, 1, 8, fptr_src_image);

        // STEP2: Call encode_byte_to_lsb(data[0], arr);
        encode_byte_to_lsb(data[i], arr);

        // STEP3: Write the encoded data to destination file (output.bmp)
        write = fwrite(arr, 1, 8, fptr_stego_image);
    }
    // STEP4: Repeat this process for length of data times (size) 
    
    // STEP5: If read and write both equal to 8 return e_success, else -> return e_failure
    if(read == 8 && write == 8)
    {
        return e_success;
    }
    return e_failure;
}

// Function to encode the magic string into the output image
Status encode_magic_string(const char *magic_string, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // Call the generic function that encodes the data to output image with magic string as an argument
    if(encode_data_to_image(magic_string, strlen(magic_string), fptr_src_image, fptr_stego_image) == e_success)
    {
        // Return e_success if the function is performed successfully
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Generic function to encode 32 bits of secret data size to LSB of 32 bytes of data from the source file
void encode_size_to_lsb(int size, char *image_buffer)
{
    // Repeat this process 32 times
    for(int i = 0; i < 32; i++)
    {    
        // STEP1: Clear LSB bit of image_buffer[loop_var]
        image_buffer[i] = image_buffer[i] & (~01);
        // STEP2: Get one bit from data
        int get = size & (1 << i);
        // STEP3: Shift the get bit to the required position
        get = (unsigned)get >> i;
        // STEP4: Replace the bit
        image_buffer[i] = image_buffer[i] | get;
    }
}

// Function to encode the size of secret file extension into the destination file 
Status encode_secret_file_extn_size(int extn_size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[32];
    int read, write;
    // STEP1: Read 32 bytes of data from source file
    read = fread(arr, 1, 32, fptr_src_image);
    // STEP2: Call encode_size_to_lsb(arr, file_extn)
    encode_size_to_lsb(extn_size, arr);
    // STEP3: Write encoded data to destination file (output.bmp)
    write = fwrite(arr, 1, 32, fptr_stego_image);
    // If the data read and written are both 32 return e_success, else -> return e_failure
    if (read == 32 && write == 32)
    {
        return e_success;
    }
    return e_failure;
}

// Function to encode secret file extension 
Status encode_secret_file_extn(const char *file_extn, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    // STEP1: Call encode_data_to_image() with arguments as file extension and its length 
    // STEP2: If its successful -> return e_Success, else -> return e_failure
    if(encode_data_to_image(file_extn, strlen(file_extn), fptr_src_image, fptr_stego_image) == e_success)
    {
        return e_success;
    }
    return e_failure;
}

// Function to encode the size of secret file into destination image
Status encode_secret_file_size(long file_size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char arr[32];
    int read, write;
    // STEP1: Read 32 bytes of data from source file
    read = fread(arr, 1, 32, fptr_src_image);
    // STEP2: Call encode_size_to_lsb(arr, file_extn)
    encode_size_to_lsb(file_size, arr);
    // STEP3: Write encoded data to destination file (output.bmp)
    write = fwrite(arr, 1, 32, fptr_stego_image);
    // STEP4: If its successful -> return e_success, else -> return e_failure
    if (read == 32 && write == 32)
    {
        return e_success;
    }
    return e_failure;
}

// Function to encode the secret file data to the destination image
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    // STEP1: Create a character array of the size of the secret file
    char secret_data[encInfo -> size_secret_file];
    // STEP2: Read the secret data from secret file
    fread(secret_data, encInfo -> size_secret_file , 1, encInfo -> fptr_secret);
    // STEP3: Call the encode_data_to_image() function with secret data and its size as arguments
    // STEP4: If its successful -> return e_success, else -> return e_failure
    if(encode_data_to_image(secret_data, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
    return e_failure;
}

// Function to copy the remaining data from source image to destination image
Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    char ch;
    // Read data from source image
    while(fread(&ch, 1, 1, encInfo->fptr_src_image))
    {   
        // Write data to the destination image  
        fwrite(&ch, 1, 1, encInfo->fptr_stego_image);
    }
    // Return e_success after all the functions have been performed
    return e_success;
}

// Function to check whether encoding was successful
Status check_successful_encoding(EncodeInfo *encInfo)
{
    // STEP1: Calculate the size of the destination file
    fseek(encInfo->fptr_stego_image, 0, SEEK_END);
    encInfo->output_image_size = ftell(encInfo->fptr_stego_image);

    // STEP2: Calculate the size of source file
    fseek(encInfo->fptr_src_image, 0, SEEK_END);
    encInfo->src_image_size = ftell(encInfo->fptr_src_image);

    // STEP3: Compare the size of both files
    // if equal -> return e_success, else -> return e_failure
    if(encInfo->output_image_size == encInfo->src_image_size)
    {
        return e_success;
    }
    return e_failure;
}

// Function to perform all the steps of encoding one by oone
Status do_encoding(EncodeInfo *encInfo)
{
    // STEP1: Call open_files function
    sleep(1);
    if(open_files(encInfo) == e_success)
    {
        printf("INFO: All files are opened successfully.\n\n");
    }
    else
    {
        printf("INFO: Files could not be opened!\n\n");
        return e_failure;
    }

    // STEP2: Call check_capacity()
    // STEP3: Check returned e_success or e_failure
    // STEP4: if_e_success -> Goto STEP5, else -> print error msg, then return e_failure
    sleep(1);
    if(check_capacity(encInfo) == e_success)
    {
        printf("INFO: The source image has enough capacity to be encoded.\n\n");
    }
    else
    {
        printf("INFO: Source Image does not have enough capacity!\n\n");
        return e_failure;
    }

    // STEP5: Call copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)
    // STEP6: Check returned e_success or e_failure
    // STEP7: if_e_success -> Goto STEP8, else -> print error msg, then return e_failure

    sleep(1);
    if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: The header has been successfully copied.\n\n");
    }
    else
    {
        printf("INFO: The header could not be copied!\n\n");
        return e_failure;
    }

    // STEP8: Call encode_magic_string(MAGIC_STRING, /*File pointers*/)
    // STEP9: Check returned e_success or e_failure
    // STEP10: if_e_success -> Goto STEP11, else -> print error msg, then return e_failure
 
    sleep(1);
    if(encode_magic_string(MAGIC_STRING, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: The magic string has been successfully encoded.\n\n");
    }
    else
    {
        printf("INFO: The magic string could not be encoded!\n\n");
        return e_failure;
    }

    // STEP11: Call encode_secret_file_extn_size(extn_size, /*File pointers*/)
    // STEP12: Check returned e_success or e_failure
    // STEP13: if_e_success -> Goto STEP14, else -> print error msg, then return e_failure
    sleep(1);
    uint extn_size = get_secret_extension_size(encInfo);
    if(encode_secret_file_extn_size(extn_size, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: The secret file extension size has been successfully encoded.\n\n");
    }
    else
    {
        printf("INFO: The secret file extension size could not be encoded!\n\n");
        return e_failure;
    }

    // STEP14: Call encode_secret_file_extn(extn, /*File pointers*/)
    // STEP15: Check returned e_success or e_failure
    // STEP16: if_e_success -> Goto STEP17, else -> print error msg, then return e_failure
    sleep(1);
    if(encode_secret_file_extn(encInfo->extn_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: The secret file extension has been successfully encoded.\n\n");
    }
    else
    {
        printf("INFO: The secret file extension could not be encoded!\n\n");
        return e_failure;
    }

    // STEP17: Call encode_secret_file_size(file_size, /*File pointers*/)
    // STEP18: Check returned e_success or e_failure
    // STEP19: if_e_success -> Goto STEP20, else -> print error msg, then return e_failure
    sleep(1);
    if(encode_secret_file_size(encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("INFO: The secret file size has been successfully encoded.\n\n");
    }
    else
    {
        printf("INFO: The secret file size could not be encoded!\n\n");
        return e_failure;
    }

    // STEP20: Call encode_secret_file_data(encInfo)
    // STEP21: Check returned e_success or e_failure
    // STEP22: if_e_success -> Goto STEP23, else -> print error msg, then return e_failure
    sleep(1);
    if(encode_secret_file_data(encInfo) == e_success)
    {
        printf("INFO: The secret file data has been successfully encoded.\n\n");
    }
    else
    {
        printf("INFO: The secret file data could not be encoded!\n\n");
        return e_failure;
    }

    // STEP23: Call copy_remaining_img_data(fptr_src_image, fptr_stego_image)
    // STEP24: Check returned e_success or e_failure
    // STEP25: if_e_success -> Goto STEP26, else -> print error msg, then return e_failure
    sleep(1);
    if(copy_remaining_img_data(encInfo) == e_success)
    {
        printf("INFO: The remaining data has been successfully copied.\n\n");
    }
    else
    {
        printf("INFO: The remaining data could not be copied!\n\n");
        return e_failure;
    }

    // STEP26: Call check_successful_encoding(encInfo)
    // STEP27: Check returned e_success or e_failure
    // STEP28: if_e_success -> Goto STEP29, else -> print error msg, then return e_failure
    if(check_successful_encoding(encInfo) == e_success)
    {
        char str[] = "INFO: Enoding Completed Successfully.";
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
    
    // STEP29: Return e_success if all the functions have been executed successfully
    return e_success;
}