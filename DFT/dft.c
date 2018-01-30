#include <math.h>
#include <stdio.h>

#define N 200
#define PI 3.14159265358979323846
                            
float xdata[N];   
float ydata[N];   
float adata[N];   
float bdata[N];   
float sdata[N/2];   
 

void dft(float x[],float y[],float a[],float b[],int n,int sign); 
void getmod(float x[],float y[],float s[], int n);
void main ()
 {
  int i;
  
  
  for (i=0;i<N;i++)
   {
    xdata[i] =2*sin(2*PI*4*i/N);
    ydata[i] =0;
   }
   dft(xdata,ydata,adata,bdata,N,1);
   getmod(adata,bdata,sdata,N);
   
  for (i=0;i<N;i++)
   {
    xdata[i] =2*sin(2*PI*12*i/N);
    ydata[i] =0;
   }
   dft(xdata,ydata,adata,bdata,N,1);
   getmod(adata,bdata,sdata,N);

  for (i=0;i<N;i++)
   {
    xdata[i] =(sin(2*PI*4*i/N) + 2*sin(2*PI*12*i/N));
    ydata[i] =0;
   }
   dft(xdata,ydata,adata,bdata,N,1);
   getmod(adata,bdata,sdata,N);

   
 }
 
 

/************************************************************************ 
* 离散傅立叶变换与反变换 
* 输入: x--要变换的数据的实部 
*       y--要变换的数据的虚部 
*       a--变换结果的实部 
*       b--变换结果的虚部 
*       n--数据长度 
************************************************************************/ 
void dft(float x[],float y[],float a[],float b[],int n,int sign) 
{ 
  int i,k; 
  double c,d,q,w,s; 

  q=6.28318530718/n; 
  for(k=0;k<n;k++) 
  { 
   w=k*q; 
   a[k]=b[k]=0.0;
   for(i=0;i<n;i++) 
     { 
       d=i*w; 
       c=cos(d); 
       s=sin(d)*sign;  
       a[k]+=c*x[i]+s*y[i]; 
       b[k]+=c*y[i]-s*x[i]; 
     } 
  } 
  if(sign==1) 
  { 
   c=1.0/(n/2); 
   for(k=0;k<n;k++) 
     { 
       a[k]=c*a[k]; 
       b[k]=c*b[k];
     } 
  } 
} 


void getmod(float x[], float y[],float s[], int n)
{
	int i,k;
	for (i=0;i<n;i++)
	{
		x[i]=(float)(pow(x[i],2)+pow(y[i],2));
		
	}
	for(k=0;k<n/2;k++)
		{
		s[k]=(float)(sqrt(x[k]));
		}
} 

/*end*/
