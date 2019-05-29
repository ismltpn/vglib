#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "vglib.h"

#define DELTA 10e-10

double split1(double x){
  if(x>10) return 50;
  return -50;
}
double split2(double x){
  if(x>-20) return x;
  return x+5;
}
double split3(double x){
  if(x>-50) return x*x/20;
  return x*x/20-5;
}
Tree* newt(int data){
  Tree * new = ALLOC(Tree,1);
  new->data=data;
  new->left=NULL;
  new->right=NULL;
  return new;
}
void fill_tree1(Tree ** root){
  (*root)=newt(333);
  (*root)->left=newt(10);
  (*root)->right=newt(15);
  (*root)->right->left=newt(20);
  (*root)->left->left=newt(25);
  (*root)->left->left->right=newt(25);
  (*root)->left->left->right->left=newt(495);
  (*root)->left->left->left=newt(25);
  (*root)->left->left->left->left=newt(25);
  (*root)->left->left->left->left->left=newt(666);
  (*root)->left->left->left->left->left->left=newt(999 );
  (*root)->left->left->left->left->left->left->left=newt(666);
  (*root)->left->left->left->right=newt(25);
  (*root)->left->left->left->right->right=newt(25);
  (*root)->left->left->left->right->right->right=newt(25);
  (*root)->left->left->left->right->right->right->left=newt(25);
  (*root)->left->left->left->right->right->left=newt(485);
  (*root)->left->left->left->right->right->left->right=newt(485);
  (*root)->left->left->left->right->right->left->left=newt(485);
  (*root)->left->left->right->left->left=newt(25);
  (*root)->right->right=newt(20);
  (*root)->right->right->right=newt(20);
  (*root)->left->right=newt(25);
  (*root)->left->right->left=newt(25);
}
void fill_tree2(Tree ** root){
  (*root)=newt(10);
  (*root)->left=newt(100);
  (*root)->right=newt(89);
  (*root)->left->left=newt(4);
  (*root)->left->right=newt(11);
  (*root)->right->right=newt(54);
}
void delete_tree(Tree * root){
  if(root == NULL) return;
  delete_tree(root->left);
  delete_tree(root->right);
  free(root);
}

