


#include <afxwin.h>  
#include <locale.h>  
//#include <stdio.h>  

#include <math.h> 
#include "test.h"
#include "../../src/blepo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif





using namespace blepo;
using namespace blepo_ex;


int main(int argc, const char* argv[], const char* envp[])
{
	
	HMODULE hModule = ::GetModuleHandle(NULL);
	if (hModule == NULL || !AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
	{
		printf("Fatal Error: MFC initialization failed (hModule = %x)\n", hModule);
		return 1;
	}


	
	//Command line parameters number check
	printf("argc = %d\n", argc);
	for (int i=0; i<argc ; i++) 
	{
		printf("argv[%d]=%s\n\n", i, argv[i]);
	}
	
	if( argc < 4)	
	{
		printf("\nError: lack enough command parameters \n");
		printf("Act: Program halt, Please close \n");
		EventLoop();
		return 0;
		//third_commandpara_specified=1;		
	}
	else if(argc > 4)
	{
		printf("\nWarning: more command parameters than use \n");
	}
	

	/*
	a.	Reads 3command-line parameters, which we will call left-filename, right-filename, and max-disparity.  
	*/
	CString path_blepo_images ="../../images/";
	CString filename_input_1=argv[1];//loading file1
	CString filename_input_2=argv[2];//loading file2
	CString filename_input_3=argv[3];//loading value




	CString filename_forloading_origin_1 = path_blepo_images + filename_input_1;
	CString filename_forloading_origin_2 = path_blepo_images + filename_input_2;
	//CString filename_forloading_origin_3 = path_blepo_images + filename_input_3;

	int status_filename_loading_error=0;
	int status_filename_1=initial_filename_recognize(path_blepo_images, filename_input_1);
	int status_filename_2=initial_filename_recognize(path_blepo_images, filename_input_2);
	//int status_filename_3=initial_filename_recognize(path_blepo_images, filename_input_3);


	
	if( status_filename_1==1 )
	{
		status_filename_loading_error=1;
		printf("\nError: File cannot be found : %s \n",filename_forloading_origin_1 );	
			
	}
	
	if( status_filename_2==1 )
	{
		status_filename_loading_error=1;
		printf("\nError: File cannot be found : %s \n",filename_forloading_origin_2 );	
		
	}

	/*if( status_filename_3==1 )
	{
		status_filename_loading_error=1;
		printf("\nError: File cannot be found : %s \n",filename_forloading_origin_3 );	
		
	}*/



	if( 1==status_filename_loading_error )
	{
		printf("Act: Program halt, Please close \n");
		EventLoop();
		return 0;
	}



	
	
	//b.	Load the two (grayscale or color) images.

	ImgBgr img_loaded_left_bgr;//img_loaded_origin_1_bgr
	ImgBgr img_loaded_right_bgr;//img_loaded_origin_2_bgr
	//ImgBgr img_loaded_maxdisparity_bgr;//img_loaded_origin_3_bgr

	ImgGray img_loaded_left_g;//img_loaded_origin_1_bgr
	ImgGray img_loaded_right_g;//img_loaded_origin_2_bgr
	//ImgGray img_loaded_maxdisparity_g;//img_loaded_origin_3_bgr




	//Figure and title
	Figure fig_loaded_left;
	Figure fig_loaded_right;
	//Figure fig_loaded_maxdisparity_bgr;


	fig_loaded_left.SetTitle("loaded_left_bgr");
	fig_loaded_right.SetTitle("loaded_right_bgr");
	//fig_loaded_maxdisparity_bgr.SetTitle("img_loaded_maxdisparity_g");
	

	//load and convert left filename
	Load(filename_forloading_origin_1, &img_loaded_left_bgr);
	Convert(img_loaded_left_bgr, &img_loaded_left_g);
	fig_loaded_left.Draw(img_loaded_left_bgr);

	//load and convert right filename
	Load(filename_forloading_origin_2, &img_loaded_right_bgr);
	Convert(img_loaded_right_bgr, &img_loaded_right_g);
	fig_loaded_right.Draw(img_loaded_right_bgr);

	//load and convert max_disp_value
	int value_max_disparity=_ttoi(filename_input_3);
	printf("Note: Input value_max_disparity is %d \n",value_max_disparity);
	
	if(value_max_disparity <= 0 ||value_max_disparity>=255 )
	{
		
		
		value_max_disparity=150;
		printf("Note: Lack of value_max_disparity or too large, using initial value %d\n",value_max_disparity);
	}


	printf("\n Start: \n");



	/*
	c.	Perform block-based matching of the two images.  
	*/
	ImgGray img_left_dispartity_map_g;
	ImgInt img__left_dispartity_map_int;

	ImgGray img_right_dispartity_map_g;
	ImgInt img_right_dispartity_map_int;

	if (
		compute_3D_array_of_dissimilarities(
			img_loaded_left_g, 
			img_loaded_right_g, 
			0, 
			value_max_disparity,
			&img_left_dispartity_map_g,
			&img_right_dispartity_map_g) 
		==0 )	printf("c1 \n");
	
	
	Convert(img_left_dispartity_map_g,&img__left_dispartity_map_int);
	Convert(img_right_dispartity_map_g,&img_right_dispartity_map_int);


	Figure fig_left_dispartity_map_g;
	fig_left_dispartity_map_g.SetTitle("left_dispartity_map_int_without_left_right_check");
	fig_left_dispartity_map_g.Draw(img__left_dispartity_map_int);


	Figure fig_right_dispartity_map_g;
	fig_right_dispartity_map_g.SetTitle("right_dispartity_map_int");
	fig_right_dispartity_map_g.Draw(img_right_dispartity_map_int);




	//
	ImgGray img_dispartity_map_g;
	ImgInt img_dispartity_map_int;
	
	if ( 
		compute_inconsistence_check_left_right(img_left_dispartity_map_g, 
											   img_right_dispartity_map_g, 
											  &img_dispartity_map_g)      
	   ==0)
	{ printf("c2 \n"); }

	Convert(img_dispartity_map_g,&img_dispartity_map_int);
	Figure fig_dispartity_map_int;
	fig_dispartity_map_int.SetTitle("dispartity_map_int_with_left_right_check");
	fig_dispartity_map_int.Draw(img_dispartity_map_int);



	




	/*
	d.	Use triangulation to determine the depth of each matched pixel. 
		and create mash file
	*/

	//depth = k / disparity


	ImgFloat img_depth;
	ImgFloat img_dispartity_map_f;
	Convert(img_dispartity_map_int, &img_dispartity_map_f);

	float k=400.0f;
	/*float kbi5=k/5;
	float kbi52=k/5*2;
	float kbi53=k/5*3;
	float kbi54=k/5*4;*/

	img_depth.Reset(img_dispartity_map_int.Width(), img_dispartity_map_int.Height() );
	Set(&img_depth,0);

	
	
	
	/*for(int x=0; x<img_depth.Width(); x++)
	{
		for(int y=0; y<img_depth.Height(); y++)
		{
			
			if ( 0 != img_dispartity_map_int(x,y) )
				img_depth(x,y)=k / img_dispartity_map_f(x,y);
	
	
		}
	}*/
	
	
	int* pixel_disp_map=img_dispartity_map_int.Begin();
	float* pixel_depth=img_depth.Begin();

	while(pixel_disp_map!=img_dispartity_map_int.End() )
	{
		
		if ( 0 != *pixel_disp_map )
			*pixel_depth=k / float(*pixel_disp_map);
		
		pixel_depth++;
		pixel_disp_map++;
	}

	//CString path_blepo_images ="../../images/";
	//CString filename_input_1=argv[1];//loading file1
	CString file_name_ply="tsukuba.ply";
	CString file_path_ply=path_blepo_images+file_name_ply;
	CString file_contents;
	
	

	if ( creat_ply_file_content( img_loaded_left_bgr, img_depth, &file_contents) ==0) 
		printf("Note: The ply file contants done \n");


	if ( creat_always_txt_file(file_path_ply, file_contents)  ==0) 
		printf("Note: The file %s has been created in the path %s \n",file_name_ply,path_blepo_images);
	


	EventLoop();
	return 0;
}

