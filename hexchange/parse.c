#include <stdio.h>
#include <stdbool.h>

void main()
{
  FILE *fp = fopen("out","rt");
  if(fp == NULL)
    printf("file error\n");
  
  char c;
  int numspace = 0;
  int numline = 0;
  bool printon = false;
  while(!feof(fp)){
    c = fgetc(fp);
    if(c == ' '){
      numspace++;
      if(numspace == 3){
	printon = true;
	numspace = 0;
      }
    }
    else if(c == '\n'){
      numline = 0;
      numspace = 0;
      printon = false;
    }
    else{
      if(!printon){
      }
      else{
	if(numspace == 1)
	  numspace = 0;
	if(numspace == 2){
	  printf("\n");
	  numspace = 0;
	  numline++;
	  if(numline == 4){
	    printon = false;
	    continue;
	  }
	}
	printf("%c",c);
      }
    }/*printon*/

  }
}
