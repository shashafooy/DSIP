#pragma once


typedef struct
{
	// signal (1)
	// image (2)
	// video (3)
	int ndim;

	// signal (1= mono , 2= stereo , etc .)
	// grayscale image / video (1)
	// color image / video (3)
	int nchan;

	// signal = > length
	// image or video = > number rows
	int dim0;

	// signal = > if audio , then dim1 = sample rate
	// image or video = > number columns
	int dim1;

	// signal = > 0
	// image = > 0
	// video = > number_frames
	int dim2;

} dsp_file_header;
