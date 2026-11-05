#include <SDL2/SDL.h>
#include <psp2/kernel/processmgr.h>
#include <math.h>
#include <stdlib.h>

#define W 800
#define H 400
typedef struct { float x,y,z,vz,atk,combo; int hp,face,hit; } Player;
typedef struct { float x,y; int hp; } Target;

static void box(SDL_Renderer*r,int x,int y,int w,int h,int cr,int cg,int cb){
 SDL_Rect q={x,y,w,h}; SDL_SetRenderDrawColor(r,cr,cg,cb,255); SDL_RenderFillRect(r,&q);
}

/* Kai's proportions and palette are translated from index.html's dch() art. */
static void kai(SDL_Renderer*r,const Player*p,float t){
 int x=(int)p->x,y=(int)(p->y-p->z);
 SDL_Rect sh={x-16,(int)p->y-4,32,8};
 SDL_SetRenderDrawColor(r,0,0,0,90); SDL_RenderFillRect(r,&sh);
 box(r,x-4,y-14,8,14,34,42,68);
 box(r,x+3,y-14,8,14,34,42,68);
 box(r,x-10,y-34,20,21,43,182,168);
 box(r,x-8,y-52,16,16,241,201,160);
 box(r,x-9,y-55,18,7,232,69,60);
 box(r,x+(p->face>0?3:-6),y-46,3,3,17,17,17);
 if(p->hit){
  box(r,x+p->face*10-(p->face<0?18:0),y-33,28,7,241,201,160);
  box(r,x+p->face*31-(p->face<0?8:0),y-32,9,8,255,224,96);
 }else{
  box(r,x-13,y-32,5,14,241,201,160);
  box(r,x+8,y-32,5,14,241,201,160);
 }
 (void)t;
}
static void target(SDL_Renderer*r,const Target*e){
 int x=(int)e->x,y=(int)e->y;
 box(r,x-10,y-34,20,21,122,79,201); box(r,x-8,y-52,16,16,241,201,160);
 box(r,x-9,y-55,18,7,34,34,34); box(r,x-9,y-14,8,14,34,42,68);
 box(r,x+1,y-14,8,14,34,42,68);
}
static void scene(SDL_Renderer*r,const Player*p,const Target*e,float t){
 SDL_SetRenderDrawColor(r,13,10,30,255); SDL_RenderClear(r);
 box(r,0,0,W,250,25,19,49);
 for(int i=0;i<10;i++){int x=i*86,bh=90+(i%3)*25;
  box(r,x,250-bh,62,bh,43,31,76); box(r,x+12,250-bh+16,7,8,0,210,220);
  box(r,x+34,250-bh+38,7,8,255,80,170);}
 box(r,0,250,W,150,27,24,42); box(r,0,395,W,3,0,210,240);
 if(e->y<p->y)target(r,e); kai(r,p,t); if(e->y>=p->y)target(r,e);
 box(r,20,16,164,10,65,20,30); box(r,20,16,(p->hp>0?p->hp:0)*164/100,10,70,220,110);
 box(r,616,16,164,10,65,20,30); box(r,616,16,(e->hp>0?e->hp:0)*164/30,10,230,75,115);
 SDL_RenderPresent(r);
}
int main(int argc,char**argv){
 (void)argc;(void)argv;
 if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMECONTROLLER)<0){sceKernelExitProcess(1);return 1;}
 SDL_Window*w=SDL_CreateWindow("Neon Brawl",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,W,H,SDL_WINDOW_SHOWN);
 if(!w){SDL_Quit();sceKernelExitProcess(1);return 1;}
 SDL_Renderer*r=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED);
 if(!r){SDL_DestroyWindow(w);SDL_Quit();sceKernelExitProcess(1);return 1;}
 SDL_GameController*pad=NULL;if(SDL_NumJoysticks()>0&&SDL_IsGameController(0))pad=SDL_GameControllerOpen(0);
 Player p={100,300,0,0,0,0,100,1,0}; Target e={650,300,30};
 int run=1;float time=0;Uint32 prev=SDL_GetTicks();SDL_Event ev;
 while(run){
  Uint32 now=SDL_GetTicks();float dt=(now-prev)/1000.0f;prev=now;if(dt>.033f)dt=.033f;time+=dt;
  while(SDL_PollEvent(&ev)){if(ev.type==SDL_QUIT)run=0;
   if(ev.type==SDL_CONTROLLERBUTTONDOWN){
    if(ev.cbutton.button==SDL_CONTROLLER_BUTTON_A&&p.z==0)p.vz=330;
    if(ev.cbutton.button==SDL_CONTROLLER_BUTTON_X&&!p.hit){
     p.hit=1;p.atk=.22f;p.combo=p.combo>.0f?(p.combo>=3?1:p.combo+1):1;
     if(fabsf(e.x-p.x)<55&&fabsf(e.y-p.y)<24){e.hp-=p.combo==3?14:8;if(e.hp<=0){e.hp=30;e.x=700;}}
    }
   }
  }
  const Uint8*k=SDL_GetKeyboardState(NULL);float mx=0,my=0;
  if(k[SDL_SCANCODE_LEFT]||k[SDL_SCANCODE_A])mx--;
  if(k[SDL_SCANCODE_RIGHT]||k[SDL_SCANCODE_D])mx++;
  if(k[SDL_SCANCODE_UP]||k[SDL_SCANCODE_W])my--;
  if(k[SDL_SCANCODE_DOWN]||k[SDL_SCANCODE_S])my++;
  if(k[SDL_SCANCODE_X]&&!p.hit){p.hit=1;p.atk=.22f;p.combo=p.combo>.0f?(p.combo>=3?1:p.combo+1):1;
   if(fabsf(e.x-p.x)<55&&fabsf(e.y-p.y)<24){e.hp-=p.combo==3?14:8;if(e.hp<=0){e.hp=30;e.x=700;}}}
  if(k[SDL_SCANCODE_Z]&&p.z==0)p.vz=330;
  if(pad){Sint16 ax=SDL_GameControllerGetAxis(pad,SDL_CONTROLLER_AXIS_LEFTX),ay=SDL_GameControllerGetAxis(pad,SDL_CONTROLLER_AXIS_LEFTY);
   if(abs(ax)>7000)mx=ax/32767.0f;if(abs(ay)>7000)my=ay/32767.0f;}
  if(!p.hit){p.x+=mx*150*dt;p.y+=my*90*dt;if(mx)p.face=mx>0?1:-1;}
  else{p.atk-=dt;if(p.atk<=0)p.hit=0;}
  p.combo-=dt;p.z+=p.vz*dt;if(p.z>0||p.vz>0)p.vz-=900*dt;if(p.z<=0){p.z=0;p.vz=0;}
  if(p.x<15)p.x=15;if(p.x>785)p.x=785;if(p.y<250)p.y=250;if(p.y>385)p.y=385;
  scene(r,&p,&e,time);SDL_Delay(1);
 }
 if(pad)SDL_GameControllerClose(pad);SDL_DestroyRenderer(r);SDL_DestroyWindow(w);SDL_Quit();sceKernelExitProcess(0);return 0;
}