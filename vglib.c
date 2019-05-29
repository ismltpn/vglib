#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vglib.h"

#define DELTA 10e-10
#define EMPTY_COORDS 124891214

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* helper functions for drawing */
int tree_depth(Tree * root);
void tree(Figure * fig,double sx, double sy, double len, double deg, int num_iterations, double dl, double dr,double thickness);
void koch(Point2D s, Point2D e, int num_iterations, double deg, Figure * fig,double thickness);
void bin_tree(Figure * fig, Tree * root, double size, double text_size, Point2D pos, double y_diff, double x_diff, Point2D old_pos);
void cirle(double theta, Point2D * p);

/* Queue functions */
void enqueue(Figure * fig, Line l);
void enqueue_t(Figure * fig, Text data);
int dequeue(Figure * fig, Line * l);
int dequeue_t(Figure * fig, Text * output);

/* Math functions */
double length(Point2D p1, Point2D p2);
double min(double a, double b);
double max(double a, double b);
double diff(double f(double), double x, double delta);
Point2D align(Figure * fig, Point2D point);

/* Intercept checking functions */
int in_border(Figure * fig, Point2D point);
int inter_point(Line l1, Line l2, Point2D * p);
int border_intercepts(Figure * fig,Line line,Point2D * inter_points);
int in_line(Line l, Point2D p);

