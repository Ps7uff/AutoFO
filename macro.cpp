// compile this like g++ macro2.cpp -lgdi32 -O2 -fopenmp -o macro

#include <windows.h>
#include <sys/time.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
#include <algorithm>
#include "capture.h"
#include "mouse.h"
#include "training.h"
# include <cstdlib>
# include <iostream>
# include <fstream>
# include <ctime>
# include <cstring>

double prev_correction = 999;
int img_cnt = 0;
double loc = 0;
double broke_loc;
int cons_flag = 0;
int corr_flag = 0;
int adj_flag = 0;
int corr_dir = 1;
int out_flag = 0;
int out_flag_m = 0;
int out_num = 0;
int broke_flag = 0;
int succ_flag = 0;
int home_flag = 0;
int start_flag = 1;
double theta = -1.5708;
double prev_theta = -1.5708;
double loc_theta = -1.5708;
double pix[2] = {0,0};
double loc_pix = 0;
int broke;
int prev_x = 0;
int prev_y = 0;
int first_x = 0;
int loc_prev_x = 0;
int loc_prev_y = 26;
int loc_first_x = 0;
double prev_pix=0;
int press_time = 200;
double tol = 0.03;
double nudge = 1.2;

int avg_pin;
int avg_ss;

const int x1_adj = 318+156+24;
const int x2_adj = 320+156+24;
const int y1_adj = 291+103+29-29;
const int y2_adj = 423+103+28;
const int width = 1080-(x2_adj+x1_adj);
const int height = 1080-(y2_adj+y1_adj);
const int width_m = 1500;
const int height_m = 62;


const int save_flag = 0;
const int sample_flag = 0;
const int save_flag_m = 0;
const int sample_flag_m = 0;

