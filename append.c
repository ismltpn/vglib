#include <stdio.h>
#include "vglib.h"

int main(int argc, char *argv[]){
  Figure * fig, * fig2;
  int i,j;
  char new_name[255];
  if(argc == 3){
    fig = start_figure(0,0);
    fig2 = start_figure(0,0);
    import_svg(argv[1],fig);
    import_svg(argv[2],fig2);
    append_figures(fig,fig2);
    for(i=0;argv[1][i+4]!='\0';i++) new_name[i]=argv[1][i];
    new_name[i]='+';
    i++;
    j=i;
    for(i=0;argv[2][i]!='\0';i++) new_name[i+j]=argv[2][i];
    new_name[i+j]='\0';
    export_svg(fig,new_name);
    printf("%s\n", new_name);
    printf("%s\n", argv[2]);
  }
  else if(argc > 3){
    printf("Too much arguments!\n");
  }
  else{
    printf("Two arguments expected.\n");
  }
}
