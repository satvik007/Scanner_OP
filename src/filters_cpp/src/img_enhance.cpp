/**
 * Implements the dark magic filter code.
 * Tool for enhancing noisy scanned text images. The function of 'imgtxtenh' is to clean/
 * enhance noisy scanned text images, which could be printed or handwritten text. It takes as 
 * input an image and generates as output another image of the same size.
 * 
 * Its a C++ and opencv based implementation of 
 * @copyright Copyright (c) 2012-present, Mauricio Villegas <mauvilsa@upv.es>
 * @link https://github.cppom/mauvilsa/imgtxtenh
 * @license MIT License
 * 
 * This document is part of the project ScanIN. See License for more details.
 * 
 * Author     : Satvik Choudhary
 * Created on : 10 July 2020
*/

#include <opencv/cv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include "img_enhance.hpp"
#include "img_opencv.hpp"
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "imgconv.hpp"
#include "intimg.hpp"
#include "htrprep.hpp"
#include "objs.hpp"
#include "log.hpp"
#include <iostream>
#include <time.h>

int dark_magic_filter_implementation (cv::Mat &src, cv::Mat &img, int gb_enhtype) {

/*** Definitions **************************************************************/
    clock_t time_ini = clock();
    static char tool[] = "imgtxtenh";
    double gb_winW_px = 80;
    double gb_prm = 0.2;
    double gb_slp = 0.5;
    int gb_autosubsamp = 2;
    double gb_prmthr = 0.004;
    double gb_coef_m = 0.0;
    double gb_coef_s = 1.0;
    double gb_small_px = 6;
    double gb_rlsa_px[4] = { 2.4, 2.4, 2.4, 2.4 }; // "-|/\"
    char gb_autoprm = FALSE;
    FILE *logfile = NULL;
    int verbosity = 1;
    
/*** Verify enhancement algorithm *********************************************/
    assert (gb_enhtype >= 1 and gb_enhtype <= 4);

/*** Copy src to img **********************************************************/
    src.copyTo (img);

    logfile = stderr;
    int n;
    gray **msk = NULL;
    gray **pmsk = NULL;

/*** Convert to grayscale *****************************************************/
    if (src.type() == CV_8UC3) {
        cvtColor (img, img, cv::COLOR_BGR2GRAY);
    } else {
        assert (src.type() == CV_8UC1);
    }

/*** Initialize gimg **********************************************************/
    gray **gimg = NULL;
    if( malloc_graym( src.cols, src.rows, &gimg, FALSE) )
        die( "error: unable to reserve memory" );

    if( getpixels_opencv_graym( img, gimg ) )
        die( "error: unable to get pixels" );

/*** Enhance img **************************************************************/
    II1** ii1 = NULL;
    II2** ii2 = NULL;
    II1** cnt = NULL;

    if( gb_autoprm ) {
        if( enhSauvola_sample_prm_graym( gimg, msk, img.cols, img.rows, &ii1, &ii2, &cnt, gb_winW_px, gb_slp, &gb_prm, gb_autosubsamp, gb_prmthr ) )
        die( "error: unable to select the Sauvola mean threshold automatically" );
    }

    enhLocal_graym( gimg, msk, img.cols, img.rows, &ii1, &ii2, &cnt, gb_winW_px, gb_prm, gb_slp, gb_enhtype, gb_coef_m, gb_coef_s );

    if( pmsk == NULL )
        pmsk = msk;
    if( pmsk != NULL ) {
        for( n = img.cols * img.rows - 1; n >= 0; n-- )
        if( pmsk[0][n] == 0 )
            gimg[0][n] = 255;
    }

/*** Small component removal **************************************************/
    int minarea = (int)(gb_small_px + 0.5);
    if( gb_small_px > 0 && minarea <= 0 ) {
        //logger( 2, "skipping small component removal due to low pixel density" );
    } 
    else if( minarea > 0 ) {
        gray **bmap = NULL;
        int **objs = NULL;
        int *area = NULL;
        if( malloc_graym( img.cols, img.rows, &bmap, FALSE ) ||
            malloc_I1m( img.cols, img.rows, &objs, FALSE ) )
        die( "error: unable to reserve memory" );

        for( n=img.cols*img.rows-1; n>=0; n-- )
            bmap[0][n] = gimg[0][n] == 255 ? 255 : 0;

/*** Use RLSA to protect faint strokes ****************************************/
        int irlsa[4];
        for( n=0; n<4; n++ )
        irlsa[n] = gb_rlsa_px[n] < 0 ? 0 : (int)(gb_rlsa_px[n]+0.5) ;

        char rlsa_op = 0;
        rlsa_op = rlsa_op | ( irlsa[0] ? 0x1 : 0 );
        rlsa_op = rlsa_op | ( irlsa[1] ? 0x2 : 0 );
        rlsa_op = rlsa_op | ( irlsa[2] ? 0x4 : 0 );
        rlsa_op = rlsa_op | ( irlsa[3] ? 0x8 : 0 );

        if( ! rlsa_op ) {
            //logger( 2, "RLSA not used" );
        } 
        else {
            //logger( 2, "applying RLSA to protect faint strokes: %d,%d,%d,%d pixels", irlsa[0], irlsa[1], irlsa[2], irlsa[3] );
            if( rlsa4_graym( bmap, img.cols, img.rows, rlsa_op, irlsa, FALSE, bmap ) )
                die( "error: problems applying RLSA" );
        }

        for( n=img.cols*img.rows-1; n>=0; n-- )
            bmap[0][n] = bmap[0][n] == 255 ? 0 : 255;

/*** Get connected components *************************************************/
        int nobjs = 0;
        if( objlabel8( bmap, img.cols, img.rows, objs, &nobjs ) )
            die( "error: problems obtaining connected components" );

        if( nobjs <= 0 ) {
            //logger( 2, "warning: encountered an all white image" );
        }
        else {
            if( malloc_I1v( 2*(nobjs+1), &area, TRUE ) )
                die( "error: unable to reserve memory" );
                
/*** Compute connected component areas ****************************************/
            objinfo( objs, img.cols, img.rows, nobjs, NULL, area, NULL );

            int *rmobj = area+nobjs+1;

/*** Mark small components for removal ****************************************/
            int nrm = 0;
            rmobj[0] = 0;
            for( n=1; n<=nobjs; n++ ) {
                rmobj[n] = area[n]<minarea ? 1 : 0;
                nrm += rmobj[n];
            }

/*** Remove marked components from image **************************************/
            for( n=img.cols*img.rows-1; n>=0; n-- )
                if( rmobj[objs[0][n]] == 1 )
                gimg[0][n] = 255;

            //logger( 1, "removed %d small components from a total of %d (min. area: %d pixels^2)", nrm, nobjs, minarea );
        }

        free( bmap );
        free( objs );
        if( area != NULL )
        free( area );
    }

/*** Write gimg back to img ***************************************************/
    writeimg_opencv (gimg, img);

    double time_end = 1000.0*((double)clock()-time_ini)/((double)CLOCKS_PER_SEC);
    logger( 4, "Total time taken for the operations. (ms): %g", time_end );

    free( gimg );
    free( ii1 );
    free( ii2 );
    if( msk != NULL )
        free( msk );
    if( cnt != NULL )
        free( cnt );
    if( pmsk != NULL && pmsk != msk )
        free( pmsk );

    if( logfile != stdout && logfile != stderr )
        fclose( logfile );

    return SUCCESS;
}