void ScreenCapture_m(int x, int y, char *filename){
  // get a DC compat. w/ the screen
  HDC hDc = CreateCompatibleDC(0);

  // make a bmp in memory to store the capture in
  HBITMAP hBmp = CreateCompatibleBitmap(GetDC(0), width_m, height_m);

  // join em up
  SelectObject(hDc, hBmp);

  // copy from the screen to my bitmap
  BitBlt(hDc, 0, 0, width_m, height_m, GetDC(0), x, y, SRCCOPY);

  BITMAPINFO MyBMInfo = {0};
  MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

  if(0 == GetDIBits(hDc, hBmp, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS)) {
    //error handling
  }
  //int* exp_flags = (int*)malloc(width*height*sizeof(int));
  BYTE* Pixels = (BYTE*)malloc(4*width_m*height_m);

  MyBMInfo.bmiHeader.biBitCount = 32;
  MyBMInfo.bmiHeader.biCompression = BI_RGB;  // no compression -> easier to use
  // correct the bottom-up ordering of lines (abs is in cstdblib and stdlib.h)
  MyBMInfo.bmiHeader.biHeight = -height_m;
  MyBMInfo.bmiHeader.biWidth = width_m;

  // Call GetDIBits a second time, this time to (format and) store the actual
  // bitmap data (the "pixels") in the buffer lpPixels
  if(0 == GetDIBits(hDc, hBmp, 0, MyBMInfo.bmiHeader.biHeight,
                    Pixels, &MyBMInfo, DIB_RGB_COLORS)) {
    //error handling
  }
  int x_coord, y_coord;
  int sum_pin = 0;
  int num_pin = 0;
  int sum_ss = 0;
  int num_ss = 0;
  y_coord = 1;
  for (int x_coord = 0; x_coord<width_m; x_coord++)
  { 
    if ((Pixels[4*((x_coord)+(y_coord)*width_m)] == 204) &&
    (Pixels[4*((x_coord)+(y_coord)*width_m)+1] == 204) &&
    (Pixels[4*((x_coord)+(y_coord)*width_m)+2] == 204)) {
      if (save_flag_m == 1) {
        Pixels[4*((x_coord)+(y_coord)*width_m)] = 255;
        Pixels[4*((x_coord)+(y_coord)*width_m)+1] = 0;
        Pixels[4*((x_coord)+(y_coord)*width_m)+2] = 255;
      }
      sum_pin = sum_pin + x_coord;
      num_pin++;
    }
  }
  if (num_pin != 0) {
    avg_pin = sum_pin/num_pin;
  } else {
    avg_pin = 0;
  }
  y_coord = 52;
  for (int x_coord = 0; x_coord<width_m; x_coord++)
  { 
    if ((Pixels[4*((x_coord)+(y_coord)*width_m)] != 255) &&
    (Pixels[4*((x_coord)+(y_coord)*width_m)+1] != 255) &&
    (Pixels[4*((x_coord)+(y_coord)*width_m)+2] != 255)) {
      if (save_flag_m == 1) {
        Pixels[4*((x_coord)+(y_coord)*width_m)] = 0;
        Pixels[4*((x_coord)+(y_coord)*width_m)+1] = 255;
        Pixels[4*((x_coord)+(y_coord)*width_m)+2] = 255;
      }
      sum_ss = sum_ss + x_coord;
      num_ss++;
    }
  }
  avg_ss = sum_ss/num_ss;
  if (num_ss < 150) {
    avg_ss = sum_ss/num_ss;
  } else {
    avg_ss = 0;
    out_flag_m = 0;
    out_num = 0;
  }
  if (save_flag_m == 1) {
    for (x_coord = 0; x_coord<width_m; x_coord++) {
      for (y_coord = 0; y_coord < height_m; y_coord++)
      { 
        SetPixel(hDc, x_coord, y_coord, RGB(Pixels[4*((x_coord)+(y_coord)*width_m)+2],
                                            Pixels[4*((x_coord)+(y_coord)*width_m)+1],
                                            Pixels[4*((x_coord)+(y_coord)*width_m)]));
      }
    }
    bool ret = SaveBMPFile(filename, hBmp, hDc, width_m, height_m);
    printf("%d %d\n", avg_ss, avg_pin);
  }

  DeleteObject(hBmp);
  DeleteDC(hDc);
  free(Pixels);
  /*if ((h_pix>10) && (v_pix>10)) {
    return (double)v_pix-h_pix;*/
  return;


  // free the bitmap memory

  //return ret;
}

