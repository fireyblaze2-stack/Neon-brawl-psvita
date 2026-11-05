#include <SDL2/SDL.h>
#include <psp2/kernel/processmgr.h>
#include <math.h>
#include <stdlib.h>

#define W 800
#define H 400
typedef struct { float x,y,z,vz,atk,combo,hurt; int hp,face,hit; } Player;
typedef struct { float x,y,cd,wind,speed; int hp,maxhp,face,hit,damage,type; } Enemy;

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
static void target(SDL_Renderer*r,const Enemy*e){
 int x=(int)e->x,y=(int)e->y;
 int shirt[4][3]={{122,79,201},{224,122,43},{58,122,208},{176,48,48}};
 int hair[4][3]={{34,34,34},{90,163,58},{221,221,221},{17,17,17}};
 int t=e->type%4;
 box(r,x-10,y-34,20,21,shirt[t][0],shirt[t][1],shirt[t][2]);
 box(r,x-8,y-52,16,16,241,201,160);
 box(r,x-9,y-55,18,7,hair[t][0],hair[t][1],hair[t][2]);
 box(r,x-9,y-14,8,14,34,42,68); box(r,x+1,y-14,8,14,34,42,68);
 if(e->hit)box(r,x+e->face*9-(e->face<0?18:0),y-33,26,7,241,201,160);
}
static void scene(SDL_Renderer*r,const Player*p,const Enemy*e,int count,float t){
 SDL_SetRenderDrawColor(r,13,10,30,255); SDL_RenderClear(r);
 box(r,0,0,W,250,25,19,49);
 for(int i=0;i<10;i++){int x=i*86,bh=90+(i%3)*25;
  box(r,x,250-bh,62,bh,43,31,76); box(r,x+12,250-bh+16,7,8,0,210,220);
  box(r,x+34,250-bh+38,7,8,255,80,170);}
 box(r,0,250,W,150,27,24,42); box(r,0,395,W,3,0,210,240);
 for(int i=0;i<count;i++)if(e[i].hp>0&&e[i].y<p->y)target(r,&e[i]);
 kai(r,p,t);
 for(int i=0;i<count;i++)if(e[i].hp>0&&e[i].y>=p->y)target(r,&e[i]);
 box(r,20,16,164,10,65,20,30); box(r,20,16,(p->hp>0?p->hp:0)*164/100,10,70,220,110);
 box(r,616,16,164,10,65,20,30); int total=0,max_total=0; for(int i=0;i<count;i++){total+=e[i].hp>0?e[i].hp:0;max_total+=e[i].maxhp;}\n box(r,616,16,max_total?total*164/max_total:0,10,230,75,115);
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
 Player p={100,300,0,0,0,0,0,100,1,0};
 /* First original wave: three thugs. Following waves introduce runners,
    gunmen, and the brute archetype from index.html TY/WAVES. */
 Enemy enemies[4]={{650,285,0,0,55,30,30,-1,0,8,0},
                   {730,335,0,0,90,25,25,-1,0,6,1},
                   {570,365,0,0,50,25,25,-1,0,7,2},
                   {0,0,0,0,60,160,160,-1,0,16,3}};
 int enemy_count=3;
 int run=1;float time=0;Uint32 prev=SDL_GetTicks();SDL_Event ev;
 while(run){
  Uint32 now=SDL_GetTicks();float dt=(now-prev)/1000.0f;prev=now;if(dt>.033f)dt=.033f;time+=dt;
  while(SDL_PollEvent(&ev)){if(ev.type==SDL_QUIT)run=0;
   if(ev.type==SDL_CONTROLLERBUTTONDOWN){
    if(ev.cbutton.button==SDL_CONTROLLER_BUTTON_A&&p.z==0)p.vz=330;
    if(ev.cbutton.button==SDL_CONTROLLER_BUTTON_X&&!p.hit){
     p.hit=1;p.atk=.22f;p.combo=p.combo>.0f?(p.combo>=3?1:p.combo+1):1;
     for(int j=0;j<enemy_count;j++)if(enemies[j].hp>0&&fabsf(enemies[j].x-p.x)<55&&fabsf(enemies[j].y-p.y)<24)enemies[j].hp-=p.combo==3?14:8;
    }
   }
  }
  const Uint8*k=SDL_GetKeyboardState(NULL);float mx=0,my=0;
  if(k[SDL_SCANCODE_LEFT]||k[SDL_SCANCODE_A])mx--;
  if(k[SDL_SCANCODE_RIGHT]||k[SDL_SCANCODE_D])mx++;
  if(k[SDL_SCANCODE_UP]||k[SDL_SCANCODE_W])my--;
  if(k[SDL_SCANCODE_DOWN]||k[SDL_SCANCODE_S])my++;
  if(k[SDL_SCANCODE_X]&&!p.hit){p.hit=1;p.atk=.22f;p.combo=p.combo>.0f?(p.combo>=3?1:p.combo+1):1;
   for(int j=0;j<enemy_count;j++)if(enemies[j].hp>0&&fabsf(enemies[j].x-p.x)<55&&fabsf(enemies[j].y-p.y)<24)enemies[j].hp-=p.combo==3?14:8;}
  if(k[SDL_SCANCODE_Z]&&p.z==0)p.vz=330;
  if(pad){Sint16 ax=SDL_GameControllerGetAxis(pad,SDL_CONTROLLER_AXIS_LEFTX),ay=SDL_GameControllerGetAxis(pad,SDL_CONTROLLER_AXIS_LEFTY);
   if(abs(ax)>7000)mx=ax/32767.0f;if(abs(ay)>7000)my=ay/32767.0f;}
  if(!p.hit){p.x+=mx*150*dt;p.y+=my*90*dt;if(mx)p.face=mx>0?1:-1;}
  else{p.atk-=dt;if(p.atk<=0)p.hit=0;}
  p.combo-=dt;
  /* Each enemy uses the speed, health, and damage values from the
     original game's TY definitions; ranged archetypes are next. */
  for(int j=0;j<enemy_count;j++){
   Enemy *a=&enemies[j]; if(a->hp<=0)continue;
   a->cd-=dt;
   if(a->wind>0){a->wind-=dt;a->hit=1;
    if(a->wind<=0){a->hit=0;a->cd=1.2f;
     if(fabsf(a->x-p.x)<52&&fabsf(a->y-p.y)<24&&p.hurt<=0){
      p.hp-=a->damage;p.hurt=.7f;
     }
    }
   }else{
    float dx=p.x-a->x,dy=p.y-a->y;a->face=dx>=0?1:-1;
    if(fabsf(dx)>40)a->x+=(dx>0?1:-1)*a->speed*dt;
    if(fabsf(dy)>9)a->y+=(dy>0?1:-1)*a->speed*.6f*dt;
    if(fabsf(dx)<52&&fabsf(dy)<18&&a->cd<=0)a->wind=.42f;
   }
  }
  if(p.hurt>0)p.hurt-=dt;
  p.z+=p.vz*dt;if(p.z>0||p.vz>0)p.vz-=900*dt;if(p.z<=0){p.z=0;p.vz=0;}
  if(p.x<15)p.x=15;if(p.x>785)p.x=785;if(p.y<250)p.y=250;if(p.y>385)p.y=385;
  scene(r,&p,enemies,enemy_count,time);SDL_Delay(1);
 }
 if(pad)SDL_GameControllerClose(pad);SDL_DestroyRenderer(r);SDL_DestroyWindow(w);SDL_Quit();sceKernelExitProcess(0);return 0;
}