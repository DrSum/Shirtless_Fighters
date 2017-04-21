#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <vector>
#include <string>

#include "MySQL.h"
#include "graphic.h"
#include "map.h"
#include "util/gameConst.h"
#include "Perso.h"
#include "util/timeTools.h"
#include "util/spawn.h"

void updatePseudo(std::vector<Perso> players)
{
    std::string query;
    char txt[512];

    for(unsigned int i = 0; i < players.size(); i++)
    {
        query = ("INSERT INTO perso(PSEUDO)");

        sprintf(txt,"VALUES('%s')", players[i].name.c_str());

        query += txt;

        MySQLRequest("127.0.0.1", "root", "", "shirtless_fighters", (char*)query.c_str());
    }

    MySQLRequest("127.0.0.1", "root", "", "shirtless_fighters", "INSERT INTO partie(DATE) VALUES(NOW())");
}

int checkForWinner(std::vector<Perso> players)
{
     for(unsigned int i = 0; i < players.size(); i++)
    {
        if(players[i].nbKill >= 10)
            return i+2;
    }
    return 0;
}

void endMatch(std::vector<Perso> players)
{

    std::string query;
    char txt[512];

    for(unsigned int i = 0; i < players.size(); i++)
    {
        query = ("INSERT INTO partie_jouee(PSEUDO, ID_PARTIE, SCORE, KILLS, DEATHS)");

        sprintf(txt,"VALUES('%s', (SELECT COUNT(ID_PARTIE) + 1 FROM PARTIE), %d, %d, %d)", players[i].name.c_str(), players[i].score, players[i].nbKill, players[i].nbDeathGame);

        query += txt;

        MySQLRequest("127.0.0.1", "root", "", "shirtless_fighters", (char*)query.c_str());
    }

}


