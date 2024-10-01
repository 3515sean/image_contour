#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/stat.h>
#include <sys/types.h>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "./img_lib/stb_image.h"
#include "./img_lib/stb_image_write.h"

#define DEVIATION  3
#define BASE_RANGE 3
int w, h, n;
const unsigned char deviation = DEVIATION;
void count_diff_color(unsigned char pix,int *difc_num,unsigned char *diffcolor);
void write_image(int x,int y,unsigned char image[]);
#define get_rgb(x,y,image,sw) image[((x)+(y)*w)*3+(sw)] //r sw=0,g sw=1,b sw=2

int mkdir(const char *pathname, mode_t mode);


int main(int argc,char *argv[]){    
	for(int pic=1;pic<argc;pic++){//process all input pictures
    //Initial
    const int base_scan_range = BASE_RANGE;//block size n*n
    unsigned char *input_image = stbi_load(argv[pic], &w, &h, &n, 0);//load image
	unsigned char *output_image = (unsigned char*)malloc(w * h * n * sizeof(unsigned char));
	unsigned char *diffcolor = (unsigned char*) malloc(w*h);	
	unsigned int scan_range;
	unsigned int difc_num;//number of different color in block
	//check memory
	if(diffcolor==NULL || output_image==NULL){
		printf("Could not allocate memory");
		return 0;
	}  
	// wipe image
	for(int i=0;i<w*h*n;i++){
	    output_image[i]=255;//r=g=b=255=white
    }
    printf("%d, %d, %d\n", w, h, n); 
    
    //scan edge
	for(int color=0;color<3;color++){//scan in 3 base color(red->green->blue)
        for(int x=0;x<w-base_scan_range;x++){
    	    for(int y=0;y<h-base_scan_range;y++){
				scan_range = base_scan_range;
				difc_num=0;
				//scanning range
    		    for(int range_x=0;range_x<scan_range;range_x++){
					for(int range_y=0;range_y<scan_range;range_y++){
						count_diff_color(
						  get_rgb(x+range_x,y+range_y,input_image,color),
						  &difc_num,
						  diffcolor
						);
					}
					
				}
				
				//increase scanning range	
				while(difc_num>scan_range*scan_range*0.6){
					for(int range_x=0;range_x<scan_range;range_x++){
					    if(x+range_x>w)break;
					    count_diff_color(
						  get_rgb(x+range_x,y+scan_range-1,input_image,color),
						  &difc_num,
						  diffcolor
						  );
				    }				
				    for(int range_y=0;range_y<scan_range;range_y++){
					    if(y+range_y>h)break;
					    count_diff_color(
						  get_rgb(x+scan_range-1,y+range_y,input_image,color),
						  &difc_num,
						  diffcolor
						  );
				    }
				    scan_range++;
				}	
				
				if(scan_range!=base_scan_range){
					//if more than 60% colors in the block are different 
					//set center pixel to black
				    write_image(
					    x+((scan_range+1)/2)-1,
						y+((scan_range+1)/2)-1,
						output_image
					);
			    }
			    
		    }
	    }
    }
	//output image
	char str_buffer[30];
	char *pic_name=argv[pic];
	//find the filename
	for(int i=0;pic_name[i]!='\0';i++){
		if(pic_name[i]=='\\'){			
			pic_name+=i+1;
			i=0;
		}
	}
	struct stat st = {0};
	if (stat(".\\output", &st) == -1) {
		mkdir(".\\output", 0700);
	}
	sprintf(str_buffer,".\\output\\%s",pic_name);
	printf("%s\n",str_buffer);
    stbi_write_jpg(str_buffer, w, h, n, output_image, 100);    
    stbi_image_free(input_image);
    free(output_image);
}
	printf("finish!!\n");
	system("pause");
    return 0;
}

void count_diff_color(unsigned char pix,int *difc_num,unsigned char *diffcolor){
	int add=1;
	for(int i=0;i<*difc_num;i++){
		if(abs(pix-diffcolor[i])<=deviation){
			add=0;
			break;
		}
	}
	if(add){
		diffcolor[*difc_num]=pix;
		(*difc_num)++;		
	}	
}

void write_image(int x,int y,unsigned char image[]){
	image[(x+y*w)*3]=0;
	image[(x+y*w)*3+1]=0;
	image[(x+y*w)*3+2]=0;
}
