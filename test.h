
#pragma once
#include <afxwin.h> 
#include <math.h> 
#include <stdio.h> 
#include <queue>


#include "../../src/blepo.h"





using namespace std;
using namespace blepo;
using namespace blepo_ex;
//using namespace blepo_ex;

struct node
	{
		int x;
		int y;
	};


int initial_filename_recognize( const CString path_input, const CString filename_input)
{
	

	if(   fopen(path_input+filename_input,"r")==0   )
	  { return 1; }
	
	else return 0;
				
}


void separable_convolution_x( const ImgGray &img_in,  const int kernel_width, const int gaussian_kernel[], ImgGray* img_out ) 
{


	(*img_out).Reset( img_in.Width(),img_in.Height() );
	Set(img_out, 0);

	
	for( int y=0; y< img_in.Height(); y++ ) 
	{
		for( int x=(kernel_width-1)/2; x< img_in.Width()-(kernel_width-1)/2; x++ ) 
		{
			int tmp_value=0;
			for(int ctn=0; ctn<kernel_width ; ctn++)
			{

				if(	  ( (x+ (kernel_width-1)/2 - ctn) < 0 ) 
					||( (x+ (kernel_width-1)/2 - ctn) >= img_in.Width() )   
				  )

				{
					tmp_value+=gaussian_kernel[ctn] * 0;				
				}

				else 
				{
					tmp_value+=gaussian_kernel[ctn] * img_in(x+ (kernel_width-1)/2 - ctn,y);
				}

			}//for ctn
			if( tmp_value>255 ) tmp_value=255;
			if( tmp_value<0 ) tmp_value=0;
			(*img_out)(x,y)=tmp_value;
			

		}//for x
	}//for y

	//return 0;

}



//convolution_y   with kernel
void separable_convolution_y( const ImgGray &img_in,  const int kernel_width, const int gaussian_kernel[], ImgGray* img_out ) 
{


	(*img_out).Reset( img_in.Width() , img_in.Height() );
	Set(img_out, 0);

	for( int x=0; x< img_in.Width(); x++) 
	{
		for(  int y=(kernel_width-1)/2; y< img_in.Height()-(kernel_width-1)/2; y++ ) 
		{
			int tmp_value=0;

			for(int ctn=0; ctn<kernel_width ; ctn++)
			{

				if(	  ( (y+ (kernel_width-1)/2 - ctn) < 0 ) 
					||( (y+ (kernel_width-1)/2 - ctn) >= img_in.Height() )   
				  )

				{
					tmp_value+=gaussian_kernel[ctn] * 0;				
				}

				else 
				{
					tmp_value+=gaussian_kernel[ctn] * img_in(x,y + (kernel_width-1)/2 - ctn );
				}

			}//for ctn
			
			if( tmp_value>255 ) tmp_value=255;
			if( tmp_value<0 ) tmp_value=0;
			(*img_out)(x,y)=tmp_value;
			

		}//for x
	}//for y

	//return 0;

}







void inner_dissim_absolute_difference(
	const unsigned char* pixel_img_in_left, 
	const unsigned char* pixel_img_in_right,
	const unsigned char* end_value,
	unsigned char* pixel_absolute_dis)
{

	while( pixel_img_in_left != end_value)
	{
		*pixel_absolute_dis= *pixel_img_in_left-*pixel_img_in_right>0? 
				(*pixel_img_in_left-*pixel_img_in_right):(*pixel_img_in_right-*pixel_img_in_left);
		pixel_absolute_dis++;
		pixel_img_in_left++;
		pixel_img_in_right++;
	}

}





/*for(int x=0; x<img_in_left.Width(); x++)
		{
			for(int y=0; y<img_in_left.Height(); y++)
			{
				absolute_dis_xy[ctn](x,y)=
		
		
		
		
		
		
		
		
			}
		}*/

void inner_dissim_finding_best_disparity(
	const ImgGray &img_in_left, 
	const int disp_min, 
	const int disp_max, 
	ImgGray absolute_dis_xy[], 
	ImgGray* img_out_disp_map,
	bool offset_disp)

