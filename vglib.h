#ifndef VGLIB_H
#define VGLIB_H

#define ALLOC(p,n) ((p*)malloc((n)*sizeof(p)))

typedef struct{
  int r;
  int g;
  int b;
} Color;

typedef struct{
  double x;
  double y;
}Point2D;

typedef struct{
  Point2D start;
  Point2D end;
  Color color;
  double thickness;
}Line;

typedef struct{
  Point2D pos;
  Color color;
  double size;
  char text[4];
}Text;

typedef struct node_l_s{
  Line data;
  struct node_l_s * next;
}node_l;

typedef struct node_t_s{
  Text data;
  struct node_t_s * next;
}node_t;

typedef struct{
  node_l * front;
  node_l * rear;
}Queue_L;

typedef struct{
  node_t * front;
  node_t * rear;
}Queue_T;
typedef struct figure{
  double width;
  double height;
  double thickness;
  double resolution;
  Color color;
  Queue_L * lines;
  Queue_T * texts;
} Figure;

typedef struct tree{
  int data;
  struct tree *left, *right;
}Tree;

Figure * start_figure(double width, double height);
void set_thickness_resolution(Figure  *  fig, double  thickness, double resolution);
void set_color(Figure * fig, Color c);
void draw_fx(Figure * fig, double f(double x),double start_x, double end_x);
void draw_circle(Figure * fig, Point2D * center, double r);
void draw_polyline(Figure * fig, Point2D * polyline, int n);
void scale_figure(Figure * fig, double scale_x, double scale_y);
void append_figures(Figure * fig1, Figure * fig2);
void resize_figure(Figure * fig1, Point2D start_roi, Point2D end_roi);
void draw_ellipse(Figure * fig, Point2D * center, double horizontal_r, double vertical_r);
void export_svg(Figure * fig, char * file_name);
void export_eps(Figure * fig, char * file_name);
void import_svg(char * file_name, Figure * fig);
void draw_koch_snowflake(Figure * fig, Point2D * center, double thickness, int size, int num_iterations);
void draw_fractal_tree(Figure * fig,double center_x, double center_y, int size, int num_iterations);
void draw_fractal_atree(Figure * fig,double center_x, double center_y, int size, int num_iterations);
void draw_binary_tree(Figure * fig, Tree * root);

void delete_figure(Figure ** fig);
Point2D new_point(double x, double y);
#endif
