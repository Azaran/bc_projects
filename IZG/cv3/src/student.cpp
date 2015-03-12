/******************************************************************************
 * Laborator 03 - Zaklady pocitacove grafiky - IZG
 * ibehun@fit.vutbr.cz
 *
 * $Id:$xvecer18

 * Popis:
 *    soubor obsahuje definice barevnych datovych typu + pomocne funkce pro
 *    praci s nimi 
 *
 * Opravy a modifikace:
 * -
 */


#include "base.h"
#include "student.h"
#include "globals.h"
#ifdef __linux__
#include <limits.h>
#endif
#include <vector>
#include <time.h>
#include <stdio.h>

/******************************************************************************
 ******************************************************************************
 Funkce vraci pixel z pozice x, y. Pozor: nehlida velkost frame_bufferu, pokud 
 je dana souradnice mimo hranice. */
S_RGBA getPixel(int x, int y)
{
  return frame_buffer[y * width + x];
}

/******************************************************************************
 ******************************************************************************
 Funkce vlozi pixel na pozici x, y. Pozor: nehlida velkost frame_bufferu, pokud 
 je dana souradnice mimo hranice. */
void putPixel(int x, int y, S_RGBA color)
{
  frame_buffer[y * width + x] = color;
}

/************************** Ukoly na cviceni *********************************/

/******************************************************************************
 ******************************************************************************
 Seminkove vyplnovani */
void floodFill(int x, int y, S_RGBA color)
{
  SeedStack sd_stck;  // zasobnik seminek

  /////////// SEM DOPLNTE VAS KOD ///////////////////

  // ulozime si barvu pocatecniho seminka
  S_RGBA seed_color = getPixel(x,y);
  // ak se pocatecni barva na pozici seminka shoduje s cilovou, koncime
  if (seed_color == color)
    return;
  // jinak si ulozime na zasobnik souradnice pocatocniho bodu	
  S_Point seed;
  seed.x = x;
  seed.y = y;
  sd_stck.push_back(seed);

  while (!sd_stck.empty()) {  // dokud neni zasobnik pradny delej
    // vybereme seminko z vrcholu zasobniku
    seed = sd_stck.back();
    sd_stck.pop_back();
    // pokud je seminko na pozici mimo kreslicu plochu, pokracujeme na dalsi seminko v zasobniku	
    if ((seed.x < width && seed.x >= 0) && \
	(seed.y < height && seed.y >= 0))
    {
      seed = sd_stck.back();
      sd_stck.pop_back();
    }
    // pokud je barva ina ako ulozena barva prvniho seminka, pokracujeme na dalsi seminko ulozene v zasobniku
    if (color != getPixel(seed.x, seed.y))
    {
      seed = sd_stck.back();
      sd_stck.pop_back();
    }
    // inak obarvime bod na suradnicich seminka
    else
      putPixel(seed.x, seed.y, color);
    // pak ulozime na zasobnik okolne body seminka
    seed.x++;
    sd_stck.push_back(seed);
    seed.x--;
    seed.y++;
    sd_stck.push_back(seed);
    seed.y -=2;
    sd_stck.push_back(seed);
    seed.x--;
    seed.y++;
    sd_stck.push_back(seed);
    // rohove body
    /*
       sd_stck.push_back(seed.x + 1, seed.y + 1);
       sd_stck.push_back(seed.x - 1, seed.y + 1);
       sd_stck.push_back(seed.x + 1, seed.y - 1);
       sd_stck.push_back(seed.x - 1, seed.y - 1);
       */
  }

  ////////////////////////////////////////////////////
}


/******************************************************************************
 ******************             BODOVANY UKOL                ******************
 ******************************************************************************
 Radkove vyplnovani */