void ScreenCapture(int x, int y, char *filename){
  // get a DC compat. w/ the screen
  HDC hDc = CreateCompatibleDC(0);

  // make a bmp in memory to store the capture in
  HBITMAP hBmp = CreateCompatibleBitmap(GetDC(0), width, height);

  // join em up
  SelectObject(hDc, hBmp);

  // copy from the screen to my bitmap
  BitBlt(hDc, 0, 0, width, height, GetDC(0), x, y, SRCCOPY);

  BITMAPINFO MyBMInfo = {0};
  MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader);

  if(0 == GetDIBits(hDc, hBmp, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS)) {
    //error handling
  }
  //int* exp_flags = (int*)malloc(width*height*sizeof(int));
  BYTE* Pixels = (BYTE*)malloc(4*width*height);

  MyBMInfo.bmiHeader.biBitCount = 32;
  MyBMInfo.bmiHeader.biCompression = BI_RGB;  // no compression -> easier to use
  // correct the bottom-up ordering of lines (abs is in cstdblib and stdlib.h)
  MyBMInfo.bmiHeader.biHeight = -height;
  MyBMInfo.bmiHeader.biWidth = width;

  // Call GetDIBits a second time, this time to (format and) store the actual
  // bitmap data (the "pixels") in the buffer lpPixels
  if(0 == GetDIBits(hDc, hBmp, 0, MyBMInfo.bmiHeader.biHeight,
                    Pixels, &MyBMInfo, DIB_RGB_COLORS)) {
    //error handling
  }
  int x_coord, x_coord2, y_coord, y_coord2;
  int max_flag = 0;
  int loc_max_flag = 0;
  int loc_flag = 0;
  prev_x = 0;
  prev_y = 0;
  first_x = 0;
  loc_prev_x = 0;
  loc_prev_y = 26;
  loc_first_x = 0;
  pix[0] = 0;
  pix[1] = 0;
  loc_pix = 0;
  broke = 0;

  x_coord = width/4-5;
  for (y_coord = 29; y_coord < height; y_coord++)
  { 
    if ((Pixels[4*((x_coord)+(y_coord)*width)] < 2) &&
    (Pixels[4*((x_coord)+(y_coord)*width)+1] < 2) &&
    (Pixels[4*((x_coord)+(y_coord)*width)+2] < 2)) {
      
      Pixels[4*((x_coord)+(y_coord)*width)] = 255;
      Pixels[4*((x_coord)+(y_coord)*width)+1] = 255;
      Pixels[4*((x_coord)+(y_coord)*width)+2] = 0;
      pix[0]++;
    }
  }

  y_coord = 8+29;
  for (int x_coord = 0; x_coord<width; x_coord++)
  { 
    if ((Pixels[4*((x_coord)+(y_coord)*width)] < 2) &&
    (Pixels[4*((x_coord)+(y_coord)*width)+1] < 2) &&
    (Pixels[4*((x_coord)+(y_coord)*width)+2] < 2)) {
      if (save_flag == 1) {
        Pixels[4*((x_coord)+(y_coord)*width)] = 255;
        Pixels[4*((x_coord)+(y_coord)*width)+1] = 0;
        Pixels[4*((x_coord)+(y_coord)*width)+2] = 255;
      }
      broke++;
    }
  }

  y_coord = 3*(height-29)/4-1+29;

  for (int x_coord = 0; x_coord<width; x_coord++)
  { 
    if ((Pixels[4*((x_coord)+(y_coord)*width)] < 2) &&
    (Pixels[4*((x_coord)+(y_coord)*width)+1] < 2) &&
    (Pixels[4*((x_coord)+(y_coord)*width)+2] < 2)) {
      if (save_flag == 1) {
        Pixels[4*((x_coord)+(y_coord)*width)] = 0;
        Pixels[4*((x_coord)+(y_coord)*width)+1] = 255;
        Pixels[4*((x_coord)+(y_coord)*width)+2] = 255;
      }
      pix[1]++;
    }
  }
  for (y_coord2 = 26; y_coord2>0; y_coord2--) {
    loc_flag = 0;
    for (int x_coord = width/4; x_coord<width; x_coord++)
    { 
      if ((Pixels[4*((x_coord)+(y_coord2)*width)] < 2) &&
          (Pixels[4*((x_coord)+(y_coord2)*width)+1] < 2) &&
          (Pixels[4*((x_coord)+(y_coord2)*width)+2] < 2) && (loc_max_flag==0 && loc_flag == 0)) {
        if (save_flag == 1) {
          Pixels[4*((x_coord)+(y_coord2)*width)] = 255;
          Pixels[4*((x_coord)+(y_coord2)*width)+1] = 255;
          Pixels[4*((x_coord)+(y_coord2)*width)+2] = 255;
        }
        if (y_coord2 == 26 && loc_first_x < x_coord) {
          loc_first_x = x_coord;
        }
        if (y_coord2 <= loc_prev_y) {
          if (x_coord == width/4) {
            loc_prev_x = 0;
          }
          if (x_coord > loc_prev_x) {
            loc_prev_x = x_coord;
            loc_prev_y = y_coord2;
          }
        }
        if (x_coord > width-(width/4)) {  
          loc_max_flag = 1;
        }
      } else {
        loc_flag = 1;
      }

    }
  }

  for (y_coord2 = 3*(height-29)/4-2+29; y_coord2>(height-29)/3+29; y_coord2--) {
    for (int x_coord = 0; x_coord<width; x_coord++)
    { 
      if ((Pixels[4*((x_coord)+(y_coord2)*width)] < 2) &&
          (Pixels[4*((x_coord)+(y_coord2)*width)+1] < 2) &&
          (Pixels[4*((x_coord)+(y_coord2)*width)+2] < 2) && (max_flag==0)) {
        if (save_flag == 1) {
          Pixels[4*((x_coord)+(y_coord2)*width)] = 0;
          Pixels[4*((x_coord)+(y_coord2)*width)+1] = 255;
          Pixels[4*((x_coord)+(y_coord2)*width)+2] = 255;
        }
        if (y_coord2 == 3*(height-29)/4-2+29 && first_x < x_coord) {
          first_x = x_coord;
        }
        if (prev_x < x_coord) {
          prev_x = x_coord;
          prev_y = y_coord2;
        }
        if (prev_x > width-2 || y_coord2 == (height-29)/2+29) {  
          max_flag = 1;
        }
      }
    }
  }
  if (save_flag == 1) {
    for (x_coord = 0; x_coord<width; x_coord++) {
      for (y_coord = 0; y_coord < height; y_coord++)
      { 
        SetPixel(hDc, x_coord, y_coord, RGB(Pixels[4*((x_coord)+(y_coord)*width)+2],
                                            Pixels[4*((x_coord)+(y_coord)*width)+1],
                                            Pixels[4*((x_coord)+(y_coord)*width)]));
      }
    }
    bool ret = SaveBMPFile(filename, hBmp, hDc, width, height);
  }

  DeleteObject(hBmp);
  DeleteDC(hDc);
  free(Pixels);
  /*if ((h_pix>10) && (v_pix>10)) {
    return (double)v_pix-h_pix;*/
  if (pix[0]>10.0 && pix[1]>10.0) {
    return;
  } else {
    pix[0] = 0.0;
    pix[1] = 0.0;
    return;
  }


  // free the bitmap memory

  //return ret;
}
void lockpick_m() {
  char buf[6];
  double correction;
  sprintf(buf, "Screenshots/%d.bmp", img_cnt);
  std::clock_t start;
  start = std::clock();
  ScreenCapture_m(210, 0, (char *)buf);
  
  if (sample_flag_m == 1) {
    if (avg_pin < 10) {
      MouseMove(-100,0);
    }
    if (press_time != 0) {
      //Abutton(press_time);
      if (img_cnt > 20) {
        printf("%d, %d\n", avg_pin, ((img_cnt)-int(img_cnt/7)*7));
        MouseMove(((img_cnt)-int(img_cnt/7)*7),0);
      } else {
        printf("%d, %d\n", avg_pin, (img_cnt));
        MouseMove((img_cnt+1),0);
      }
      
      Sleep(press_time);
    }
  } else {
    if (out_flag_m == 1) {
      Sleep(100);
      out_num++;
      if (out_num > 20) {
        out_flag_m = 0;
        out_num = 0;
      }
    } else if (avg_ss == 0) {
      Ebutton();
      Sleep(500);
    } else if (abs(avg_pin-avg_ss)>10) {
      MouseMove((avg_pin-avg_ss)*(1.0/78.765),0);
      //0.0332
      Sleep(100);
    } else {
      Abutton_press();
      Sleep(1200);
      Abutton_release();
      out_flag_m = 1;
    }
  }

}
void lockpick() {
  char buf[6];
  double correction;
  sprintf(buf, "Screenshots/%d.bmp", img_cnt);
  std::clock_t start;
  start = std::clock();
  theta = 0.0;
  ScreenCapture((1920-1080)/2+x1_adj, 0+y1_adj, (char *)buf);
  printf("%f\t%f\n", pix[0], pix[1]);
  if (pix[0] > 100.0 && (pix[1]>51.0 && pix[1]<54.0)) {
    if (succ_flag == 1 || out_flag == 1) {
      prev_theta = -1.5708;
      cons_flag = 0;
      if (out_flag == 1) {
        Sleep(1000);
      }
    }
    home_flag = 1;
    succ_flag = 0;
    out_flag = 0;
  } else if (pix[1] > 65.0 && (pix[0]>51.0 && pix[0]<54.0) && theta == 0.0 ) {
    home_flag = 0;
    succ_flag = 1;
    out_flag = 0;
  } else if (pix[0] < 5 && pix[1] < 5) {
    Abutton_release();
    home_flag = 0;
    succ_flag = 0;
    out_flag = 1;
  } else {
    home_flag = 0;
    //succ_flag = 0;
    out_flag = 0;
  }
  if (loc_first_x == 0) {
    loc_theta = -3.1416;
  } else if (loc_prev_y == 26) {
    loc_theta = 0.0;
  } else {
    loc_theta = atan((double)(26 - loc_prev_y)/(double)(loc_first_x-loc_prev_x));
    if (loc_theta > 0.0) {
      loc_theta = loc_theta - 3.1416;
    }
  }
  if (home_flag == 1) {
    loc = (loc_theta+1.5708) * 0.3319 * 180 / 3.1416;
  }
  //printf("%d %d\n",prev_x,prev_y );

  if (succ_flag == 1) {
    theta = 0.0;
  } else if (out_flag == 0) {
    if (first_x<prev_x) { 
      theta = atan((double)(3*(height-29)/4-1+29 - prev_y)/(double)(first_x-prev_x));
    } else if (home_flag == 1) {
      theta = -1.5708;
    }
    //printf("%f, %f, %f, %f, %f\n",theta, (double)(3*height/4-1),(double)prev_y,(double)first_x,(double)prev_x);
  } else {
    theta = -1.5708;
  } 
  if (theta>1.0) {
    theta = -1.5708;
  } else if (theta>0.0) {
    theta = 0.0;
  }
  //printf("%d %d %d %d %d\n", home_flag, succ_flag, out_flag, cons_flag, corr_flag);
  printf("%s %f\n",buf, theta);
  if (sample_flag == 1) {
      if (img_cnt > 5) {
        exit(1);
      }
      printf("%d\n", theta);
      if (press_time != 0) {
        //Abutton(press_time);
        Abutton_press();
        Sleep(press_time);
      }
      /*Sleep(500);
      if (succ_flag == 0 && out_flag == 0) {
        MouseMove(5,0);
      }
      Sleep(500);
      printf("fx, x, y, angle: %d, %d, %d, %f\n", 
        loc_first_x, loc_prev_x, loc_prev_y,loc_theta);*/

  } else if (out_flag == 1) {
    adj_flag = 0;
    start_flag = 1;
    prev_correction = 999;
    Ebutton();
    Sleep(500);
    printf("reopen\n");
    corr_flag = 0;
    cons_flag = 0;
    prev_theta = -1.5708;
  } else {
    printf("%f %f Diff: %f\n", theta, prev_theta, theta-prev_theta);
    if (broke>50) {
          prev_theta = -1.5708;
          prev_correction = 999;
          adj_flag = 0;
          start_flag = 1;
          corr_flag = 0;
          cons_flag = 0;
          Abutton_release();
          printf("broke_flag\n");
          Sleep(1000);
    } else if (((theta-prev_theta < tol && start_flag == 0) || theta<prev_theta)
        && out_flag == 0) {
      Abutton_release();
      //cons_flag = 1;
      /*if (cons_flag == 0) {
        cons_flag = 1;
        start_flag = 0;
        if (press_time != 0) {
          //Abutton(press_time);
          Abutton_press();
          Sleep(press_time);
        }
        prev_theta = theta;
        printf("correcting next %d\n",home_flag);
      } else {*/
        start_flag = 1;
        printf("correcting %f\n", theta);
        if (home_flag == 1) {
            correction = 100*-1.5708/-1.5708;
            correction = correction*(38.3/100.0)*0.3319+14*0.3319;
        } else { 
            correction = 100*theta/-1.5708;
            correction = correction*(38.3/100.0)*0.3319+14*0.3319;
        }
        if (correction>prev_correction) {
          corr_dir = -1*corr_dir;
          printf("reversal\n");
        }
        printf("corr: %f\n",correction );
        if (loc+corr_dir*correction>30 || loc+corr_dir*correction<-30) {
          printf("turnaround\n");
          corr_dir = -1*corr_dir;/*
          if (succ_flag == 0 && out_flag == 0) {
            MouseMove(corr_dir*prev_correction,0);
          }
          Sleep(50);*/
          if (succ_flag == 0 && out_flag == 0) {
            MouseMove(corr_dir*correction,0);
          }
          Sleep(500);
          prev_theta = -1.5708;
          cons_flag = 0;
        } else if (correction > 14*0.3319) {
          if (correction>prev_correction) {
              if (succ_flag == 0 && out_flag == 0) {
                MouseMove(corr_dir*correction,0);
              }
              Sleep(500);
          } else {
            if (succ_flag == 0 && out_flag == 0) {
              MouseMove(corr_dir*correction,0);
            }
            Sleep(500);
          }
          
          
          prev_theta = -1.5708;
          cons_flag = 0;
        } else if (correction > prev_correction && adj_flag == 0) {
          printf("WRONG WAY \n");
          corr_dir = -1*corr_dir;
          if (succ_flag == 0 && out_flag == 0) {
            MouseMove(corr_dir*correction,0);
          }
          if (prev_correction != 999) {
            correction = prev_correction;
          }
          Sleep(500);
          prev_theta = -1.5708;
          cons_flag = 0;
        } else {
          if (corr_flag == 1) {
            adj_flag == 0;
          } else {
            if (adj_flag == 0) {
              if (out_flag == 0) {
                MouseMove(corr_dir*14*0.3319*0.9,0);
              }
              Sleep(1500);
              correction = corr_dir*14*0.3319*0.9;
              printf("adj1\n");
              adj_flag = 1;
            } else {
              corr_dir = -1*corr_dir;
              if (out_flag == 0) {
                MouseMove(corr_dir*2*14*0.3319*0.9,0);
              }
              Sleep(1500);
              correction = corr_dir*2*14*0.3319*0.9;
              printf("adj2\n");
              adj_flag = 1;
            }
          }
          prev_theta = 0.0;
        } 
        prev_correction = correction;
        corr_flag = 1;
        
        
        printf("loc: %f\n", loc);
      //}
    } else {
      /*if (press_time != 0) {
        Abutton(press_time);
      }*/
      if (press_time != 0) {
        //Abutton(press_time);
        Abutton_press();
        Sleep(press_time);
      }
      corr_flag = 0;
      cons_flag = 0;
      start_flag = 0;
      prev_theta = theta;
    }
  }
}