void SetMap(int numMap, std::vector<Perso> perso)
{

    updatePseudo(perso);


    int x=SCREENWIDTH / 2;
    int y=SCREENHEIGHT / 2;

    int xTab=39;
    int yTab=23;

    int chestOpen=0;
    int time1=0;
    int time2=0;
    int time3=0;
    int time4=0;
    int time5=2;
    int close=0;
    int nbDeath;
    Text timerMap=CreateText("",CreateColor(250,250,250),75);
    TextSetPosition(timerMap,CreateVec2((SCREENWIDTH/2)-50,SCREENHEIGHT-80));
    Text pause=CreateText("PAUSE",CreateColor(250,250,250),150);
    TextSetPosition(pause,CreateVec2((SCREENWIDTH/2-175),(SCREENHEIGHT/2-70)));
    int load=0;

    int imgPos=32;

    Image sky;

    //init an array for the spawning points
    std::vector<Vec2> *respawnPoint = new std::vector<Vec2>();

    std::vector<unsigned int> *respawnPointTimer = new std::vector<unsigned int>();
    //std::vector<int> *spawningQueue = new std::vector<int>();

    unsigned int timeToCheckSpawns = Time() + 1000;

    switch (numMap)
    {
    case 1:
        sky= CreateImage("data/skyrim.jpg");
        break;

    case 2:
        sky= CreateImage("data/Borderlands2.jpg");
        break;

    case 3:
        sky= CreateImage("data/deadpool.jpg");
        break;

    case 4:
        sky= CreateImage("data/map4.jpg");
        break;

    }

    ImgSetScale(sky,(SCREENWIDTH/ImgGetSize(sky).x),(SCREENHEIGHT/ImgGetSize(sky).y));
    ImgSetPosition(sky,x-ImgGetSize(sky).x/2,y-ImgGetSize(sky).y/2);

    int tab[40][23];
    Image matImg[40][23];

    for (int i=0; i<=xTab; i++)
    {
        for(int j=0; j<=yTab; j++)
        {
            tab[i][j]=0;
        }
    }

    FILE* fichier = NULL;

    switch (numMap)
    {
    case 0:
        fichier = fopen("data/maptest.txt","r");
        break;

    case 1:
        fichier = fopen("data/map1.txt","r");
        break;

    case 2:
        fichier = fopen("data/map2.txt","r");
        break;

    case 3:
        fichier = fopen("data/map3.txt","r");
        break;

    case 4:
        fichier = fopen("data/map4.txt","r");
        break;

    }
    for (int j=0; j<=yTab; j++)
    {
        for(int i=0; i<=xTab; i++)
        {
            fscanf(fichier,"%d",&tab[i][j]);
        }
    }

    fclose(fichier);

    for (int i=0; i<=xTab; i++)
    {
        for(int j=0; j<=yTab; j++)
        {
            if (tab[i][j]==1)
            {
                switch (numMap)
                {
                case 1:
                    matImg[i][j]=CreateImage("data/sol.png");
                    break;

                case 2:
                    matImg[i][j]=CreateImage("data/dirt.jpg");
                    break;

                case 3:
                    matImg[i][j]=CreateImage("data/bois.jpg");
                    break;

                case 4:
                    matImg[i][j]=CreateImage("data/blanc.png");
                    break;
                }

            }
            else if (tab[i][j]==2)
            {
                matImg[i][j]=CreateImage("data/herbe.jpg");
            } //if we find a respawn point we add it to the array
            else if(tab[i][j] == -1)
            {
                /*
                    we create the point and then we see if we reach the limit of the array
                    if yes we reallocate
                */

                respawnPoint->push_back(CreateVec2((i*32) + 16, (j*32) + 16));
                respawnPointTimer->push_back(0);
            }
            else if (tab[i][j]==3)
            {
                matImg[i][j]=CreateImage("data/spike.png");
            }
            else if (tab[i][j]==4)
            {
                matImg[i][j]=CreateImage("data/water.jpg");
            }

        }
    }


    for (int i=0; i<=xTab; i++)
    {
        for(int j=0; j<=yTab; j++)
        {
            if (tab[i][j]==1 || tab[i][j]==2 || tab[i][j]==3 || tab[i][j]==4)
            {
                ImgSetPosition(matImg[i][j],imgPos*i,imgPos*j);
            }
        }
    }

    myTime timer;
    timer.Init();

     Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024); //Initialisation de l'API Mixer

      Mix_Music *musique; //Création du pointeur de type Mix_Music
      switch (numMap)
      {
          case 1:
              musique = Mix_LoadMUS("data/Skyrim-music.mp3");
              break;

          case 2:
              musique = Mix_LoadMUS("data/Borderlands-music.mp3");
              break;

          case 3:
              musique = Mix_LoadMUS("data/Deadpool-music.mp3");
              break;

          case 4:
              musique = Mix_LoadMUS("data/map4-music.mp3");
              break;
      }
       //Chargement de la musique
      Mix_PlayMusic(musique, -1); //Jouer infiniment la musique

    // ImgSetScale(perso[0].img,1.42f,1.42f);

    //init spawnpoints in players

    for(unsigned int i = 0; i < perso.size(); i++)
    {
        perso[i].setPosition(getRandomSpawnPoint(*respawnPoint, respawnPointTimer));
        perso[i].setSpawnPoints(respawnPoint, respawnPointTimer);
    }

    std::vector<Text>scorePerso;
    for(unsigned int i=0;i<perso.size();i++)
    {
        switch(i)
        {
        case 0:
            scorePerso.push_back(CreateText("",CreateColor(0,0,250),50));
            TextSetPosition(scorePerso[i],CreateVec2(50,SCREENHEIGHT-50));
            break;
        case 1:
            scorePerso.push_back(CreateText("",CreateColor(250,0,0),50));
            TextSetPosition(scorePerso[i],CreateVec2(SCREENWIDTH-50,SCREENHEIGHT-50));
            break;
        case 2:
            scorePerso.push_back(CreateText("",CreateColor(0,250,0),50));
            TextSetPosition(scorePerso[i],CreateVec2(50,50));
            break;
        case 3:
            scorePerso.push_back(CreateText("",CreateColor(255,255,0),50));
            TextSetPosition(scorePerso[i],CreateVec2(SCREENWIDTH-50,50));
            break;
        }

    }

    bool isPaused = false;
    unsigned int time = 0;

    while (!KeyGetOneShot(SDLK_ESCAPE) && close < 1)
    {


            //1sec passed since last checkout
            if(timeToCheckSpawns <= Time())
            {
                timeToCheckSpawns = Time() + 500;
                checkSpawnPoints(respawnPointTimer);
            }

            timer.update();

            DrawImage(sky);
            for (int i=0; i<=xTab; i++)
            {
                for(int j=0; j<=yTab; j++)
                {
                    if (tab[i][j]==1 || tab[i][j]==2 || tab[i][j]==3 || tab[i][j]==4)
                    {
                        DrawImage(matImg[i][j]);
                    }
                }
            }

            if(!isPaused)
            {
                 close = checkForWinner(perso);

                if(time1==0)
                {
                    time1=time;
                }
                time2=(time-time1)/1000;
                if(time2>=60)
                {
                    time3+=1;
                    time1=time;
                }
                time4=59-time2;
                time5=2-time3;
                if(time4<10)
                {
                    TextSetText(timerMap,"0%d: 0%d", time5,time4);
                }
                else
                {
                    TextSetText(timerMap,"0%d: %d", time5,time4);
                }
                if(time4==0 && time5==0)
                {
                    close=1;
                }
            }

            //DrawRect(perso[0].bottomLeft * 32, perso[0].bottomRight * 32, perso[0].topRight * 32, perso[0].topLeft * 32, CreateColor(255,0,0));


            for( unsigned int i = 0; i < perso.size(); i++)
            {
                //pause when start is pressed
                 if(StickGetClick(i, 7) || KeyGetOneShot(SDLK_0))
                {
                    isPaused = !isPaused;

                }

                TextSetText(scorePerso[i],"%d", perso[i].nbKill);
                DrawText(scorePerso[i]);

                if(!isPaused)
                {

                    time += timer.deltaTime / 2;

                    //gravity
                    perso[i].checkBottomCollision(tab);



                    // update weapon and anim
                    perso[i].update(timer.deltaTime / 4, tab, &perso);

                    if(StickGetAxis(i,1)<-0.5)
                    {
                        perso[i].lookUp();
                    }
                    else if(StickGetAxis(i,1)>0.5)
                    {
                        perso[i].lookDown();
                    }
                    else
                    {
                        perso[i].ResetDirection();
                    }

                    //when the player try to move, we check if he can and then move or not
                    if(KeyGet(SDLK_LEFT) || StickGetAxis(i,0)<-0.5)
                    {
                        if(!perso[i].checkLeftCollision(tab))
                        {
                            perso[i].goLeft();
                        }

                    }
                    else if(KeyGet(SDLK_RIGHT)|| StickGetAxis(i,0)>0.5)
                    {
                        if(!perso[i].checkRightCollision(tab))
                        {
                            perso[i].goRight();
                        }

                    }



                    if(KeyGetOneShot(SDLK_UP)|| StickGetClick(i,0))
                    {
                        if(!perso[i].checkTopCollision(tab))
                        {
                            perso[i].goUp();
                        }
                    }

                    if(KeyGetOneShot(SDLK_1))
                    {
                        perso[i].Die(getRandomSpawnPoint(*respawnPoint, respawnPointTimer));
                        perso[i].gun.reload();
                    }

                    if(KeyGetOneShot(SDLK_9))
                        close = -1;

                    if(KeyGetOneShot(SDLK_2)|| StickGetClick(i,1))
                    {
                        perso[i].Shoot();
                    }

                    if(KeyGetOneShot(SDLK_3)|| StickGetClick(i,2))
                    {
                        perso[i].Attack();
                    }

                    if(perso[i].checkLeftSpikeCollision(tab) || perso[i].checkTopSpikeCollision(tab) || perso[i].checkBottomSpikeCollision(tab) || perso[i].checkRightSpikeCollision(tab))
                    {
                        perso[i].Die(getRandomSpawnPoint(*respawnPoint, respawnPointTimer));
                        perso[i].gun.reload();
                        if (perso[i].nbKill>0)
                            perso[i].nbKill=perso[i].nbKill-1;
                    }

                    if(perso[i].checkWaterCollision(tab))
                    {
                        perso[i].noAmmo();
                    }


                }
                else
                {
                    DrawImage(perso[i].getImage());
                    DrawText(pause);
                }



        }
        if(load==0)
        {
            for( unsigned int i = 0; i < perso.size(); i++)
            {
                perso[i].Die(getRandomSpawnPoint(*respawnPoint, respawnPointTimer));
                perso[i].gun.reload();
            }
            load=1;
        }

        DrawText(timerMap);
        RefreshScreen();
    }
    nbDeath=perso[0].InitNbDeath();
    for( unsigned int i = 0; i < perso.size(); i++)
    {
        nbDeath+=perso[i].returnDeath();
    }
    perso[0].updateDeath(nbDeath);

    //shows scores

    while(!KeyGetOneShot(SDLK_RETURN))
    {
        for(int i = 0; i < perso.size(); i++)
        {
            DrawText(scorePerso[i]);
        }

        RefreshScreen();
    }

    //needed because every players use the same img
    perso[0].gun.Free();
    perso[0].weapon.Free();

    for(unsigned int i = 0; i < perso.size(); i++)
    {
        perso[i].Free();
    }

    FreeImage(matImg[0][0]);

    endMatch(perso);

    Mix_FreeMusic(musique);

    Mix_CloseAudio();
    Mix_Quit();

    //used for the keygetoneshot
   // RefreshScreen();

    //save scores
}
