#ifndef def_schroed_komponenten
#define def_schroed_komponenten

#include <emscripten.h>
#include<math.h>
#include"funkmacro.h"
#include <tgmath.h>
#include<string.h>

/*typedef struct {} f64_1DEulerAenderung_args;
fdek(f64_1DEulerAenderung, i_VEK_WANDEL, f64_1DEulerAenderung_args, s_VEK_WANDEL, void)*/


typedef struct {double raum; double zeit;} s_f64_RAUMZEIT;
idek(i_cf64_WERTBEI, s_f64_RAUMZEIT, _Complex double)
idek(i_rf64_WERTBEI, s_f64_RAUMZEIT, double)

typedef struct {void* alt; void* neu; double parameter;} s_VEK_WANDEL;
idek(i_VEK_WANDEL, s_VEK_WANDEL, void)

typedef struct{int ordnung; void** tempvektoren;  int bytelaenge; i_VEK_WANDEL aenderung; i_VEK_WANDEL adiereauf; double* rungematrix;} s_f64_RUNGEKUTTA;
//fdek(rungekuttaD, arbeiter, s_f64_RUNGEKUTTA, void*, int);

typedef struct {i_rf64_WERTBEI* pot; size_t vektorlaenge; double laplace_gewichtung; double anfang; double deltax; int periodisch;} s_f64_SCHROED_WANDEL;
fdek(f_f64_SCHROED_WANDEL, i_VEK_WANDEL, s_f64_SCHROED_WANDEL, s_VEK_WANDEL, void);


fdek(f_f64_VEK_ADD, i_VEK_WANDEL, int, s_VEK_WANDEL, void);


fdek(f_cf64_R_WERTBEI, i_rf64_WERTBEI, i_cf64_WERTBEI, s_f64_RAUMZEIT, double);
fdek(f_cf64_I_WERTBEI, i_rf64_WERTBEI, i_cf64_WERTBEI, s_f64_RAUMZEIT, double);
fdek(f_cf64_Q_WERTBEI, i_rf64_WERTBEI, i_cf64_WERTBEI, s_f64_RAUMZEIT, double);

typedef struct {_Complex double* moden; int ord; double dreh; double xstretch; double xoff; double breite;} s_f64_KASTEN_MODEN;
fdek(f_f64_KASTEN_MODEN, i_cf64_WERTBEI, s_f64_KASTEN_MODEN, s_f64_RAUMZEIT, _Complex double);

typedef struct {_Complex double* ptr; int len; double anf; double lendurchbreite;} s_cf64_VEK;
fdek(f_cf64_VEK, i_cf64_WERTBEI, s_cf64_VEK, s_f64_RAUMZEIT, _Complex double)

typedef struct {double* ptr; int len; double anf; double lendurchbreite;} s_rf64_VEK;
fdek(f_rf64_VEK, i_rf64_WERTBEI, s_rf64_VEK, s_f64_RAUMZEIT, double)

typedef struct {int ordnung; double * matrix;} s_f64_RUNGEKUTTA_MATRIX;

typedef struct s_f64_KEY_FRAME {s_rf64_VEK werte; double zeit; struct s_f64_KEY_FRAME* vorher; struct s_f64_KEY_FRAME* nachher; int modus;}s_f64_KEY_FRAME;//modus: 0 = aprupt; 1 = linear;
typedef struct {s_f64_KEY_FRAME* anfang; s_f64_KEY_FRAME* aktuell; s_f64_KEY_FRAME* ende; double ort_laenge; double ort_anf; double ort_end; double frame_radius; int modus; i_rf64_WERTBEI wertbei;} s_f64_WANDEL_POT;
fdek(f_f64_WANDEL_POT, i_rf64_WERTBEI, s_f64_WANDEL_POT, s_f64_RAUMZEIT, double)

fdek(f_f64_NULL, i_rf64_WERTBEI, int, s_f64_RAUMZEIT, double)
i_rf64_WERTBEI tempf_f64_NULL();
#endif
