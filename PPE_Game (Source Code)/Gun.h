#ifndef GUN_H_INCLUDED
#define GUN_H_INCLUDED

#include <cstring>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "graphic.h"
#include "util/gameConst.h"


typedef struct Gun
{

    Image bullet[4];
    Image impact[4];
    int balle=0;
    Image smoke=CreateImage("data/smoke.png");
    int sens[4];
    int collision[4];
    int comptB=0;
    int comptI[4];
    //int chest=1;
    //Image openChest=CreateImage("data/openChest.png");
    int timerB=0;
    int timer=0;


    void Init()
    {
        for (int i=0; i<4; i++)
        {
            bullet[i]=CreateImage("data/fireball.png");
            collision[i]=0;
            comptI[i]=0;
            impact[i]=CreateImage("data/impact.png");
        }
    }



    void Bullet(int movementDirection,Vec2 Right,Vec2 Left)
    {
        if (balle<1)
        {

            balle=balle+1;
            ImgSetScale(bullet[balle],2.0);

            if(movementDirection==0)
            {
                ImgSetPosition(bullet[balle],Right);
                sens[balle]=1;
            }
            else if(movementDirection==1)
            {
                ImgSetPosition(bullet[balle],Left.x-25,Left.y);
                sens[balle]=0;
            }
            else if(movementDirection==2)
            {
                ImgSetPosition(bullet[balle],Left.x, Left.y+25);
                sens[balle]=2;
            }
            else if(movementDirection==3)
            {
                ImgSetPosition(bullet[balle],Left.x, Left.y-25);
                sens[balle]=3;
            }
        }
        else
        {
            comptB=40;
        }
    }

    void actuBullet(Vec2 Right,Vec2 Left,int Mat[40][23],int direction)
    {
        if (balle>=0 && balle<4)
        {
            Vec2 posBullet;
            for (int i=0; i<=balle; i++)
            {
                if(collision[i]!=1)
                {

                    if(sens[i]==1)
                    {
                        ImgSetPosition(bullet[i],ImgGetPosition(bullet[i]).x+25,ImgGetPosition(bullet[i]).y);
                        posBullet=CreateVec2(ImgGetPosition(bullet[i]).x+30,ImgGetPosition(bullet[i]).y+16);
                    }
                    else if(sens[i]==0)
                    {
                        ImgSetScale(bullet[i],-2,2);
                        ImgSetPosition(bullet[i],ImgGetPosition(bullet[i]).x-25,ImgGetPosition(bullet[i]).y);
                        posBullet=CreateVec2(ImgGetPosition(bullet[i]).x-30,ImgGetPosition(bullet[i]).y+16);
                    }
                    else if(sens[i]==2)
                    {
                        ImgSetRotation(bullet[i],90.0);
                        ImgSetPosition(bullet[i],ImgGetPosition(bullet[i]).x,ImgGetPosition(bullet[i]).y+25);
                        posBullet=CreateVec2(ImgGetPosition(bullet[i]).x,ImgGetPosition(bullet[i]).y+50);
                    }
                    else
                    {
                        ImgSetRotation(bullet[i],-90.0);
                        ImgSetPosition(bullet[i],ImgGetPosition(bullet[i]).x,ImgGetPosition(bullet[i]).y-25);
                        posBullet=CreateVec2(ImgGetPosition(bullet[i]).x,ImgGetPosition(bullet[i]).y-50);
                    }




                    if (Mat[(int)posBullet.x/32][(int)posBullet.y/32]==1)
                    {
                        collision[i]=1;
                        comptI[i]=15;
                        if(sens[i]==1)
                        {
                            ImgSetPosition(impact[i],ImgGetPosition(bullet[i]).x-25,ImgGetPosition(bullet[i]).y);
                        }
                        else if(sens[i]==2)
                        {
                            ImgSetPosition(impact[i],ImgGetPosition(bullet[i]).x,ImgGetPosition(bullet[i]).y+16);
                        }
                        else if(sens[i]==3)
                        {
                            ImgSetPosition(impact[i],ImgGetPosition(bullet[i]).x,ImgGetPosition(bullet[i]).y-16);
                        }
                        else
                        {
                            ImgSetPosition(impact[i],ImgGetPosition(bullet[i]));
                        }
                        ImgSetPosition(bullet[i],-200,-200);
                    }
                    DrawImage(bullet[i]);
                }
                else
                {
                    posBullet=CreateVec2(-200.0,-200.0);
                    ImgSetPosition(bullet[i],-200,-200);
                    ImgSetRotation(bullet[i],0.0);
                }

                if(comptI[i]>0)
                {
                    DrawImage(impact[i]);
                    comptI[i] =comptI[i]-1;
                }


                if(comptB>0)
                {
                    if(direction==RIGHT)
                    {
                        ImgSetScale(smoke,0.7,0.7);
                        ImgSetPosition(smoke,Right-20);
                    }
                    else if(direction==LEFT)
                    {
                        ImgSetScale(smoke,-0.7,0.7);
                        ImgSetPosition(smoke,Left-20);
                    }
                    else
                    {
                        ImgSetScale(smoke,-0.7,0.7);
                        ImgSetPosition(smoke,Left.x+12,Left.y+30);
                    }

                    DrawImage(smoke);
                    comptB =comptB-1;
                }
            }
        }
        /*if (chest==0)
        {
            timer+=1;
            if (timer-timerB>600)
            {
                chest=1;
                timerB=timer;
            }
        }*/
    }


    void reload()
    {
        for (int i=0; i<4; i++)
        {
            ImgSetPosition(bullet[i], -100.0f, -100.0f);
        }
        balle=0;
        //chest=0;
        for (int i=0; i<4; i++)
        {
            collision[i]=0;
        }
    }



    /*int OpenChest()
    {
        if(chest==0)
        {
            DrawImage(openChest);
            return 1;
        }
        else
        {
            return 0;
        }
    }*/

    void dropAmmo()
    {
        balle=3;
    }

    void Free()
    {
         for (int i=0; i<4; i++)
        {
            FreeImage(bullet[i]);
            FreeImage(impact[i]);
        }
        FreeImage(smoke);
        //FreeImage(openChest);
    }


} Gun;

#endif // GUN_H_INCLUDED