Figure * start_figure(double width, double height) {
  Figure * new = ALLOC(Figure,1);
  Color black = {0,0,0}; /* stock_color */
  new->width = width;
  new->height = height;
  new->color = black;
  new->resolution = 1;  /* stock resolution */
  new->thickness = 1;  /* stock thickness */
  new->lines = ALLOC(Queue_L,1);
  new->lines->front = new->lines->rear = NULL;
  new->texts = ALLOC(Queue_T,1);
  new->texts->front = new->texts->rear = NULL;
  return new;
}
void set_thickness_resolution(Figure  *  fig, double  thickness, double resolution){
  if(fig==NULL) return;
  fig->resolution = resolution;
  fig->thickness = thickness;
}
void set_color(Figure * fig, Color c){
  if(fig==NULL) return;
  fig->color = c;
}
void draw_fx(Figure * fig, double f(double x),double start_x, double end_x){
  /* iterates every x point in the given interval, finds results and adds them
   * to figure if they are in range */
  Line line;
  Point2D cur, old;
  int there_is_old = 0;
  if(fig==NULL) return;
  for(cur.x=start_x;cur.x<end_x+DELTA;cur.x+=(1/fig->resolution)){
    cur.y = f(cur.x);
    if(isfinite(cur.y)&&(!isnan(cur.y))){
      if(in_border(fig,cur) && (diff(f,cur.x, 1/fig->resolution)<fig->resolution && diff(f,cur.x, 1/fig->resolution)>-fig->resolution)){
        if(there_is_old==1){
          line.start=old;
          line.end=cur;
          line.color=fig->color;
          line.thickness=fig->thickness;
          enqueue(fig,line);
          old = cur;
        }
        else{
          old = cur;
          there_is_old = 1;
        }
      }
      else{
        there_is_old=0;
      }
    }
    else{
      there_is_old=0;
    }
  }
}
void circle(double theta, Point2D * p){
  /* unit circle */
  p->x=cos(theta);
  p->y=sin(theta);
}
void draw_circle(Figure * fig, Point2D * center, double r){
  /* drawing circle with using unit circle */
  Point2D old,cur;
  double angle;
  int there_is_old=0;
  Line line;
  if(fig==NULL) return;
  line.color=fig->color;
  line.thickness=fig->thickness;
  if(r>0){
    for(angle=0.0;angle<M_PI*2-DELTA;angle+=1/fig->resolution){
      circle(angle,&cur);
      cur.x*=r;
      cur.y*=r;
      cur.x+=center->x;
      cur.y+=center->y;
      if(in_border(fig,cur)){
        if(there_is_old==1){
          line.start=old;
          line.end=cur;
          enqueue(fig,line);
          old = cur;
        }
        else{
          old = cur;
          there_is_old = 1;
        }
      }
      else{
        there_is_old=0;
      }
    }
    circle(0,&cur);
    line.start=old;
    cur.x*=r;
    cur.y*=r;
    cur.x+=center->x;
    cur.y+=center->y;
    line.end=cur;
    enqueue(fig,line);
  }
}
void enqueue(Figure * fig, Line l){
  /* enqueue function for lines */
  node_l * new = ALLOC(node_l,1);
  if(fig==NULL) return;
  new->data = l;
  new->next = NULL;
  if(fig->lines->front==NULL){
    fig->lines->front=new;
    fig->lines->rear=new;
  }
  else{
    fig->lines->rear->next = new;
    fig->lines->rear = new;
  }
}
void enqueue_t(Figure * fig, Text t){
  /* enqueue function for texts */
  node_t * new = ALLOC(node_t,1);
  if(fig==NULL) return;
  new->data=t;
  new->next=NULL;
  if(fig->texts->front==NULL){
    fig->texts->front=new;
    fig->texts->rear=new;
  }
  else{
    fig->texts->rear->next = new;
    fig->texts->rear = new;
  }
}
int dequeue_t(Figure * fig, Text * output){
  /* dequeue function for texts */
  node_t * to_free;
  if(fig==NULL) return 0;
  if(fig->texts==NULL) return 0;
  if(fig->texts->front==NULL) return 0;
  to_free = fig->texts->front;
  *output = fig->texts->front->data;
  if(fig->texts->rear == fig->texts->front){
    fig->texts->rear = fig->texts->rear->next;
  }
  fig->texts->front=fig->texts->front->next;
  free(to_free);
  return 1;
}
int dequeue(Figure * fig, Line * l){
  /* dequeue function for lines */
  node_l * to_free;
  if(fig==NULL) return 0;
  if(fig->lines==NULL) return 0;
  if(fig->lines->front==NULL) return 0;
  to_free = fig->lines->front;
  *l = fig->lines->front->data;
  if(fig->lines->rear == fig->lines->front){
    fig->lines->rear = fig->lines->rear->next;
  }
  fig->lines->front=fig->lines->front->next;
  free(to_free);
  return 1;
}
Point2D align(Figure * fig, Point2D point){
  /* aligning abstact coordinates to svg coordinates */
  Point2D new;
  new.x=point.x+fig->width/2;
  new.y=point.y+fig->height/2;
  return new;
}
double diff(double f(double), double x, double delta){
  /* simple differential function */
  double dy = f(x) - f(x-delta);
  return dy/delta;
}
Point2D new_point(double x, double y){
  Point2D new;
  new.x=x;
  new.y=y;
  return new;
}
int border_intercepts(Figure * fig,Line line,Point2D * inter_points){
  /* checks if the line intercepts with border,
   * returns number of interceptions and interceptions points */
  int inter_counter = 0;
  Point2D temp;
  Line border1;
  Line border2;
  Line border3;
  Line border4;
  border1.start = new_point(-fig->width/2,-fig->height/2);
  border1.end = new_point(-fig->width/2,fig->height/2);
  border2.start = new_point(-fig->width/2,fig->height/2);
  border2.end = new_point(fig->width/2,fig->height/2);
  border3.start = new_point(fig->width/2,fig->height/2);
  border3.end = new_point(fig->width/2,-fig->height/2);
  border4.start = new_point(fig->width/2,-fig->height/2);
  border4.end = new_point(-fig->width/2,-fig->height/2);
  if(inter_point(line,border1,&temp)==1){
    if(in_border(fig,temp)==1){
      inter_points[inter_counter]=temp;
      inter_counter++;
    }
  }
  if(inter_point(line,border2,&temp)==1){
    if(in_border(fig,temp)==1){
      inter_points[inter_counter]=temp;
      inter_counter++;
    }
  }
  if(inter_point(line,border3,&temp)==1){
    if(in_border(fig,temp)==1){
      inter_points[inter_counter]=temp;
      inter_counter++;
    }
  }
  if(inter_point(line,border4,&temp)==1){
    if(in_border(fig,temp)==1){
      inter_points[inter_counter]=temp;
      inter_counter++;
    }
  }
  return inter_counter;
}