{
	//initial 
	int disp_diff= disp_max-disp_min+1;
	
	(*img_out_disp_map).Reset(img_in_left.Width(), img_in_left.Height());
	Set(img_out_disp_map,0);
	
	std::vector<unsigned char*> pixel_absolute_diff_array(disp_diff);
    unsigned char* pixel_dispartity_map = (*img_out_disp_map).Begin();




	//set offset 
	int offset_in_end=0;
	if( 1== offset_disp) offset_in_end=disp_diff-1;

	for(int ctn = disp_min ; ctn <= disp_max ; ctn++)  
	{
		if(1==offset_disp) pixel_absolute_diff_array[ctn] = (absolute_dis_xy[ctn]).Begin(ctn);
		else pixel_absolute_diff_array[ctn] = (absolute_dis_xy[ctn]).Begin();
	}



	//find best disp
    for( ;pixel_absolute_diff_array[disp_max] != (absolute_dis_xy[disp_max]).End()-offset_in_end; )
    {
		int value_left_best_disparity=0;
		int minimum_value = img_in_left.Height() * img_in_left.Width();
		
     
		for (int ctn = disp_min ; ctn <= disp_max ; ctn++)  
		{
			int pixel_absolute_diff_value = *pixel_absolute_diff_array[ctn];
			pixel_absolute_diff_array[ctn]++;

			if (pixel_absolute_diff_value < minimum_value)
			{
				value_left_best_disparity = ctn;
				minimum_value = pixel_absolute_diff_value;
			}
		}
		*pixel_dispartity_map = value_left_best_disparity;
		pixel_dispartity_map++;
    }
	

}















int compute_3D_array_of_dissimilarities(const ImgGray& img_in_left, ImgGray &img_in_right, const int disp_min, const int disp_max, ImgGray* img_out_left_dispartity_map, ImgGray* img_out_right_dispartity_map)
{
	/*int disp_min;
	int disp_max;*/

	//make sure,the size of left and right is the same
	if( 0==IsSameSize(img_in_left,img_in_right) )
	{
		return 1;//size diff		
	}



	//precompute

	/*(*img_out_left_dispartity_map).Reset(img_in_left.Width(), img_in_left.Height());
	Set(img_out_left_dispartity_map,0);

	(*img_out_right_dispartity_map).Reset(img_in_left.Width(), img_in_left.Height());
	Set(img_out_right_dispartity_map,0);*/


	ImgGray img_mid;
	const int window_size=5;
	int convolution_kernel[window_size+1];

	for(int ctn=0; ctn<window_size; ctn++)
	{
		convolution_kernel[ctn]=1;
	}



	const int disp_diff=disp_max-disp_min+1;

	ImgGray absolute_dis_xy[255];
	//ImgGray absolute_dis_xy(disp_max);
	for(int ctn= disp_min; ctn<=disp_max; ctn++ )
	{
	
		const unsigned char* pixel_img_in_left = img_in_left.Begin(ctn);
		const unsigned char* pixel_img_in_right = img_in_right.Begin();
		
		absolute_dis_xy[ctn].Reset(img_in_left.Width(), img_in_left.Height());
		unsigned char* pixel_absolute_dis = absolute_dis_xy[ctn].Begin(ctn);
		
		
		inner_dissim_absolute_difference(pixel_img_in_left,pixel_img_in_right, img_in_left.End()-ctn ,pixel_absolute_dis );
		ConvolveBox5x5(absolute_dis_xy[ctn], &absolute_dis_xy[ctn]);
		//ConvolveBoxNxN(absolute_dis_xy[ctn], &absolute_dis_xy[ctn], window_size);
		/*separable_convolution_y( absolute_dis_xy[ctn],  window_size, convolution_kernel, &img_mid );
		separable_convolution_x( img_mid,  window_size, convolution_kernel,  &absolute_dis_xy[ctn]);*/
	}



	//left disp map
	inner_dissim_finding_best_disparity(
		img_in_left, 
		disp_min, 
		disp_max, 
		absolute_dis_xy, 
		img_out_left_dispartity_map,
		0);

	//right disp map
	inner_dissim_finding_best_disparity(
		img_in_left, 
		disp_min, 
		disp_max, 
		absolute_dis_xy, 
		img_out_right_dispartity_map,
		1);

	
	return 0;
}







int compute_inconsistence_check_left_right(
	const ImgGray& img_in_left_disp_map,
	const ImgGray& img_in_right_disp_map,
	ImgGray* img_out_dispartity_map)
{
	//initial
	(*img_out_dispartity_map).Reset(img_in_left_disp_map.Width(), img_in_left_disp_map.Height() );
	//Set(img_out_dispartity_map,0);
	*img_out_dispartity_map=img_in_left_disp_map;


	for(int x=0; x<img_in_left_disp_map.Width(); x++)
	{
		for(int y=0; y<img_in_left_disp_map.Height(); y++)
		{
			if( (x-img_in_left_disp_map(x,y) )>0
				&&img_in_left_disp_map(x,y) !=img_in_right_disp_map( x-img_in_left_disp_map(x,y),y )
			  )
			  (*img_out_dispartity_map)(x,y)=0;
			
		}
	}


	//ImgGray img_in_left_disp_map_copy;
	//img_in_left_disp_map_copy.Reset(img_in_left_disp_map.Width(), img_in_left_disp_map.Height() );
	//img_in_left_disp_map_copy=img_in_left_disp_map;
	//
	//ImgGray img_in_right_disp_map_copy;
	//img_in_right_disp_map_copy.Reset(img_in_left_disp_map.Width(), img_in_left_disp_map.Height() );
	//img_in_right_disp_map_copy=img_in_right_disp_map;
	//
	//unsigned char* pixel_left_disp_map=img_in_left_disp_map_copy.Begin();
	//unsigned char* pixel_right_disp_map=img_in_right_disp_map_copy.Begin();
	//unsigned char* pixel_disp_map = (*img_out_dispartity_map).Begin();
 //   //unsigned char* pe = absolute_dis_xy[0].End();
 //   
	//for(;
	//	pixel_left_disp_map != img_in_left_disp_map.End(); 
	//	pixel_left_disp_map++ )
	//{
	//	pixel_right_disp_map=pixel_left_disp_map;
	//	unsigned char* pixel_position_right= pixel_right_disp_map-*pixel_left_disp_map;//get position
	//	//pixel_right_disp_map = *pixel_position_right;


	//	if(*pixel_left_disp_map!=*(pixel_right_disp_map)) *pixel_disp_map=0;

	//}
	return 0;

}



