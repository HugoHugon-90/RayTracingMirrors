// PARA CORRERES O CODIGO: 
//     g++ SofiaRT.cpp -o SofiaRT (para compilar)
//    ./SofiaRT (para correr)

//DEPOIS TENS QUE FAZER O PLOT DE TODOS OS 6 TXT!!!!


#include <string.h>
#include <Math.h>
#include <fstream>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;




//criar ficheiros de escrita
ofstream RTposC();
ofstream RTposD();
ofstream RTposU();

ofstream ED();
ofstream EC();
ofstream EU();


int main(int argc,char *argv[])
{
 //abrir ficheiros de escrita
 RTposC.open("RTposC.txt");
 RTposU.open("RTposU.txt");
 RTposD.open("RTposD.txt");

 ED.open("ED.txt");
 EC.open("EC.txt");
 EU.open("EU.txt");

//nota: a origem do referencial cartesiano usado neste codigo esta no ponto de injecao do feixe,
//com o eixo dos xx orientado desde a origem ate ao centro geometrico do espelho central.

 double x0, y0; //posicao de injeccao do raio
 double theta; //abertura do raio
 
 double x0c, y0c, x0u,y0u, x0d, y0d; //posicao do centro geometrico do espelho central, do espelho de cima (u) e do espelho de baixo (d)

 double dc, dd, du; //metade da largura dos espelhos (c-central, d-baixo,u-cima)
 
 double ddy, ddx, duy, dux, dcy, dcx; //projeccao de metade de cada espelho no eixo dos xx e dos yy (c-central, d-baixo,u-cima)

 double wc, wd, wu; //angulo (contra-relogio) que cada espelho faz com o eixo dos xx

 double c, n, v; //velocidade da luz, indice de refraccao, velocidade de grupo, respectivament


 //inicializaçao de variaveis
 c=1;
 n=1;
 v=c/n;

 theta=40*M_PI/180.;

x0=0;
y0=0;

x0c=100.;
y0c=0;
dc=5; //metade do espelho
wc=45.*M_PI/180.;
dcx=dc*cos(wc);
dcy=dc*sin(wc);



x0d=80;
y0d=-70;
dd=10;
wd=23*M_PI/180.;
ddx=dd*cos(wd);
ddy=dd*sin(wd);


x0u=120;
y0u=100;
du=10;
wu=70.*M_PI/180.;
dux=du*cos(wu);
duy=du*sin(wu);


//declives das rectas dos espelhos
double mc=dcy/dcx;
double mu=duy/dux;
double md=ddy/ddx;;

double xe;
double ye;

//Cada ciclo desenha cada espelho e escreve-o, para posterior plot, em EC.txt ED.txt EU.txt

for(xe=x0c-dcx; abs(xe-(x0c+dcx))>0.0001;xe+=2*dcx/100.) //espelho central
{
ye=y0c-dcy+ mc*(xe-x0c+dcx);
EC<<xe<<"\t"<<ye<<endl;
}


for(xe=x0u-dux; abs(xe-(x0u+dux))>0.0001;xe+=2*dux/100.)  //espelho de cima
{
ye=y0u-duy+mu*(xe-x0u+dux);
EU<<xe<<"\t"<<ye<<endl;
}

for(xe=x0d-ddx; abs(xe-(x0d+ddx))>0.0001;xe+=2*ddx/100.)  //espelho de baixo
{
ye=y0d-ddy+ md*(xe-x0d+ddx);
ED<<xe<<"\t"<<ye<<endl;
}

//quantidades relativas à discretizacao do tempo. qual o tempo maxim e qual o time step (ou, equivalente, quantas subdivisoes usar)
double tMAX=400;
double accuracy=100;
double tDIV=accuracy*tMAX;
double tSTEP = tMAX/tDIV;

double t=0;

//tolerancia usada em alguns sitios
double tol=0.00001;
//tolerancia usada em alguns sitios
double tol2=0.1;


//variavieis usadas para escrever a posicao de cada raio
double xc,yc,xd,yd,xu,yu;

xc=0;
yc=0;
xd=0;
yd=0;
xu=0;
yu=0;

 //equacoes de movimento, x=x0+ct etc., ate o raio central bater no espelho central abs(c*t-(x0c-x0))<tol
 //NOTA: assume-se neste codigo que os espelhos estao tao proximos que o raio central bate sempre
 //primeiro no espelho que o raio inferior
  for (t = 0; abs(c*t-(x0c-x0))>tol ; t+=tSTEP)
  {
    xc += v*tSTEP;
    yc += 0;

    xu += v*tSTEP*cos(theta);
    yu += v*tSTEP*sin(theta);

    xd += v*tSTEP*cos(theta);
    yd += -v*tSTEP*sin(theta);

 //escreve nos ficheiros
    RTposC<<xc<<"\t"<<yc<<"\t"<<endl;
    RTposU<<xu<<"\t"<<yu<<"\t"<<endl;
    RTposD<<xd<<"\t"<<yd<<"\t"<<endl;

   }

  cout<<"espelho central bateu em t="<<t<<endl;
 //A partir de aqui o raio central já embateu no espelho central, mas o raio inferior ainda nao
 //A equacao de movimento do raio superior fica igual
 //A equacao do movimento do raio central muda (reflectido)
 //A equação do raio inferior fica igual, mas tem que se checkar quando bate no espelho.

  while(1)  
  {


    //checkar se o raio de baixo embate em algum momentono espelho central. Se sim, erro.
    //em principio isto impede que o raio superior tambem bata devido a inclinacao do espelho
      if(xd>(x0c-dcx) && abs(y0c-yd)<dcy) 
     {
     cout<<endl<<"Erro. O raio inferior choca com o espelho central."<<endl;
       RTposC.close();
       RTposU.close();
       RTposD.close();
     return 0;
      }
    
    xc += v*cos(2*wc)*tSTEP; //nova equacao para o raio central
    yc += v*sin(2*wc)*tSTEP;

    xu += v*tSTEP*cos(theta);
    yu += v*tSTEP*sin(theta);

    xd += +v*tSTEP*cos(theta);
    yd += -v*tSTEP*sin(theta);
        
    //se o raio inferior intersectar o segmento de recta formada pelo espelho,
    //ou seja: yd=ye=(y0d-ddy)+md*(x-(x0d+ddx)) e |yd-y0d|<2xddy, entao ha uma reflexao
    //e sai-se do ciclo while

    if(abs(yd-(y0d-ddy)-md*(xd-x0d+ddx))<tol2  &&  abs(yd-y0d)<2*ddy)
    {
        cout<<endl<<"o raio inferior atingiu o espelho inferior na posicao xu = "<<xd<<", yu = "<<yd<<", no tempo t = "<<t<<endl;
        break;
    }

     //Se nao e porque falhou o espelho
    if (abs(t-tMAX)<tol) {cout<<"Erro. O raio inferior falhou o espelho."<<endl; return 0;}

    RTposC<<xc<<"\t"<<yc<<"\t"<<endl;
    RTposU<<xu<<"\t"<<yu<<"\t"<<endl;
    RTposD<<xd<<"\t"<<yd<<"\t"<<endl;
    t+=tSTEP;

   }


 //A partir de aqui o raio central e o inferior já embateram nos respectivos espelhos, 
 //mas o raio superior ainda nao.
 //A equacao do movimento do raio central  e inferior mudam (reflectidos).
 //A equacao de movimento do raio superior fica igual, mas tem que se checkar quando bate no espelho.
 
while(1)  
  {
    xc += v*cos(2*wc)*tSTEP; // equacao para o raio central reflectido 
    yc += v*sin(2*wc)*tSTEP;

    xu += v*tSTEP*cos(theta);
    yu += v*tSTEP*sin(theta);

    xd += v*tSTEP*cos(2*wd+theta); //nova equacao para o raio inferior reflectido //DUVIDA!!!!!!!!!!
    yd += v*tSTEP*sin(2*wd+theta);//DUVIDA!!!!!!!!!!


       //checkar se o raio de baixo embate em algum momento no espelho central. Se sim, erro.
       if(abs(yd-(y0c-dcy)-mc*(xd-x0c+dcx))<tol2 &&  abs(yd-y0c)<2*dcy)
    {
        cout<<endl<<" Erro. O raio inferior atingiu o espelho central apos ser reflectido";
         RTposC.close();
         RTposU.close();
         RTposD.close();
        return 0;
    }
    
    //checkar se o raio de cima embate em algum momento no espelho de cima. Se sim, sai do loop.
    if(abs(yu-(y0u-duy)-mu*(xu-x0u+dux))<tol2  &&  abs(yu-y0u)<2*duy)
    {
        cout<<endl<<"o raio superior atingiu o espelho superior na posicao xu = "<<xu<<", yu = "<<yu<<", no tempo t = "<<t<<endl;
        break;
    }

         //Se nao e porque falhou o espelho
         if (abs(t-tMAX)<tol) {cout<<"Erro. O raio superior falhou o espelho."<< "t="<<t<<endl; return 0;}

     t+=tSTEP;

    
    RTposC<<xc<<"\t"<<yc<<"\t"<<endl;
    RTposU<<xu<<"\t"<<yu<<"\t"<<endl;
    RTposD<<xd<<"\t"<<yd<<"\t"<<endl;

   }


//aqui ja todos foram reflectidos!
   while(abs(t-tMAX)>tol)  
  {
    xc += v*cos(2*wc)*tSTEP; // equacao para o raio central reflectido
    yc += v*sin(2*wc)*tSTEP;


    xu += v*tSTEP*cos(2*wu-theta); //nova equacao para o raio superior reflectido //DUVIDA!!!!!!!!!!
    yu += v*tSTEP*sin(2*wu-theta);//DUVIDA!!!!!!!!!!

    xd += v*tSTEP*cos(2*wd+theta); //equacao para o raio inferior reflectido //DUVIDA!!!!!!!!!!
    yd += v*tSTEP*sin(2*wd+theta);//DUVIDA!!!!!!!!!!

     
     t+=tSTEP;

    
     RTposC<<xc<<"\t"<<yc<<"\t"<<endl;
    RTposU<<xu<<"\t"<<yu<<"\t"<<endl;
    RTposD<<xd<<"\t"<<yd<<"\t"<<endl;

   }

 RTposC.flush();
 RTposU.flush();
 RTposD.flush();


 ED.flush();
 EC.flush();
 EU.flush();

 RTposC.close();
 RTposU.close();
 RTposD.close();


 ED.close();
 EC.close();
 EU.close();
}