void invertFill(const S_Point * points, const int size,  const S_RGBA & color1, const S_RGBA & color2)
{
  // zasobnik hran polygonu 
  LineStack lines;

  // ze seznamu vrcholu se vytvori seznam hran
  for(int i = 0; i < size; i++)
  {
    ///////////////////////////////////////////////////////
    //////////////////// VAS UKOL c. 1 ////////////////////
    //
    //   upravte kod pro vytvareni seznamu hran tak, aby se:
    //   a.) hrany rovnobezne s radkem preskocili
    //   b.) orientace hran byla zdola nahoru
    //
    ///////////////////////////////////////////////////////

    S_Point p_1, p_2;

    p_1 = points[i];	    // bod 1
    if (i < size-1) 		// bod 2, posledni hrana bude z posledniho bodu do prvniho
    {
      p_2 = points[i+1];
    }
    else
    {
      p_2 = points[0]; 
    }
    if (p_1.y != p_2.y)
    {
      if (p_1.y > p_2.y)
      {
	SWAP(p_1.x, p_2.x);
	SWAP(p_1.y, p_2.y);
      }
      lines.push_back(S_Line(p_1, p_2));
    }
  }

  // vytvoreni masky a jeji naplneni nulovymi hodnotami
  S_Mask m(width, height);

  // modifikacia masky pre kazdu hranu
  for(int i = 0; i < lines.size(); i++) 
  {
    S_Line line_i = lines[i];	// vybereme hranu ze seznamu

    // od pocatecniho bodu ke koncovemu (krome koncoveho) ve smeru osy Y najdeme prusecik hrany s radkem
    for(int y_i = line_i.point1.y; y_i < line_i.point2.y; y_i++) 
    {
      ///////////////////////////////////////////////////////
      //////////////////// VAS UKOL c. 2 ////////////////////
      //
      //        spoctete prusecik "intersection" 
      //
      ///////////////////////////////////////////////////////

      int intersection = 0; // prusecik hrany s radkem "row"
      float k = (float)(line_i.point1.x - line_i.point2.x)/ \
		(line_i.point1.y - line_i.point2.y);
      intersection = ABS(ROUND(line_i.point1.x - k*line_i.point1.y )) ;
      // invertuj masku napravo od tohoto bodu
      for(int x_i = intersection; x_i < width; x_i++) 
      {
	if(m.mask[x_i][y_i] == 0)
	{
	  m.mask[x_i][y_i] = 1;
	}
	else
	{
	  m.mask[x_i][y_i] = 0;
	}
      }
    }
  }

  // vykresli pixely podle hodnot v masce
  for(int y_i = 0; y_i < height; y_i++)
  {
    for(int x_i = 0; x_i < width; x_i++)
    {
      if(m.mask[x_i][y_i] == 1)
      {
	///////////////////////////////////////////////////////
	//////////////////// VAS UKOL c. 3 ////////////////////
	//
	// provedte vyplnovani pomoci gradientu - prechod barev
	// z color1 do color2 na zaklade x-ove suradnice. Staci
	// spocitat interpolacni faktor "factor" pro spravne
	// namichani barev pro aktualni x-ovu souradnici
	//
	///////////////////////////////////////////////////////

	float factor = 0.0; // interpolacni faktor pro namichani barev
	factor = (float)x_i / width;
	// interpolace barvy 
	S_RGBA color =  S_RGBA::interpolate(color1, color2, factor);

	putPixel(x_i, y_i, color);
      }
    }
  }

  // Prekresleni hranic polygonu
  for(int i = 0; i < lines.size(); i++) 
  {
    ///////////////////////////////////////////////////////
    //////////////////// VAS UKOL c. 4 ////////////////////
    //
    //   doplnte kod pro prekresleni hranic polygonu
    //   pro barvu hran pouzijte barvu color1
    //
    ///////////////////////////////////////////////////////
    S_Line line = lines.back();
    lines.pop_back();
    int x = line.p1.x;
    int y = line.p1.y;
    float k = (float)(x - line.p2.x) / (y - line.p2.y);
    for(y; y < line.p2.y; y++) 
    {
      x = ABS(ROUND(x - k*y )) ;
      putPixel(x, y, color1);
    }
  }

}