int main (int argc,char **argv)
{
  char chRet;
  char *raw_par;
  int par;
  int param[6];
  
  int ac_id;
  int rot_id;
  int mon_id;
  int loc_id;
  printf("QUIT anytime when focus is on terminal window using 'ctrl-c'\n");
  printf("Choose an ACTION to repeat: Type (E) to collect or (M) for MOUSE_BUTTON_1 or (L) for lockpick.\n");
  while (fgets(&chRet,100, stdin)) {
    if ((chRet == 'e') || (chRet == 'E')) {
      printf("\tYou answered E, program will tap E key to collect from containers\n\n");
      ac_id = 0;
      break;
    }
    else if ((chRet == 'm') || (chRet == 'M')) {
      printf("\tYou answered M, program will click the mouse to shoot or swing.\n\n");
      ac_id = 1;
      break;
    } else if ((chRet == 'l') || (chRet == 'L')) {
      printf("\tYou answered L, program will lockpick.\n\n");
      ac_id = 2;
      break;
    }
    else
      printf("\tYou entered %s\tInvalid entry, try again using (E/M)\n\n",&chRet);
  }
  if (ac_id == 2) {
    param[0] = 0;
    param[1] = 0;
    param[2] = 1;
    rot_id = 1;
    param[3] = 0;
    mon_id = 1;
    printf("Choose a method of lockpick: Type (M) for mod or (V) for vanilla.\n");
    while (fgets(&chRet,100, stdin)) {
      if ((chRet == 'v') || (chRet == 'V')) {
        printf("\tYou answered V for vanilla, program will attempt trial an error (inaccurate)\n\n");
        loc_id = 0;
        break;
      }
      else if ((chRet == 'm') || (chRet == 'M')) {
        printf("\tYou answered M for mod, program will not fail.\n\n");
        loc_id = 1;
        break;
      }
      else
        printf("\tYou entered %s\tInvalid entry, try again using (E/M)\n\n",&chRet);
    }
  } else {
    printf("Would you like to ROTATE HORIZONTALLY at DEFAULT -1px / 0.5s? Type (Y/N) or (C) for CUSTOM\n");
  //obtain user input on whether to add or multiply
    while (fgets(&chRet,100, stdin)) {
      if ((chRet == 'y') || (chRet == 'Y')) {
        printf("\tYou answered YES to ROTATE at -1px / 0.5s, ACTION will occur every tick.\n\n");
        param[0] = -1;
        param[1] = 0;
        param[2] = 1;
        param[3] = 500;
        rot_id = 0;
        break;
      }
      else if ((chRet == 'n') || (chRet == 'N')) {
        printf("\tYou answered NO to ROTATE, character will be stationary.  ACTION set to repeat every 500ms.\n\n");
        param[0] = 0;
        param[1] = 0;
        param[2] = 1;
        rot_id = 1;
        if (ac_id == 0) {
          param[3] = 4000;
        } else if (ac_id == 1) {
          param[3] = 4000;
        } else if (ac_id == 2) {
          param[3] = 0;
        }
        break;
      }
      else if ((chRet == 'c') || (chRet == 'C')) {
        raw_par = (char *)malloc(100 * sizeof (char));
        printf("\tYou answered CUSTOM, please specify number of HORIZONTAL pixels in one tick (DEFAULT is -1).\n\n");
        while (fgets(raw_par, 100, stdin)) {
          par = atoi(raw_par); //reduce input to integer value
          if (par > 99999999 || par < -99999999)
            printf("\tYou entered %s\tInvalid entry, try again using integer\n\n",raw_par);
          else {
            param[0] = par;
            break;
          }
        }
        raw_par = (char *)malloc(100 * sizeof (char));
        printf("\tPlease specify number of VERTICAL pixels in one tick (DEFAULT is 0).\n\n");
        while (fgets(raw_par, 100, stdin)) {
          par = atoi(raw_par); //reduce input to integer value
          if (par > 99999999 || par < -99999999)
            printf("\tYou entered %s\tInvalid entry, try again using integer\n\n",raw_par);
          else {
            param[1] = par;
            break;
          }
        }
        raw_par = (char *)malloc(100 * sizeof (char));
        printf("\tPlease specify number of MILLISECONDS in one tick (DEFAULT is 500).\n\n");
        while (fgets(raw_par, 100, stdin)) {
          par = atoi(raw_par); //reduce input to integer value
          if (par == 0 || par > 99999999 || par < 0)
            printf("\tYou entered %s\tInvalid entry, try again using integer\n\n",raw_par);
          else {
            param[3] = par;
            break;
          }
        }
        raw_par = (char *)malloc(100 * sizeof (char));
        printf("\tPlease specify number of ticks before ACTION is repeated (DEFAULT is 1).\n\n");
        while (fgets(raw_par, 100, stdin)) {
          par = atoi(raw_par); //reduce input to integer value
          if (par == 0 || par > 99999999 || par < 0)
            printf("\tYou entered %s\tInvalid entry, try again using integer\n\n",raw_par);
          else {
            param[2] = par;
            break;
          }
        }
        rot_id = 2;
        break;
      }
      else
        printf("\tYou entered %s\tInvalid entry, try again using (Y/N) or (C)\n\n",&chRet);
    }
    printf("Would you like HEALTH/RADS, FOOD, and WATER monitoring? (Y/N) or (C) for CUSTOM\n");
    while (fgets(&chRet,100, stdin)) {
      if ((chRet == 'y') || (chRet == 'Y')) {
        printf("\tYou answered YES, program will screenshot the HEALTH/RADS, FOOD, and WATER bar.  Please FAVORITE stimpacks to '=', WATER to '-', FOOD to '0', and RADAWAY to '9'.\n\n");
        mon_id = 0;
        break;
      }
      else if ((chRet == 'n') || (chRet == 'N')) {
        printf("\tYou answered NO, program will not monitor food, water, and health.  Leave unattended at your own risk.\n\n");
        mon_id = 1;
        break;
      }
      else if ((chRet == 'c') || (chRet == 'C')) {
        printf("\tYou answered CUSTOM, no screenshots will be taken.  Please FAVORITE WATER to '-' and FOOD to '0'.\n\tSpecify the number of ticks before consuming FOOD (hint: ~1000)\n\n");
        while (fgets(raw_par, 100, stdin)) {
          par = atoi(raw_par); //reduce input to integer value
          if (par == 0 || par > 99999999 || par < 0)
            printf("\tYou entered %s\tInvalid entry, try again using integer\n\n",raw_par);
          else {
            param[4] = par;
            break;
          }
        }
        printf("\tSpecify the number of ticks before consuming WATER (hint: ~1000).\n\n");
        while (fgets(raw_par, 100, stdin)) {
          par = atoi(raw_par); //reduce input to integer value
          if (par == 0 || par > 99999999 || par < 0)
            printf("\tYou entered %s\tInvalid entry, try again using integer\n\n",raw_par);
          else {
            param[5] = par;
            break;
          }
        }
        mon_id = 2;
        break;
      }
      else
        printf("\tYou entered %s\tInvalid entry, try again using (Y/N) or (C)\n\n",&chRet);
    }
  }
  if (RegisterHotKey(NULL, 1, MOD_ALT, 0x42))    //0x42 is 'b'
  {
      wprintf(L"Hotkey 'alt+b' registered to START.\n");
  }
  if (RegisterHotKey(NULL, 2, MOD_ALT, 0x43))    //0x43 is 'c'
  {
      wprintf(L"Hotkey 'alt+c' registered to STOP.\n");
  }
  MSG msg;
  MSG kmsg;
  int flag;
  std::clock_t start;
  start = std::clock();
  while (GetMessage(&msg, NULL, 0, 0))
  {   
    flag = 0;
    while (flag == 0) {
      switch (msg.message)
      {
        case WM_HOTKEY:
          if (msg.wParam == 1) { 
            //ScreenCapture((1920-1080)/2, 0, 1080, 1080, (char *)buf);
            
            int wat_tim = 0;
            int foo_tim = 0;
      
            if (ac_id == 0) {
              Ebutton();
            } else if (ac_id == 1) {
              LeftClick();
            } else if (ac_id == 2) {
              if (loc_id == 0) {
                lockpick();
              } else if (loc_id == 1) {
                lockpick_m();
              }
              img_cnt++;
            }
            if (succ_flag == 0 && out_flag == 0) {
              MouseMove(param[0],param[1]);
            }
            if (param[3]>0) {
              Sleep(param[3]);
            }
            
            if (mon_id == 2) {
              wat_tim++;
              foo_tim++;
              if (wat_tim==param[5]) {
                minusbutton();
                Sleep(500);
                wat_tim = 0;
              }
              if (foo_tim==param[4]) {
                zerobutton();
                Sleep(500);
                foo_tim = 0;
              }
            }
          } else if (msg.wParam == 2) {
            flag = 1;
          }
      }
      PeekMessage(&msg,NULL,0,0,0);
    }
  }
  return 0;
}
