/* ----------------------------- MNI Header -----------------------------------
@NAME       : mincwindow
@INPUT      : argc, argv - command line arguments
@OUTPUT     : (none)
@RETURNS    : status
@DESCRIPTION: Program to limit voxel values to a given range
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 10, 1994 (Peter Neelin)
@MODIFIED   : $Log: mincwindow.c,v $
@MODIFIED   : Revision 1.1  1994-01-11 15:02:56  neelin
@MODIFIED   : Initial revision
@MODIFIED   :
---------------------------------------------------------------------------- */

#ifndef lint
static char rcsid[]="$Header: /private-cvsroot/minc/progs/mincwindow/mincwindow.c,v 1.1 1994-01-11 15:02:56 neelin Exp $";
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <minc.h>
#include <ParseArgv.h>
#include <time_stamp.h>
#include <minc_def.h>
#include <voxel_loop.h>

#ifndef public
#  define public
#endif

/* Structure for window information */
typedef struct {
   double minimum;
   double maximum;
} Window_Data;

/* Function prototypes */
public void do_window(void *voxel_data, long nvoxels, double *data);

/* Argument variables */
int clobber = NC_NOCLOBBER;

/* Argument table */
ArgvInfo argTable[] = {
   {"-clobber", ARGV_CONSTANT, (char *) NC_CLOBBER, (char *) &clobber,
       "Overwrite existing file."},
   {"-noclobber", ARGV_CONSTANT, (char *) NC_NOCLOBBER, (char *) &clobber,
       "Don't overwrite existing file (default)."},
   {NULL, ARGV_END, NULL, NULL, NULL}
};
/* Main program */

public int main(int argc, char *argv[])
{
   char *infile, *outfile;
   char *arg_string;
   int inmincid, outmincid;
   Window_Data window_data;
   char *endptr;

   /* Save time stamp and args */
   arg_string = time_stamp(argc, argv);

   /* Get arguments */
   if (ParseArgv(&argc, argv, argTable, 0) || 
       (argc < 5) || (argc > 5)) {
      (void) fprintf(stderr, 
      "Usage: %s [options] <in.mnc> <out.mnc> <min> <max>\n",
                     argv[0]);
      exit(EXIT_FAILURE);
   }
   infile = argv[1];
   outfile = argv[2];
   window_data.minimum = strtod(argv[3], &endptr);
   if ((endptr == argv[3]) || (*endptr != NULL)) {
      (void) fprintf(stderr, "Cannot get min value from %s\n", argv[3]);
      exit(EXIT_FAILURE);
   }
   window_data.maximum = strtod(argv[4], &endptr);
   if ((endptr == argv[4]) || (*endptr != NULL)) {
      (void) fprintf(stderr, "Cannot get max value from %s\n", argv[4]);
      exit(EXIT_FAILURE);
   }

   /* Open input file */
   inmincid = miopen(infile, NC_NOWRITE);

   /* Create output file */
   outmincid = micreate(outfile, clobber);

   /* Do loop */
   voxel_loop(inmincid, outmincid, arg_string, 
              do_window, (void *) &window_data);

   exit(EXIT_SUCCESS);
}

/* ----------------------------- MNI Header -----------------------------------
@NAME       : do_window
@INPUT      : voxel_data - pointer to structure containing windowing info
              nvoxels - number of voxels to operate on
              data - array of voxels
@OUTPUT     : data
@RETURNS    : (nothing)
@DESCRIPTION: Routine to loop through an array of voxels and limit the 
              range of values.
@METHOD     : 
@GLOBALS    : 
@CALLS      : 
@CREATED    : January 11, 1994 (Peter Neelin)
@MODIFIED   : 
---------------------------------------------------------------------------- */
public void do_window(void *voxel_data, long nvoxels, double *data)
{
   Window_Data *window_data;
   long ivox;

   /* Get pointer to window info */
   window_data = (Window_Data *) voxel_data;

   /* Loop through the voxels */
   for (ivox=0; ivox < nvoxels; ivox++) {
      if (data[ivox] < window_data->minimum)
         data[ivox] = window_data->minimum;
      if (data[ivox] > window_data->maximum)
         data[ivox] = window_data->maximum;
   }

   return;
}