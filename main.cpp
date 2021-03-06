#include <iostream>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "EcualizarImagen.hpp"
#include "HistogramaAcumulado.hpp"

using namespace std;
using namespace cv;
using namespace FSIV;

struct opcionesPrograma
{
  inline opcionesPrograma()
  {
    radioFlag = false;
    imagenFlag = false;
    mascaraFlag = false;
    showPicture = false;
    biecualizar = false;
    save = false;
    radioValue = 0;
    imagenPath = NULL;
    mascaraPath = NULL;
    espacio = ESPACIO_COLOR_HSV;
  }

  bool radioFlag;
  bool imagenFlag;
  bool mascaraFlag;
  bool showPicture;
  bool save;
  bool biecualizar;

  int radioValue;
  char * imagenPath;
  char * mascaraPath;

  string output;

  enum espacioColor espacio;
};

int main(int argc, char ** argv)
{
  struct opcionesPrograma opciones;
  Mat imagen;
  Mat mascara;
  Mat resultado;

  vector<int> parametrosSalida;

  int variable;

  while((variable = getopt(argc, argv, "r:s:m:e:io:b")) != -1)
    {
      switch(variable)
	{
	case 'r':
	  opciones.radioFlag = true;
	  opciones.radioValue = atoi(optarg);
	  break;

	case 's':
	  opciones.imagenFlag = true;
	  opciones.imagenPath = optarg;
	  break;
	  
	case 'o':
	  opciones.save = true;
	  opciones.output = optarg;

	case 'm':
	  opciones.mascaraFlag = true;
	  opciones.mascaraPath = optarg;
	  break;

	case 'b':
	  opciones.biecualizar = true;
	  break;

	case 'e':
	  if(strstr(optarg, "HSV") != NULL)
	    {
	      opciones.espacio = ESPACIO_COLOR_HSV; 
	    }
	  else if(strstr(optarg, "CIE") != NULL)
	    {
	      opciones.espacio = ESPACIO_COLOR_CIE;
	    }
	  else if(strstr(optarg, "YCrCb") != NULL)
	    {
	      opciones.espacio = ESPACIO_COLOR_YCrCb;
	    }
	  else if(strstr(optarg, "HSL") != NULL)
	    {
	      opciones.espacio = ESPACIO_COLOR_HSL;
	    }
	  else
	    {
	      cerr << "Error en el parametro -e" << endl;
	      cerr << "Debe de usarlo con uno de las siguientes opciones: " << endl;
	      cerr << "HSV para hacer la conversion en el espacio de color HSV (por defecto)" << endl;
	      cerr << "CIE para hacer la conversion en el espacio de color CIE" << endl;
	      cerr << "YCrCb para hacer la conversion en el espacio de color YCrCb" << endl;
	      cerr << "HSL para hacer la conversion en el espacio de color HSL" << endl;
	      exit(1);
	    }
	  break;

	case 'i':
	  opciones.showPicture = true;
	  break;

	case '?':
	  if(optopt == 'r')
	    {
	      cerr << "La opcion r requiere un parametro entero" << endl;
	      exit(1);
	    }
	  else if(optopt == 's')
	    {
	      cerr << "La opcion s requiere un parametro" << endl;
	      exit(1);
	    }
	  else if(optopt == 'm')
	    {
	      cerr << "La opcion m requiere un parametro" << endl;
	      exit(1);
	    }
	  else if(optopt == 'e')
	    {
	      cerr << "La opcion e requiere un parametro" << endl;
	      exit(1);
	    }
	  else if(optopt == 'o')
	    {
	      cerr << "La opcion o requiere un parametro" << endl;
	      exit(1);
	    }
	  else if(isprint(optopt))
	    {
	      cerr << "Se desconoce el parametro " << optopt << endl;
	      exit(1);
	    }
	  else
	    {
	      cerr << "Parametro desconocido no imprimible" << endl;
	      exit(1);
	    }
	  break;
	}
    };

  if(opciones.imagenFlag == false)
    {
      cerr << "Es obligatorio especificar el path de la imagen" << endl;
      cerr << "Use -s <path> " << endl;
      exit(1);
    }
  else
    {
      imagen = imread(opciones.imagenPath, CV_LOAD_IMAGE_UNCHANGED);
    }

  if((opciones.biecualizar == true) && (opciones.radioFlag == true))
    {
      cerr << "Los parametros -b y -r son incompatibles" << endl;
      exit(1);
    }

  if(opciones.mascaraFlag == true)
    {
      mascara = imread(opciones.mascaraPath, CV_LOAD_IMAGE_UNCHANGED);
    }

  EcualizarImagen claseEcualizar(opciones.radioValue, opciones.espacio);
  
  claseEcualizar.setBiecualizacion(opciones.biecualizar);
 
  resultado = claseEcualizar.ecualizar(imagen, mascara);

  if(opciones.showPicture == true)
    {
      int ancho = resultado.rows;
      int alto = resultado.cols;
      double ratio = static_cast<double>(ancho)/static_cast<double>(alto);

      while(ancho >= 800)
	{
	  ancho /= 2;
	}

      alto = static_cast<int>(ratio * ancho);

      cvStartWindowThread();
      cvNamedWindow("Imagen ecualizada", WINDOW_NORMAL);
      cvNamedWindow("Imagen original", WINDOW_NORMAL);

      imshow("Imagen ecualizada", resultado);
      imshow("Imagen original", imagen);

      cvResizeWindow("Imagen ecualizada", ancho, alto);
      cvResizeWindow("Imagen original", ancho, alto);

      cvWaitKey(0);

      cvDestroyWindow("Imagen ecualizada");
      cvDestroyWindow("Imagen original");
    }

  if(opciones.save == true)
    {
      parametrosSalida.push_back(CV_IMWRITE_PNG_COMPRESSION);
      parametrosSalida.push_back(9);
  
      imwrite(opciones.output, resultado, parametrosSalida);
    }

  return 0;
}