void scale_file(char * input, double scale_x, double scale_y, char * output){
  Figure * temp = start_figure(0,0);
  import_svg(input,temp);
  scale_figure(temp,scale_x,scale_y);
  export_svg(temp,output);
}
void resize_file(char * input, Point2D start_roi, Point2D end_roi, char * output){
  Figure * temp = start_figure(0,0);
  import_svg(input,temp);
  resize_figure(temp,start_roi,end_roi);
  export_svg(temp,output);
}
void svg_to_eps(char * svgname, char * epsname){
  Figure * temp = start_figure(0,0);
  import_svg(svgname,temp);
  export_eps(temp,epsname);
}
Color darker(Color c){
  Color new;
  new.r = c.r-5;
  new.g = c.g-5;
  new.b = c.b-5;
  if(new.r<0) new.r=0;
  if(new.g<0) new.g=0;
  if(new.b<0) new.b=0;
  return new;
}
Color lighter(Color c){
  Color new;
  new.r = c.r+5;
  new.g = c.g+5;
  new.b = c.b+5;
  if(new.r>255) new.r=255;
  if(new.g>255) new.g=255;
  if(new.b>255) new.b=255;
  return new;
}
double fog1(double x){
  return sin(x)+1;
}double fog2(double x){
  return cos(x+1)+2.5;
}double fog3(double x){
  return sin(x)+1.5;
}double fog4(double x){
  return sin(x)+2;
}double fog5(double x){
  return cos(x+1)+0.5;
}double fog6(double x){
  return sin(x)+3.5;
}
void draw_fog(Figure * fog, Color c, double thickness, double range){
  set_thickness_resolution(fog,thickness,100);
  set_color(fog,c);
  draw_fx(fog,fog1,-range,range);
  draw_fx(fog,fog2,-range,range);
  draw_fx(fog,fog3,-range,range);
  draw_fx(fog,fog4,-range,range);
  draw_fx(fog,fog5,-range,range);
  draw_fx(fog,fog6,-range,range);
}
void painting(Figure ** fig){
  Point2D bgleft = {-500,450};
  Point2D bgright = {500,450};
  Point2D moon_center = {100,250};
  Point2D background[50];
  Point2D ground[10];
  Point2D s = {-500,-500};
  Point2D e = {500,500};
  Color background_c = {22,27,33};
  Color fog_c = {37,44,52};
  Color moon_fog = {130,133,135};
  Color moon_c = {223,222,218};
  Color moon_light = {179,188,188};
  Color tree_c = {11,9,9};
  Color ground_c = {6,6,5};
  Color snow_c = {200,200,220};
  int i=0;
  double moon_r= 100;
  Figure * fog = start_figure(1000,1000);
  set_thickness_resolution(*fig,100,1);
  set_color(*fig,background_c);
  for(;bgleft.y>-500;bgleft.y-=100,bgright.y-=100){
    background[i]=bgleft;
    i++;
    background[i]=bgright;
    i++;
  }
  draw_polyline(*fig,background,i);

  /* Fog */
  draw_fog(fog,fog_c,70,5);
  fog_c=lighter(fog_c);
  draw_fog(fog,fog_c,40,5);
  fog_c=lighter(fog_c);
  draw_fog(fog,fog_c,10,5);
  scale_figure(fog,100,100);
  append_figures(fog,*fig);
  *fig=fog;

  /* Moon */
  set_thickness_resolution(*fig,20,100);
  set_color(*fig,moon_light);
  draw_circle(*fig,&moon_center,moon_r+10);
  set_color(*fig,moon_c);
  for(;moon_r>0-DELTA;moon_r-=10) draw_circle(*fig,&moon_center,moon_r);

  /* Ground */
  set_thickness_resolution(*fig,11,100);
  set_color(*fig,ground_c);
  ground[0]= new_point(-500,0);
  ground[1]= new_point(-300,-110);
  ground[2]= new_point(-50,-100);
  ground[3]= new_point(100,0);
  ground[4]= new_point(200,-170);
  ground[5]= new_point(300,-200);
  ground[6]= new_point(500,-300);
  draw_polyline(*fig,ground,7);
  do{
    for(i=0;i<7;i++){
      ground[i].y-=10;
    }
    draw_polyline(*fig,ground,7);
  }while(ground[3].y>-500);

  /* Trees */
  set_thickness_resolution(*fig,5,10);
  set_color(*fig,tree_c);
  draw_fractal_atree(*fig,-400,100,150,6);
  draw_fractal_atree(*fig,-330,50,150,4);
  set_thickness_resolution(*fig,9,10);
  draw_fractal_atree(*fig,-450,40,150,3);
  set_thickness_resolution(*fig,5,10);
  draw_fractal_tree(*fig,400,40,150,6);
  set_thickness_resolution(*fig,12,10);
  draw_fractal_atree(*fig,300,0,200,3);
  set_thickness_resolution(*fig,9,10);
  draw_fractal_tree(*fig,470,20,150,4);

  /* some snow */
  set_thickness_resolution(*fig,1,10);
  set_color(*fig,snow_c);
  srand(time(NULL));
  for(i=0;i<25;i++){
    moon_center.x=(rand()%1000)-500;
    moon_center.y=(rand()%700)-200;
    draw_koch_snowflake(*fig,&moon_center,0.4,rand()%4+9,rand()%3+2);
  }
}
int main(){
  Tree *root1,*root2;
  Figure * fig = NULL;
  Figure * fig2 = NULL;
  Figure * fig3 = NULL;
  Color red = {255,0,0};
  Color green = {0,255,0};
  Color blue = {0,0,255};
  Color black = {0,0,0};
  Color purple = {232, 67, 147};
  Point2D start_roi;
  Point2D end_roi;
  Point2D center = {0,0};
  Point2D var_center;
  Point2D polyline[5];

  /* Binary Tree */
  fill_tree1(&root1);
  fig = start_figure(1500, 500);
  set_color(fig,black);
  set_thickness_resolution(fig,1,100);
  draw_binary_tree(fig,root1);
  delete_tree(root1);
  export_svg(fig,"binary-tree.svg");

  /* Binary Tree in PDF*/
  fill_tree2(&root2);
  fig = start_figure(500, 500);
  set_color(fig,black);
  set_thickness_resolution(fig,1,100);
  draw_binary_tree(fig,root2);
  delete_tree(root2);
  export_svg(fig,"binary-tree-pdf.svg");

  /* Koch Snowflake */
  fig = start_figure(500,500);
  set_color(fig,red);
  draw_koch_snowflake(fig,&center,0.5,200,6);
  export_svg(fig,"koch-snowflake.svg");

  /* Fractal Tree */
  fig = start_figure(700,700);
  set_color(fig,black);
  draw_fractal_tree(fig,0,0,100,10);
  export_svg(fig,"fractal-tree.svg");

  /* Asimetric Fractal Tree */
  fig = start_figure(700,700);
  set_color(fig,black);
  draw_fractal_atree(fig,0,0,100,10);
  export_svg(fig,"a-fractal-tree.svg");

  /* Sine Fx */
  fig = start_figure(500,500);
  set_color(fig,blue);
  set_thickness_resolution(fig,1,100);
  draw_fx(fig,sin,-20,20);
  scale_figure(fig,50,50);
  export_svg(fig,"sine-fx.svg");

  /* Split Fx */
  fig = start_figure(500,500);
  fig2 = start_figure(500,500);
  fig3 = start_figure(500,500);
  set_color(fig,red);
  set_color(fig2,green);
  set_color(fig3,blue);
  set_thickness_resolution(fig,1,50);
  set_thickness_resolution(fig2,2,100);
  set_thickness_resolution(fig3,0.5,200);
  draw_fx(fig,split1,-250,250);
  draw_fx(fig2,split2,-250,250);
  draw_fx(fig3,split3,-250,250);
  append_figures(fig2,fig3);
  append_figures(fig,fig2);
  export_svg(fig,"split-fxs.svg");

  /* Circle & Ellipse */
  fig = start_figure(500,500);
  fig2= start_figure(500,500);
  set_color(fig,purple);
  set_color(fig,black);
  set_thickness_resolution(fig,3,10);
  set_thickness_resolution(fig2,1,10);
  draw_circle(fig,&center,200);
  draw_ellipse(fig2,&center,150,100);
  var_center = new_point(0,100);
  draw_ellipse(fig2,&var_center,30,50);
  var_center = new_point(0,-100);
  draw_ellipse(fig2,&var_center,30,50);
  append_figures(fig,fig2);
  export_svg(fig,"circle-ellipse.svg");

  /* Low-res Circle */
  fig = start_figure(500,500);
  set_color(fig,black);
  set_thickness_resolution(fig,1,2);
  draw_circle(fig,&center,200);
  export_svg(fig,"low-res-circle.svg");

  /* Polyline */
  fig = start_figure(500,500);
  set_color(fig,green);
  polyline[0] = new_point(-150,-150);
  polyline[1] = new_point(-150,150);
  polyline[2] = new_point(150,150);
  polyline[3] = new_point(150,-150);
  polyline[4] = new_point(-150,-150);
  draw_polyline(fig,polyline,5);
  set_color(fig,blue);
  polyline[0] = new_point(0,-200);
  polyline[1] = new_point(-200,0);
  polyline[2] = new_point(0,200);
  polyline[3] = new_point(200,0);
  polyline[4] = new_point(0,-200);
  draw_polyline(fig,polyline,5);
  set_color(fig,red);
  polyline[0] = new_point(-300,250);
  polyline[1] = new_point(300,150);
  polyline[2] = new_point(-300,-100);
  polyline[3] = new_point(-100,-300);
  polyline[4] = new_point(100,-200);
  draw_polyline(fig,polyline,5);
  export_svg(fig,"polylines.svg");

  /* A simple painting */
  fig = start_figure(1000,1000);
  painting(&fig);
  export_svg(fig,"painting.svg");

  /* Scale Drawings */
  scale_file("binary-tree.svg",1.4,1.4,"binary-tree-scaled.svg");
  scale_file("koch-snowflake.svg",1.4,1.4,"koch-snowflake-scaled.svg");
  scale_file("fractal-tree.svg",1.4,1.4,"fractal-tree-scaled.svg");
  scale_file("a-fractal-tree.svg",1.4,1.4,"a-fractal-tree-scaled.svg");

  /* Resize Drawings */
  start_roi = new_point(-200,-200);
  end_roi = new_point(200,200);
  resize_file("binary-tree.svg",start_roi,end_roi,"binary-tree-resized.svg");
  resize_file("koch-snowflake.svg",start_roi,end_roi,"koch-snowflake-resized.svg");
  resize_file("fractal-tree.svg",start_roi,end_roi,"fractal-tree-resized.svg");
  resize_file("a-fractal-tree.svg",start_roi,end_roi,"a-fractal-tree-resized.svg");

  /* Copying svg figures to eps */
  svg_to_eps("binary-tree.svg","binary-tree.eps");
  svg_to_eps("koch-snowflake.svg","koch-snowflake.eps");
  svg_to_eps("fractal-tree.svg","fractal-tree.eps");
  svg_to_eps("a-fractal-tree.svg","a-fractal-tree.eps");
  svg_to_eps("sine-fx.svg","sine-fx.eps");
  svg_to_eps("split-fxs.svg","split-fxs.eps");
  svg_to_eps("circle-ellipse.svg","circle-ellipse.eps");
  svg_to_eps("low-res-circle.svg","low-res-circle.eps");
  svg_to_eps("polylines.svg","polylines.eps");

  return 0;
}
