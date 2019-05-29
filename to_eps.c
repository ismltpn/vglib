#include <stdio.h>
#include "vglib.h"

void svg_to_eps(char * svgname){
  int i;
  char out[100];
  Figure * temp = start_figure(0,0);
  import_svg(svgname,temp);
  for(i=0;svgname[i+3]!='\0';i++){
    out[i]=svgname[i];
  }
  out[i]='e';
  out[i+1]='p';
  out[i+2]='s';
  out[i+3]='\0';
  export_eps(temp,out);
}

int main(int argc, char *argv[]){
  if(argc == 2){
    svg_to_eps(argv[1]);
  }
  else if(argc > 2){
    printf("Too much arguments!\n");
  }
  else{
    printf("One argument expected.\n");
  }
}