int in_line(Line l, Point2D p){
  /* checks if the given point is on the line */
  if(p.x<min(l.start.x,l.end.x)-DELTA || p.x>max(l.start.x,l.end.x)+DELTA ||
     p.y<min(l.start.y,l.end.y)-DELTA || p.y>max(l.start.y,l.end.y)+DELTA) {
  return 0;
  }
  return 1;
}
int inter_point(Line l1, Line l2, Point2D * p){
  /* returns interception point of two line */
  double px_top,px_bot;
  double py_top;
  px_top = (l1.start.x*l1.end.y-l1.start.y*l1.end.x)*(l2.start.x-l2.end.x)-(l1.start.x-l1.end.x)*(l2.start.x*l2.end.y - l2.start.y*l2.end.x);
  px_bot = (l1.start.x-l1.end.x)*(l2.start.y-l2.end.y)-(l1.start.y-l1.end.y)*(l2.start.x-l2.end.x);
  py_top = (l1.start.x*l1.end.y-l1.start.y*l1.end.x)*(l2.start.y-l2.end.y)-(l1.start.y-l1.end.y)*(l2.start.x*l2.end.y - l2.start.y*l2.end.x);
  if(fabs(px_bot)<0.000000001) return 0;
  p->x=px_top/px_bot;
  p->y=py_top/px_bot;
  /* checking if the lines segments intercept */
  if(in_line(l1,*p)==1) return 1;
  return 0;
}
int in_border(Figure * fig, Point2D point){
  /* checking if the given point is inside the figure */
  return (point.x>-(fig->width/2+DELTA) && point.x<(fig->width/2+DELTA) &&
          point.y>-(fig->height/2+DELTA) && point.y<(fig->height/2+DELTA));
}
void draw_polyline(Figure * fig, Point2D * polyline, int n){
  int i;
  Line line;
  Point2D s;
  Point2D e;
  if(fig==NULL) return;
  for(i=0;i<n-1;i++){
    line.start=polyline[i];
    line.end=polyline[i+1];
    line.color=fig->color;
    line.thickness=fig->thickness;
    enqueue(fig,line);
  }
  /* for clearing the lines outside of the figure */
  s.x=-fig->width/2;
  e.x=fig->width/2;
  s.y=-fig->height/2;
  e.y=fig->height/2;
  resize_figure(fig,s,e);
}
void delete_figure(Figure ** fig){
  Line junk;
  Text junk_t;
  if(*fig!=NULL){
    while(dequeue(*fig,&junk)==1);
    while(dequeue_t(*fig,&junk_t)==1);
    if((*fig)->lines!=NULL){
      free((*fig)->lines);
    }
    if((*fig)->texts!=NULL){
      free((*fig)->texts);
    }
    free(*fig);
    *fig=NULL;
  }
}
void scale_figure(Figure * fig, double scale_x, double scale_y){
  /* scales both lines and text coordinates */
  node_l * iter;
  node_t * itert;
  Point2D start_roi;
  Point2D end_roi;
  if(fig==NULL) return;
  iter = fig->lines->front;
  itert= fig->texts->front;
  while(iter!=NULL){
    iter->data.start.x*=scale_x;
    iter->data.start.y*=scale_y;
    iter->data.end.x*=scale_x;
    iter->data.end.y*=scale_y;
    iter=iter->next;
  }
  while(itert!=NULL){
    itert->data.pos.x*=scale_x;
    itert->data.pos.y*=scale_y;
    itert=itert->next;
  }
  /* for clearing the lines and texts outside of the figure */
  start_roi.x=-fig->width/2;
  end_roi.x=fig->width/2;
  start_roi.y=-fig->height/2;
  end_roi.y=fig->height/2;
  resize_figure(fig,start_roi,end_roi);
}
void append_figures(Figure * fig1, Figure * fig2){
  /* combining two figures */
  Queue_L * temp_ql;
  Queue_T * temp_qt;
  node_l * iter;
  Point2D start_roi;
  Point2D end_roi;
  Line templ;
  Text tempt;
  if(fig1==NULL) return;
  if(fig2==NULL) return;
  iter = fig1->lines->front;
  /* adds all the lines in fig1 to fig2 */
  while(dequeue(fig1,&templ)==1){
    enqueue(fig2,templ);
  }
  /* adds all the lines in fig1 to fig2 */
  while(dequeue_t(fig1,&tempt)==1){
    enqueue_t(fig2,tempt);
  }
  /* swapping datas */
  temp_ql = fig1->lines;
  temp_qt = fig1->texts;
  fig1->lines=fig2->lines;
  fig1->texts=fig2->texts;
  fig2->lines=temp_ql;
  fig2->texts=temp_qt;
  /* for clearing the lines outside of the figure */
  start_roi.x=-fig1->width/2;
  end_roi.x=fig1->width/2;
  start_roi.y=-fig1->height/2;
  end_roi.y=fig1->height/2;
  resize_figure(fig1,start_roi,end_roi);
  /* delete empty figure */
  delete_figure(&fig2);
}
void resize_figure(Figure * fig, Point2D start_roi, Point2D end_roi){
  /* resizes and crops the figure */
  node_l * iter;
  node_l * temp;
  node_t * itert;
  node_t * tempt;
  int inter_count;
  double x_diff,y_diff;
  Point2D inter_points[4];
  Line junk;
  Text junkt;
  if(fig==NULL) return;
  iter = fig->lines->front;
  itert= fig->texts->front;
  /* manipulating lines */
  if(iter!=NULL){
    fig->height=end_roi.y-start_roi.y;
    fig->width=end_roi.x-start_roi.x;
    x_diff=-fig->width/2-start_roi.x;
    y_diff=-fig->height/2-start_roi.y;
    /* for modifying, need to use pointer to pointer,
     * so can only use next, but this can't modify the first value,
     * first value will be managed later */
    while(iter->next!=NULL){
      iter->next->data.start.x+=x_diff;
      iter->next->data.end.x+=x_diff;
      iter->next->data.start.y+=y_diff;
      iter->next->data.end.y+=y_diff;
      if(in_border(fig,iter->next->data.start)==0 && in_border(fig,iter->next->data.end)==0){
        inter_count=border_intercepts(fig,iter->next->data,inter_points);
        /* delete part */
        if(inter_count == 0){
          /* if current iter->next is the end of the queue */
          if(iter->next==fig->lines->rear){
            fig->lines->rear=iter;
            free(iter->next);
            iter->next=NULL;
          }
          else{
            temp=iter->next;
            iter->next=iter->next->next;
            free(temp);
          }
        }
        else{
          iter->next->data.start=inter_points[0];
          iter->next->data.end=inter_points[1];
          iter=iter->next;
        }
      }
      else if(in_border(fig,iter->next->data.start)==1 && in_border(fig,iter->next->data.end)==1){
        iter=iter->next;
      }
      else{
        inter_count=border_intercepts(fig,iter->next->data,inter_points);
        if(in_border(fig,iter->next->data.start)==1){
          iter->next->data.end=inter_points[inter_count-1];
        }
        else{
          iter->next->data.start=inter_points[inter_count-1];
        }
        iter=iter->next;
      }
    }
    /* managing first line */
    fig->lines->front->data.start.x+=x_diff;
    fig->lines->front->data.start.y+=y_diff;
    fig->lines->front->data.end.x+=x_diff;
    fig->lines->front->data.end.y+=y_diff;
    if(in_border(fig,fig->lines->front->data.start)==0 && in_border(fig,fig->lines->front->data.end)==0){
      inter_count=border_intercepts(fig,fig->lines->front->data,inter_points);
      if(inter_count == 0){
        dequeue(fig,&junk);
      }
      else{
        fig->lines->front->data.start=inter_points[0];
        fig->lines->front->data.end=inter_points[1];
      }
    }
    else if(in_border(fig,fig->lines->front->data.start)==1 && in_border(fig,fig->lines->front->data.end)==1);
    else{
      inter_count=border_intercepts(fig,fig->lines->front->data,inter_points);
      if(in_border(fig,fig->lines->front->data.start)==1){
        fig->lines->front->data.end=inter_points[0];
      }
      else{
        fig->lines->front->data.start=inter_points[0];
      }
    }
  }
  /* manipulating texts */
  if(itert!=NULL){
    fig->texts->front->data.pos.y+=x_diff;
    fig->texts->front->data.pos.y+=y_diff;
    /* same situation again */
    while(itert->next!=NULL){
      itert->next->data.pos.x+=x_diff;
      itert->next->data.pos.y+=y_diff;
      if(in_border(fig,itert->next->data.pos)==1){
        itert=itert->next;
      }
      /* delete */
      else{
        /* if current iter->next is the end of the queue */
        if(itert->next==fig->texts->rear){
          fig->texts->rear=itert;
          free(itert->next);
          itert->next=NULL;
        }
        else{
          tempt=itert->next;
          itert->next=itert->next->next;
          free(tempt);
        }
      }
    }
    /* managing first text */
    if(in_border(fig,fig->texts->front->data.pos)==0){
      dequeue_t(fig,&junkt);
    }
  }
}
int tree_depth(Tree * root){
  /* calculating max depth of tree */
  int depth_l,depth_r;
  if (root==NULL) return 0;
  depth_l = tree_depth(root->left);
  depth_r = tree_depth(root->right);
  return max(depth_l,depth_r)+1;
}
void draw_binary_tree(Figure * fig, Tree * root){
  /* drawing binary file with respect to figure size and tree depth */
  Point2D pos,old_pos,s,e;
  int depth = tree_depth(root);
  int size=fig->height/(depth+5);
  double text_size=size/3;
  double y_diff = fig->height/depth+size/2;
  double x_diff = fig->width/4;
  pos.x=0;
  pos.y=fig->height/2-size/2-20; /* 20 px padding top */
  old_pos.x=EMPTY_COORDS;
  old_pos.y=EMPTY_COORDS;
  bin_tree(fig,root,size,text_size,pos,y_diff,x_diff,old_pos);
  s.x=-fig->width/2;
  e.x=fig->width/2;
  s.y=-fig->height/2;
  e.y=fig->height/2;
  resize_figure(fig,s,e);
}
void bin_tree(Figure * fig, Tree * root, double size, double text_size, Point2D pos, double y_diff, double x_diff, Point2D old_pos){
  Text t;
  Line l;
  Point2D newposl, newposr;
  if(root==NULL) return;
  /* drawing lines between nodes */
  if(old_pos.x!=EMPTY_COORDS&&old_pos.y!=EMPTY_COORDS){
    l.color = fig->color;
    l.thickness = fig->thickness;
    l.start.x = old_pos.x;
    l.start.y = old_pos.y-size*10/16;
    l.end.x = pos.x;
    l.end.y = pos.y+size/2;
    enqueue(fig,l);
  }
  draw_circle(fig,&pos,size/2);
  /* adding text to inside of node */
  t.size=text_size;
  t.pos.x=pos.x-t.size*2/3;
  t.pos.y=pos.y-t.size/2+1;
  t.color=fig->color;
  sprintf(t.text,"%3d",root->data);
  enqueue_t(fig,t);
  /* changing position */
  newposl.x=pos.x-x_diff;
  newposr.x=pos.x+x_diff;
  newposl.y=newposr.y=pos.y-y_diff;
  bin_tree(fig,root->left,size*0.8,text_size*0.8,newposl,y_diff*0.9,x_diff/2,pos);
  bin_tree(fig,root->right,size*0.8,text_size*0.8,newposr,y_diff*0.9,x_diff/2,pos);
}
void draw_koch_snowflake(Figure * fig, Point2D * center, double thickness, int size, int num_iterations){
  Point2D p1,p2,p3;
  double deg;
  Point2D s,e;
  if(num_iterations<0) return;
  if(fig==NULL) return;
  p1.x=center->x;
  p1.y=center->y+size;
  p2.x=center->x+sqrt(3)*size/2;
  p2.y=p3.y=center->y-size/2;
  p3.x=center->x-sqrt(3)*size/2;
  /* drawing 3 koch line for triangle look */
  deg=-M_PI/3;
  koch(p1,p2,num_iterations,deg,fig,thickness);
  deg=-M_PI;
  koch(p2,p3,num_iterations,deg,fig,thickness);
  deg=M_PI/3;
  koch(p3,p1,num_iterations,deg,fig,thickness);
  s.x=-fig->width/2;
  e.x=fig->width/2;
  s.y=-fig->height/2;
  e.y=fig->height/2;
  resize_figure(fig,s,e);
}
void koch(Point2D s, Point2D e, int num_iterations, double deg, Figure * fig,double thickness){
  Line line;
  Point2D p1,p2,p3,p4;
  double len;
  if(num_iterations<=0){
    line.color=fig->color;
    line.thickness=thickness;
    line.start=s;
    line.end=e;
    enqueue(fig,line);
    return;
  }
  len=length(s,e);
  p1.x=s.x+len*cos(deg)/3;
  p1.y=s.y+len*sin(deg)/3;
  p2.x=p1.x+len*cos(deg+M_PI/3)/3;
  p2.y=p1.y+len*sin(deg+M_PI/3)/3;
  p3.x=s.x+len*2*cos(deg)/3;
  p3.y=s.y+len*2*sin(deg)/3;
  p4.x=s.x+len*cos(deg);
  p4.y=s.y+len*sin(deg);
  koch(s,p1,num_iterations-1,deg,fig,thickness*0.9);
  koch(p1,p2,num_iterations-1,deg+M_PI/3,fig,thickness*0.9);
  koch(p2,p3,num_iterations-1,deg-M_PI/3,fig,thickness*0.9);
  koch(p3,p4,num_iterations-1,deg,fig,thickness*0.9);
}
void draw_fractal_tree(Figure * fig,double center_x, double center_y, int size, int num_iterations){
  double len = (double)size;
  Point2D s,e;
  if(fig==NULL) return;
  if(size<0) return;
  if(num_iterations<=0) return;
  tree(fig,center_x,center_y-size*2,len,M_PI/2,num_iterations,M_PI/6,M_PI/6,fig->thickness);
  s.x=-fig->width/2;
  e.x=fig->width/2;
  s.y=-fig->height/2;
  e.y=fig->height/2;
  resize_figure(fig,s,e);
}
void tree(Figure * fig,double sx, double sy, double len, double deg, int num_iterations, double dl, double dr,double thickness){
  Line line;
  double len_rate=0.8;
  if(num_iterations>=0 && len>0){
    line.color=fig->color;
    line.thickness=thickness;
    line.start.x=sx;
    line.start.y=sy;
    line.end.x=sx+len*cos(deg);
    line.end.y=sy+len*sin(deg);
    enqueue(fig,line);
    tree(fig,line.end.x,line.end.y,len*len_rate,deg-dr,num_iterations-1,dl,dr,thickness*0.8);
    tree(fig,line.end.x,line.end.y,len*len_rate,deg+dl,num_iterations-1,dl,dr,thickness*0.8);
  }
}
void draw_fractal_atree(Figure * fig,double center_x, double center_y, int size, int num_iterations){
  double len = (double)size;
  Point2D s,e;
  if(fig==NULL) return;
  if(size<0) return;
  if(num_iterations<=0) return;
  tree(fig,center_x,center_y-size*2,len,M_PI/2,num_iterations,M_PI/12,M_PI/6,fig->thickness);
  s.x=-fig->width/2;
  e.x=fig->width/2;
  s.y=-fig->height/2;
  e.y=fig->height/2;
  resize_figure(fig,s,e);
}
void draw_ellipse(Figure * fig, Point2D * center, double horizontal_r, double vertical_r){
  /* drawing ellipse by using a unit cirle */
  Point2D old,cur;
  double angle;
  int there_is_old=0;
  Line line;
  if(fig==NULL) return;
  line.color=fig->color;
  line.thickness=fig->thickness;
  if(horizontal_r>0&&vertical_r>0){
    for(angle=0.0;angle<2*M_PI-DELTA;angle+=1/fig->resolution){
      circle(angle,&cur);
      cur.x*=horizontal_r;
      cur.y*=vertical_r;
      cur.x+=center->x;
      cur.y+=center->y;
      if(in_border(fig,cur)){
        if(there_is_old==1){
          line.start=old;
          line.end=cur;
          enqueue(fig,line);
          old = cur;
        }
        else{
          old = cur;
          there_is_old = 1;
        }
      }
      else{
        there_is_old=0;
      }
    }
    circle(0,&cur);
    cur.x*=horizontal_r;
    cur.x+=center->x;
    cur.y*=vertical_r;
    cur.y+=center->y;
    line.start=old;
    line.end=cur;
    enqueue(fig,line);
  }
}
double length(Point2D p1, Point2D p2){
  /* distance of 2 points */
  return sqrt((p2.x-p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y));
}
double min(double a, double b){
  if(a<b) return a;
  return b;
}
double max(double a, double b){
  if(a>b) return a;
  return b;
}
void import_svg(char * file_name, Figure * fig){
  /* this function only accepts this type of svg lines:
   * <svg width="w" height="h" (..)>
   *
   * <path d="Mx1 y1 Lx2 y2 Z>" style="(..);stroke-width:x;stroke:rgb(r,g,b)" (..)/>
   * <path d="Mx1 y1 Lx2 y2 Z>" style="(..);stroke-width:x;stroke:rgb(r,g,b)" (..)/>
   *
   * <line x1="x1" y1="y1" x2="x2" y2="y2" style="(..);stroke-width:x;stroke:rgb(r,g,b)" (..)/>
   * <line x1="x1" y1="y1" x2="x2" y2="y2" style="(..);stroke-width:x;stroke:rgb(r,g,b)" (..)/>
   *
   * <text x="x" y="y" font-size="size" style="(..)fill:rgb(r,g,b)" (..)>XXX</text>
   * <text x="x" y="y" font-size="size" style="(..)fill:rgb(r,g,b)" (..)>XXX</text> */
  FILE *fp = fopen(file_name, "r");
  char row[256];
  char * cur_pos;
  Line line;
  Text text;
  Color stock_color = {0,0,0};
  int stock_thickness = 1;
  int acceptable;
  if(fp==NULL) return;
  while(!feof(fp)){
    acceptable = 1;
    fgets(row,255,fp);
    if(strstr(row,"<svg")!=NULL){
      /* <svg width="w" height="h" (..)>*/
      cur_pos = strstr(row,"width=");
      if(cur_pos!=NULL) sscanf(cur_pos+7,"%lf",&(fig->width));
      cur_pos = strstr(row,"height=");
      if(cur_pos!=NULL) sscanf(cur_pos+8,"%lf",&(fig->height));
    }
    else if(strstr(row,"path")!=NULL){
      /* <path d="Mx1 y1 Lx2 y2 Z>" style="(..);stroke-width:x;stroke:rgb(r,g,b)" (..)/>*/
      line.color=stock_color;
      line.thickness=stock_thickness;
      cur_pos=strstr(row,"d");
      if(cur_pos!=NULL){
        sscanf(cur_pos+4,"%lf %lf%*c%*c%lf %lf",&(line.start.x),&(line.start.y),&(line.end.x),&(line.end.y));
        line.start.x-=fig->width/2;
        line.start.y-=fig->height/2;
        line.start.y=-line.start.y;
        line.end.x-=fig->width/2;
        line.end.y-=fig->height/2;
        line.end.y=-line.end.y;
      }
      else{
        acceptable=0;
      }
      cur_pos = strstr(row,"stroke-width:");
      if(cur_pos!=NULL){
        sscanf(cur_pos+13,"%lf",&(line.thickness));
      }
      cur_pos = strstr(row,"stroke:rgb(");
      if(cur_pos!=NULL){
        sscanf(cur_pos+11,"%d%*c%d%*c%d",&(line.color.r),&(line.color.g),&(line.color.b));
      }
      if(acceptable==1){
        enqueue(fig,line);
      }
    }
    else if(strstr(row,"line")!=NULL){
      /* <line x1="x1" y1="y1" x2="x2" y2="y2" style="(..);stroke-width:x;stroke:rgb(r,g,b)" (..)/>*/
      line.color=stock_color;
      line.thickness=stock_thickness;
      cur_pos=strstr(row,"x1=");
      if(cur_pos!=NULL) sscanf(cur_pos+4,"%lf",&(line.start.x));
      else acceptable=0;
      cur_pos=strstr(row,"y1=");
      if(cur_pos!=NULL) sscanf(cur_pos+4,"%lf",&(line.start.y));
      else acceptable=0;
      cur_pos=strstr(row,"x2=");
      if(cur_pos!=NULL) sscanf(cur_pos+4,"%lf",&(line.end.x));
      else acceptable=0;
      cur_pos=strstr(row,"y2=");
      if(cur_pos!=NULL) sscanf(cur_pos+4,"%lf",&(line.end.y));
      else acceptable=0;
      cur_pos = strstr(row,"stroke-width:");
      if(cur_pos!=NULL){
        sscanf(cur_pos+13,"%lf",&(line.thickness));
      }
      cur_pos = strstr(row,"stroke:rgb(");
      if(cur_pos!=NULL){
        sscanf(cur_pos+11,"%d%*c%d%*c%d",&(line.color.r),&(line.color.g),&(line.color.b));
      }
      if(acceptable==1){
        line.start.x-=fig->width/2;
        line.start.y-=fig->height/2;
        line.start.y=-line.start.y;
        line.end.x-=fig->width/2;
        line.end.y-=fig->height/2;
        line.end.y=-line.end.y;
        enqueue(fig,line);
      }
    }
    else if(strstr(row,"text")!=NULL){
      /* <text x="x" y="y" font-size="size" style="(..)fill:rgb(r,g,b)" (..)>XXX</text>*/
      text.color=stock_color;
      cur_pos=strstr(row,"x=");
      if(cur_pos!=NULL) sscanf(cur_pos+3,"%lf",&(text.pos.x));
      else acceptable=0;
      cur_pos=strstr(row,"y=");
      if(cur_pos!=NULL) sscanf(cur_pos+3,"%lf",&(text.pos.y));
      else acceptable=0;
      cur_pos=strstr(row,"font-size=");
      if(cur_pos!=NULL) sscanf(cur_pos+11,"%lf",&(text.size));
      else acceptable=0;
      cur_pos = strstr(row,"</t");
      if(cur_pos!=NULL){
        strncpy((char*)&(text.text),cur_pos-3,3);
        text.text[3]='\0';
      }
      else acceptable=0;
      cur_pos = strstr(row,"stroke:rgb(");
      if(cur_pos!=NULL){
        sscanf(cur_pos+11,"%d%*c%d%*c%d",&(text.color.r),&(text.color.g),&(text.color.b));
      }
      if(acceptable==1){
        text.pos.x-=fig->width/2;
        text.pos.y-=fig->height/2;
        text.pos.y=-text.pos.y;
        enqueue_t(fig,text);
      }
    }
  }
  fclose(fp);
}
void export_svg(Figure * fig, char * file_name){
  Line line;
  Text text;
  FILE *fp;
  if(fig!=NULL){
    fp = fopen(file_name, "w");
    fprintf(fp,"<svg width=\"%.0f\" height=\"%.0f\" ",fig->width,fig->height);
    fprintf(fp,"xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink= \"http://www.w3.org/1999/xlink\">\n");
    while(dequeue(fig,&line)==1){
      fprintf(fp,"<path d=\"M%f %f L%f %f Z>\" ",align(fig,line.start).x,fig->height-align(fig,line.start).y,align(fig,line.end).x,fig->height-align(fig,line.end).y);
      fprintf(fp,"style=\"fill:none;stroke-width:%.2f;",line.thickness);
      fprintf(fp,"stroke:rgb(%d,%d,%d)\"/>\n",line.color.r,line.color.g,line.color.b);
    }
    while(dequeue_t(fig,&text)==1){
      fprintf(fp, "<text x=\"%.4f\" y=\"%.4f\" font-size=\"%.2f\"", align(fig,text.pos).x,fig->height-align(fig,text.pos).y,text.size);
      fprintf(fp, " style=\"fill:rgb(%d,%d,%d)\">%s</text>\n", text.color.r,text.color.g,text.color.b,text.text);
    }
    fprintf(fp, "</svg>");
    fclose(fp);
    delete_figure(&fig);
  }
}
void export_eps(Figure * fig, char * file_name){
  Line line;
  FILE *fp;
  Text text;
  if(fig!=NULL){
    fp = fopen(file_name, "w");
    fprintf(fp,"%%!PS-Adobe-3.0 EPSF-3.0\n");
    fprintf(fp,"%%%%BoundingBox:%.0f %.0f %.0f %.0f\n",-fig->width/2,-fig->height/2,fig->width/2,fig->height/2);
    while(dequeue(fig,&line)==1){
      fprintf(fp,"newpath\n%.4f %.4f moveto\n%.4f %.4f lineto\n",line.start.x,line.start.y,line.end.x,line.end.y);
      fprintf(fp,"%.2f setlinewidth\n%.2f %.2f %.2f setrgbcolor\nstroke\n",line.thickness,(double)line.color.r/255,(double)line.color.g/255,(double)line.color.b/255);
    }
    while(dequeue_t(fig,&text)==1){
      fprintf(fp,"/Times-Roman %.2f selectfont\n%.2f %.2f %.2f setrgbcolor",text.size,(double)text.color.r,(double)text.color.g,(double)text.color.b);
      fprintf(fp,"\nnewpath\n%.4f %.4f moveto (%s) show\n",text.pos.x,text.pos.y,text.text);
    }
    fprintf(fp,"%%%%eof");
    fclose(fp);
    delete_figure(&fig);
  }
}