int creat_always_txt_file(const CString& file_Path, CString file_Contents)  
{  
    HANDLE hFile = CreateFile(file_Path,  
        GENERIC_WRITE, FILE_SHARE_READ,  
        NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
   
    if (hFile == INVALID_HANDLE_VALUE)  
    {  
        printf("Cannot create the file: %s", file_Path);  
        return 1;  
    }  
    else  
    {  
        CFile myFile(hFile);  
        char* c_string = (file_Contents).GetBuffer( file_Contents.GetLength() );  
		unsigned int n_length = strlen(c_string);  
   
        myFile.Write(c_string, n_length);  
        myFile.Close();  
   
       file_Contents.ReleaseBuffer();  
    }  
    return 0;  
}








int creat_ply_file_content( const ImgBgr &img_loaded_left_bgr, const ImgFloat &img_depth, CString* file_contents)
{
		
	CString file_contents_head;
	CString file_contents_data;
	CString file_contents_data_line[12];
	int number_v=0;
	for(int x=0; x<img_depth.Width(); x++)
	{
		for(int y=0; y<img_depth.Height(); y++)
		{
			if( 0!=img_depth(x,y) )
			{

				file_contents_data_line[0].Format(TEXT("%f"),float(x) );//x
				file_contents_data_line[1].Format(TEXT(" "),1);
				file_contents_data_line[2].Format(TEXT("%f"),float(img_depth.Height()-1-y) );//y
				file_contents_data_line[3].Format(TEXT(" "),1);
				file_contents_data_line[4].Format(TEXT("%f"),(-1.0f)*img_depth(x,y) );//z
				//file_contents_data.Format(TEXT("\r\n"),1);

				file_contents_data_line[5].Format(TEXT(" "),1);
				
				//color
				//int R_color=0, G_color=0, B_color=0;

				file_contents_data_line[6].Format(TEXT("%d"),img_loaded_left_bgr(x,y).r );//r
				file_contents_data_line[7].Format(TEXT(" "),1);
				file_contents_data_line[8].Format(TEXT("%d"),img_loaded_left_bgr(x,y).g );//g
				file_contents_data_line[9].Format(TEXT(" "),1);
				file_contents_data_line[10].Format(TEXT("%d"),img_loaded_left_bgr(x,y).b );//b
				file_contents_data_line[11].Format(TEXT(" "),1);

				for(int ctn=0; ctn<12 ; ctn++)
				{
					file_contents_data+=file_contents_data_line[ctn];
				}
				file_contents_data+="\r\n";
				number_v++;



			}
		}
	}


	//head
	CString head_line[12];
	head_line[1]="ply";
	head_line[2]="format ascii 1.0";
	head_line[3]="element vertex ";
	head_line[4].Format(TEXT("%d"),number_v);

	head_line[5]="property float x";
	head_line[6]="property float y";
	head_line[7]="property float z";
	head_line[8]="property uchar diffuse_red";
	head_line[9]="property uchar diffuse_green";
	head_line[10]="property uchar diffuse_blue";
	head_line[11]="end_header";

	for(int ctn=1; ctn<12 ; ctn++)
	{
		
		if( 3==ctn ) head_line[3]+=head_line[4];
		if( 4==ctn) continue;

		file_contents_head+=head_line[ctn];
		file_contents_head+="\r\n";
		//file_contents_head.Format(TEXT("\r\n"),1);
	}

	/*while(pixel_depth!=img_depth.End() )
	{
		file_contents_head.Format(TEXT("%d"),pixel_depth);
		file_contents_head.Format(TEXT(" "),1);
		file_contents_head.Format(TEXT("%d"),pixel_depth);
		pixel_depth++;
	}*/


	*file_contents=file_contents_head+file_contents_data;




	return 0;



}


