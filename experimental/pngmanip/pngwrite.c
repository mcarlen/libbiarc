#include "pngwrite.h"

int
sp_png_get_block_stripe (const unsigned char **rows, int row, int num_rows, void *data)
{
    SPPNGBD *bd = (SPPNGBD *) data;
    int r;
    for (r = 0; r < num_rows; r++) {
        rows[r] = bd->px + (row + r) * bd->rowstride;
    }

    return num_rows;
}

int
sp_png_write_rgb_striped (const char *filename, int width, int height, double xdpi, double ydpi,
               int (* get_rows) (const unsigned char **rows, int row, int num_rows, void *data),
               void *data)
{
    //struct SPEBP *ebp = (struct SPEBP *) data;
    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;
    png_color_8 sig_bit;
    png_text text_ptr[3];
    png_uint_32 r;
    int n;

    if (filename==NULL) return -1;

    /* open the file */

    fp = fopen(filename, "wb");
    if (fp == NULL) return -1;

    /* Create and initialize the png_struct with the desired error handler
     * functions.  If you want to use the default stderr and longjump method,
     * you can supply NULL for the last three parameters.  We also check that
     * the library version is compatible with the one used at compile time,
     * in case we are using dynamically linked libraries.  REQUIRED.
     */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL) {
        fclose(fp);
        return -1;
    }

    /* Allocate/initialize the image information data.  REQUIRED */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        fclose(fp);
        png_destroy_write_struct(&png_ptr, NULL);
        return -1;
    }

    /* Set error handling.  REQUIRED if you aren't supplying your own
     * error hadnling functions in the png_create_write_struct() call.
     */
    if (setjmp(png_jmpbuf(png_ptr))) {
        /* If we get here, we had a problem reading the file */
        fclose(fp);
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return -1;
    }

    /* set up the output control if you are using standard C streams */
    png_init_io(png_ptr, fp);

    /* Set the image information here.  Width and height are up to 2^31,
     * bit_depth is one of 1, 2, 4, 8, or 16, but valid values also depend on
     * the color_type selected. color_type is one of PNG_COLOR_TYPE_GRAY,
     * PNG_COLOR_TYPE_GRAY_ALPHA, PNG_COLOR_TYPE_PALETTE, PNG_COLOR_TYPE_RGB,
     * or PNG_COLOR_TYPE_RGB_ALPHA.  interlace is either PNG_INTERLACE_NONE or
     * PNG_INTERLACE_ADAM7, and the compression_type and filter_type MUST
     * currently be PNG_COMPRESSION_TYPE_BASE and PNG_FILTER_TYPE_BASE. REQUIRED
     */
    png_set_IHDR(png_ptr, info_ptr,
             width,
             height,
             8, /* bit_depth */
	     PNG_COLOR_TYPE_RGB, // RGB_ALPHA for 4th channel
             PNG_INTERLACE_NONE,
             PNG_COMPRESSION_TYPE_BASE,
             PNG_FILTER_TYPE_BASE);

    /* otherwise, if we are dealing with a color image then */
    sig_bit.red = 8;
    sig_bit.green = 8;
    sig_bit.blue = 8;
    /* if the image has an alpha channel then */
    // sig_bit.alpha = 8;
    png_set_sBIT(png_ptr, info_ptr, &sig_bit);

    /* Made by Inkscape comment */
    char KeyText[] = "Software";
    text_ptr[0].key = &KeyText[0];
    char TypeText[] = "homebrew";
    text_ptr[0].text = &TypeText[0];
    text_ptr[0].compression = PNG_TEXT_COMPRESSION_NONE;
    png_set_text(png_ptr, info_ptr, text_ptr, 1);

    /* other optional chunks like cHRM, bKGD, tRNS, tIME, oFFs, pHYs, */
    /* note that if sRGB is present the cHRM chunk must be ignored
     * on read and must be written in accordance with the sRGB profile */

    // maybe test if xdpi and ydpi are > 0
    png_set_pHYs(png_ptr, info_ptr, xdpi / 0.0254 + 0.5, ydpi / 0.0254 + 0.5, PNG_RESOLUTION_METER); 

    /* Write the file header information.  REQUIRED */
    png_write_info(png_ptr, info_ptr);

    /* Once we write out the header, the compression type on the text
     * chunks gets changed to PNG_TEXT_COMPRESSION_NONE_WR or
     * PNG_TEXT_COMPRESSION_zTXt_WR, so it doesn't get written out again
     * at the end.
     */

    /* set up the transformations you want.  Note that these are
     * all optional.  Only call them if you want them.
     */

    /* --- CUT --- */

    /* The easiest way to write the image (you may have a different memory
     * layout, however, so choose what fits your needs best).  You need to
     * use the first method if you aren't handling interlacing yourself.
     */

    png_bytep* row_pointers = (png_bytep*)png_malloc(png_ptr, sizeof(png_bytep)*height); // CARLEN(changed) ebp->sheight];

    r = 0;
    while (r < (png_uint_32)height ) {
        n = get_rows ((const unsigned char **) row_pointers, r, height-r, data);
        if (!n) break;
        png_write_rows (png_ptr, row_pointers, n);
        r += n;
    }

    png_free(png_ptr,row_pointers);

    /* You can write optional chunks like tEXt, zTXt, and tIME at the end
     * as well.
     */

    /* It is REQUIRED to call this to finish writing the rest of the file */
    png_write_end(png_ptr, info_ptr);

    /* if you allocated any text comments, free them here */

    /* clean up after the write, and free any memory allocated */
    png_destroy_write_struct(&png_ptr, &info_ptr);

    /* close the file */
    fclose(fp);

    /* that's it */
    return 1;
}

int
sp_png_write_rgb (const char *filename, const unsigned char *px, int width, int height, double xdpi, double ydpi, int rowstride)
{
    SPPNGBD bd;

    bd.px = px;
    bd.rowstride = rowstride;

    return sp_png_write_rgb_striped (filename, width, height, xdpi, ydpi, sp_png_get_block_stripe, &bd);
}

