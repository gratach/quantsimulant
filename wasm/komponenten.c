#include "komponenten.h"

idef(i_cf64_WERTBEI, s_f64_RAUMZEIT, _Complex double)
idef(i_rf64_WERTBEI, s_f64_RAUMZEIT, double)

int EMSCRIPTEN_KEEPALIVE main(int count, char ** args){
	//EM_ASM({bereit();});
	return 0;
}

idef(i_VEK_WANDEL, s_VEK_WANDEL, void)

void EMSCRIPTEN_KEEPALIVE RungeKuttaAnwenden_f64(s_f64_RUNGEKUTTA* selbst, void* alt, void* neu, double zeit, double zeitschritt){
	int k;
	int rungezaehler = 0;// zaehlet durch d
	int ordnung = selbst->ordnung;
	double * rungematrix  = selbst->rungematrix;
	//EM_ASM({alert($0 + " tuu "+ $1)}, alt, neu);
	int i;
	void** tempvektoren = selbst->tempvektoren;
	int sizeofvector = selbst->bytelaenge;
	i_VEK_WANDEL aenderung = selbst -> aenderung;
	i_VEK_WANDEL adiereauf = selbst -> adiereauf;
	double gewicht;
	
	
	//EM_ASM({alert($0 + " ooh "+ $1)}, ((double*)neu)[0], ((double*)neu)[1]);
	memcpy(neu, alt, sizeofvector);
	for (i=0;i<ordnung;i++){
		for(k=0;k<i;k++){
			gewicht = rungematrix[rungezaehler++];
			if(gewicht!= 0)
				i_VEK_WANDEL_f(&adiereauf, (s_VEK_WANDEL){tempvektoren[k], neu, gewicht*zeitschritt});
		}
		i_VEK_WANDEL_f(&aenderung, (s_VEK_WANDEL){neu, tempvektoren[i], zeit + rungematrix[rungezaehler++]*zeitschritt});
		memcpy(neu, alt, sizeofvector);
	}
	for (i=0;i<ordnung;i++){
		//EM_ASM({alert($0 + ": "+ $1 + " " +$2)}, i , ((double*)tempvektoren[i])[0], ((double*)tempvektoren[i])[1]);
		gewicht = rungematrix[rungezaehler++];
		if(gewicht!= 0){
			i_VEK_WANDEL_f(&adiereauf, (s_VEK_WANDEL){tempvektoren[i], neu, gewicht*zeitschritt});
		}
	}
	//EM_ASM({alert($0 + " tuu "+ $1)}, ((double*)neu)[0], ((double*)neu)[1]);
	//EM_ASM({alert($0 + " uxx "+ $1)}, alt, neu);
}
s_f64_RUNGEKUTTA* EMSCRIPTEN_KEEPALIVE mache_f64_RUNGEKUTTA(int ord, int bytelaenge, i_VEK_WANDEL aenderung, i_VEK_WANDEL adiereauf, double* rungematrix){
	void** tempvektoren = malloc(sizeof(tempvektoren)*ord);
	int i;
	for(i = 0; i<ord; i++)
		tempvektoren[i] = calloc(bytelaenge, 1);
	s_f64_RUNGEKUTTA* mall = malloc(sizeof(s_f64_RUNGEKUTTA));
	*mall = (s_f64_RUNGEKUTTA){ord, tempvektoren,  bytelaenge, aenderung, adiereauf, rungematrix};
	return mall;
}
void EMSCRIPTEN_KEEPALIVE free_f64_RUNGEKUTTA(s_f64_RUNGEKUTTA* rkd){
	int ord = rkd->ordnung;
	void** tempvektoren = rkd->tempvektoren;
	int i;
	for(i = 0; i<ord; i++)
		free(tempvektoren[i]);
	free(tempvektoren);
	free(rkd);
}

static inline _Complex double f64_eulerpot_plus(_Complex double links, _Complex double mitte, _Complex double rechts, double pot, double laplace_gewichtung){
    return (laplace_gewichtung *(links + rechts - mitte * 2) - mitte * pot) * I ;
}

fdef(f_f64_SCHROED_WANDEL, i_VEK_WANDEL, s_f64_SCHROED_WANDEL, s_VEK_WANDEL, void){
	double complex * alt = arg.alt;
	double complex * neu = arg.neu;
	i_rf64_WERTBEI* pot = selbst->pot;
	double laplace_gewichtung = selbst -> laplace_gewichtung;
	double deltax = selbst->deltax;
	double param = arg.parameter;
	size_t laenge = selbst->vektorlaenge;
	int periodisch = selbst->periodisch;
	int i;
	double ort = selbst->anfang;
				//EM_ASM({alert($0);}, laenge);
	for(i = 2;i<laenge;i++){
		ort += deltax;
		neu[i-1] = f64_eulerpot_plus(alt[i-2], alt[i-1], alt[i], i_rf64_WERTBEI_f(pot, (s_f64_RAUMZEIT){ort, param}), laplace_gewichtung);
	}
	if(periodisch){
		neu[0] = neu[laenge - 2];
		neu[laenge - 1] = neu[1];
	}
}
i_VEK_WANDEL* EMSCRIPTEN_KEEPALIVE mache_f_f64_SCHROED_WANDEL(i_rf64_WERTBEI* pot, int len, double laplace_gewichtung, double anfang, double deltax, int periodisch){
	i_VEK_WANDEL* ptr = malloc(sizeof(i_VEK_WANDEL));
	*ptr = f_f64_SCHROED_WANDEL_neu((s_f64_SCHROED_WANDEL){pot, len, laplace_gewichtung, anfang, deltax, periodisch});
	return ptr;
}
void EMSCRIPTEN_KEEPALIVE potsetzen_f_f64_SCHROED_WANDEL(i_VEK_WANDEL* pvw, i_rf64_WERTBEI* pot){
	s_f64_SCHROED_WANDEL* ptr = pvw->inhalt;
	ptr->pot = pot;
}
void EMSCRIPTEN_KEEPALIVE laplacegewichtungsetzen_f_f64_SCHROED_WANDEL(i_VEK_WANDEL* pvw, double laplace_gewichtung){
	((s_f64_SCHROED_WANDEL*)pvw->inhalt)->laplace_gewichtung = laplace_gewichtung;
}
void EMSCRIPTEN_KEEPALIVE periodischsetzen(i_VEK_WANDEL* pvw, int periodisch){
	((s_f64_SCHROED_WANDEL*)pvw->inhalt)->periodisch = periodisch;
}
void EMSCRIPTEN_KEEPALIVE free_f_f64_SCHROED_WANDEL(i_VEK_WANDEL* pvw){
	free(pvw->inhalt);
	free(pvw);
}

fdef(f_f64_VEK_ADD, i_VEK_WANDEL, int, s_VEK_WANDEL, void){
    double * alt = arg.alt;
    double * neu = arg.neu;
    double param = arg.parameter;
    size_t laenge = *selbst;
    int i;
    for(i=0;i<laenge;i++)
    {
        neu[i] += alt[i] * param;
    }
    
}
i_VEK_WANDEL* EMSCRIPTEN_KEEPALIVE mache_f_f64_VEK_ADD(int len){
	i_VEK_WANDEL* ptr = malloc(sizeof(i_VEK_WANDEL));
	*ptr = f_f64_VEK_ADD_neu(len);
	return ptr;
}
void EMSCRIPTEN_KEEPALIVE free_f_f64_VEK_ADD(i_VEK_WANDEL* pvw){
	free(pvw->inhalt);
	free(pvw);
}

double EMSCRIPTEN_KEEPALIVE getf64(void* ort){
	double* ptr = ort;
	return *ptr;
}
void EMSCRIPTEN_KEEPALIVE setf64(void* ort, double wert){
	double* ptr = ort;
	*ptr = wert;
}

s_f64_RUNGEKUTTA_MATRIX* EMSCRIPTEN_KEEPALIVE mache_s_f64_RUNGEKUTTA_MATRIX(int ordnung){
	s_f64_RUNGEKUTTA_MATRIX * r = malloc(sizeof(s_f64_RUNGEKUTTA_MATRIX));
	double * ptr = malloc(sizeof(double) * (2 * ordnung + ordnung*(ordnung-1)/2));
	*r = (s_f64_RUNGEKUTTA_MATRIX){ordnung, ptr};
	return r;
}
void EMSCRIPTEN_KEEPALIVE free_s_f64_RUNGEKUTTA_MATRIX(s_f64_RUNGEKUTTA_MATRIX* rkm){
	free(rkm->matrix);
	free(rkm);
}
double* EMSCRIPTEN_KEEPALIVE get_s_f64_RUNGEKUTTA_MATRIX_ptr(s_f64_RUNGEKUTTA_MATRIX* rkm){
	return rkm->matrix;
}

fdef(f_cf64_VEK, i_cf64_WERTBEI, s_cf64_VEK, s_f64_RAUMZEIT, _Complex double){
	double r = (arg.raum - selbst->anf) * selbst->lendurchbreite;
	int u = floor(r);
	int o = u + 1;
	double complex ru = 0;
	double complex ro = 0;
	if(u > -1 && u < selbst->len)
		ru = selbst->ptr[u];
	if(o > -1 && o < selbst->len)
		ro = selbst->ptr[o];

	r -= u;
	return r * ro + (1 - r) * ru;
}
//modus für rand: 'n' null; 's' schleife; 'r' reflektiert; 'l' letzterWert
void* EMSCRIPTEN_KEEPALIVE mache_f_cf64_VEK(void* vek, double laenge, double anf, double end){
	i_cf64_WERTBEI* rueck = malloc(sizeof(i_cf64_WERTBEI));
	*rueck = f_cf64_VEK_neu((s_cf64_VEK){vek, laenge, anf, (laenge - 1) / (end - anf)});
	return rueck;
}
void EMSCRIPTEN_KEEPALIVE free_f_cf64_VEK(void* ptr){
	i_cf64_WERTBEI_leeren(ptr);
	free(ptr);
}

void EMSCRIPTEN_KEEPALIVE randbedingung_cf64_VEK(_Complex double* ptr, int len, int periodisch){
	if(periodisch){
		ptr[0] = ptr[len -2];
		ptr[len - 1] = ptr[1];
	}
	else{
		ptr[0] = 0;
		ptr[len -1] = 0;
	}
}

void EMSCRIPTEN_KEEPALIVE rotiere_cf64_ARR(_Complex double* ptr, int len, double winkel){
	int i;
	_Complex double rot = exp(I*winkel);
	for(i=0;i<len;i++)
		ptr[i] *= rot;
		
}

void EMSCRIPTEN_KEEPALIVE tausche_f_cf64_VEK(i_cf64_WERTBEI* wb, _Complex double* ptr){
	((s_cf64_VEK*)wb->inhalt)->ptr = ptr;
}

fdef(f_cf64_R_WERTBEI, i_rf64_WERTBEI, i_cf64_WERTBEI, s_f64_RAUMZEIT, double){
	return creal(i_cf64_WERTBEI_f(selbst, arg));
}


//Der erste und letzte Wert gehen jeweils halb ein weil sie nur zur Hälfte im Intervall liegen.
double EMSCRIPTEN_KEEPALIVE Cf64betrquad(_Complex double* ptr, int len, double breite){
	double summe = 0;
	_Complex double wert;
	int i;
	for(i=len-2;i>0;i--){
		wert = ptr[i];
		summe += creal(wert)*creal(wert) + cimag(wert)*cimag(wert);
	}
	wert = ptr[0];
	summe += 0.5*(creal(wert)*creal(wert) + cimag(wert)*cimag(wert));
	wert = ptr[len - 1];
	summe += 0.5*(creal(wert)*creal(wert) + cimag(wert)*cimag(wert));
	return summe * breite / (len - 1);
}

i_rf64_WERTBEI* EMSCRIPTEN_KEEPALIVE mache_f_cf64_R_WERTBEI(i_cf64_WERTBEI* ptr){
	i_rf64_WERTBEI* r = malloc(sizeof(i_rf64_WERTBEI));
	*r = f_cf64_R_WERTBEI_neu_ptr(ptr);
	return r;
}

fdef(f_cf64_I_WERTBEI, i_rf64_WERTBEI, i_cf64_WERTBEI, s_f64_RAUMZEIT, double){
	return cimag(i_cf64_WERTBEI_f(selbst, arg));
}
i_rf64_WERTBEI* EMSCRIPTEN_KEEPALIVE mache_f_cf64_I_WERTBEI(i_cf64_WERTBEI* ptr){
	i_rf64_WERTBEI* r = malloc(sizeof(i_rf64_WERTBEI));
	*r = f_cf64_I_WERTBEI_neu_ptr(ptr);
	return r;
}

fdef(f_cf64_Q_WERTBEI, i_rf64_WERTBEI, i_cf64_WERTBEI, s_f64_RAUMZEIT, double){
	_Complex double r = i_cf64_WERTBEI_f(selbst, arg);
	double re = creal(r);
	double im = cimag(r);
	return re*re + im*im;
}
i_rf64_WERTBEI* EMSCRIPTEN_KEEPALIVE mache_f_cf64_Q_WERTBEI(i_cf64_WERTBEI* ptr){
	i_rf64_WERTBEI* r = malloc(sizeof(i_rf64_WERTBEI));
	*r = f_cf64_Q_WERTBEI_neu_ptr(ptr);
	return r;
}
void EMSCRIPTEN_KEEPALIVE free_Cf64R_Cf64I_f_cf64_Q_WERTBEI(i_rf64_WERTBEI* ptr){
	free(ptr);
}

void EMSCRIPTEN_KEEPALIVE copy_vek(void* von, void* zu, int bytelen){
	memcpy(zu, von, bytelen);
}


fdef(f_f64_KASTEN_MODEN, i_cf64_WERTBEI, s_f64_KASTEN_MODEN, s_f64_RAUMZEIT, _Complex double){
	int max = selbst->ord + 1;
	double ort = (arg.raum - selbst->xoff)*selbst->xstretch;
	double complex dreh = selbst->dreh * arg.zeit * I;
	double complex* arr = selbst->moden;
	int i;
	double complex r = 0;
	for(i = 1; i < max; i++){
		r += arr[i]*sin(ort*i)*exp(i*i*dreh);
	}
	return r;
}

void* EMSCRIPTEN_KEEPALIVE mache_f_f64_KASTEN_MODEN(int ord, double xanf, double xend, double mass){
	double breite = xend - xanf;
	double dreh = -M_PI*M_PI/(2*mass*breite*breite);
	double xstretch = M_PI / breite;
	double complex* arr = malloc((ord + 1) * sizeof(double)*2);
	i_cf64_WERTBEI* ptr = malloc(sizeof(i_cf64_WERTBEI));
	*ptr = f_f64_KASTEN_MODEN_neu((s_f64_KASTEN_MODEN){arr, ord, dreh, xstretch, xanf, breite});
	return ptr;
}
_Complex double* EMSCRIPTEN_KEEPALIVE getmoden_f_f64_KASTEN_MODEN(i_cf64_WERTBEI* ptr){
	return ((s_f64_KASTEN_MODEN*)ptr->inhalt)->moden;
}

void EMSCRIPTEN_KEEPALIVE setze_werte_f_f64_KASTEN_MODEN(i_cf64_WERTBEI* ptr, double xanf, double xend, double mass, int periodisch){
	s_f64_KASTEN_MODEN* km = (s_f64_KASTEN_MODEN*)ptr->inhalt;
	double breite = xend - xanf;
	double dreh = -M_PI*M_PI/(2*mass*breite*breite);
	double xstretch = M_PI / breite;
	km->dreh = -M_PI*M_PI/(2*mass*breite*breite);
	km->breite = breite;
	km->xstretch = xstretch;
	km->xoff = xanf;
}

void EMSCRIPTEN_KEEPALIVE normiere_f_f64_KASTEN_MODEN(i_cf64_WERTBEI* ptr){
	s_f64_KASTEN_MODEN* km = (s_f64_KASTEN_MODEN*)ptr->inhalt;
	int max = km->ord + 1;
	_Complex double* moden = km->moden;
	_Complex double wert;
	double summe = 0;
	int i = 1;
	for(i = 1; i < max; i++){
		wert = moden[i];
		summe += creal(wert)*creal(wert) + cimag(wert)*cimag(wert);
	}
	double mul = sqrt(2 / (km->breite * summe));
	for(i = 1; i < max; i++){
		moden[i] = moden[i]*mul;
	}
}
void EMSCRIPTEN_KEEPALIVE free_f_f64_KASTEN_MODEN(i_cf64_WERTBEI* ptr){
	free(((s_f64_KASTEN_MODEN*)ptr->inhalt)->moden);
	i_cf64_WERTBEI_leeren(ptr);
	free(ptr);
}


double EMSCRIPTEN_KEEPALIVE Cf64quad_f64iff(double complex* p1, double complex* p2, int len, double intervall){
	int i;
	double r;
	double re;
	double im;
	for(i = 0; i<len; i++){
		re = creal(p1[i])-creal(p2[i]);
		im = cimag(p2[i])-cimag(p1[i]);
		r += re*re+im*im;
	}
	return r * intervall / (len - 1);
}

//der erste und letzte wert gehen halb ein
double EMSCRIPTEN_KEEPALIVE Cf64Mittlere_abw(double complex* p1, double complex* p2, int len){
	int i;
	double r = 0;
	for(i = len -2; i>0; i--){
		r += cabs(p1[i] - p2[i]);
	}
	r += 0.5*cabs(p1[0] - p2[0]);
	i= len - 1;
	r += 0.5*cabs(p1[i] - p2[i]);
	return r / i;
}

//der erste und letzte wert gehen halb ein
double EMSCRIPTEN_KEEPALIVE Cf64Mittlerer_Betrag(double complex* p1, int len){
	int i;
	double r = 0;
	for(i = len -2; i>0; i--){
		r += cabs(p1[i]);
	}
	r += 0.5*cabs(p1[0]);
	i= len - 1;
	r += 0.5*cabs(p1[i]);
	return r / i;
}

void EMSCRIPTEN_KEEPALIVE cf64_multiplizieren(double complex* p1, int len, double mul){
	int i;
	for(i = 0;i<len;i++){
		p1[i] *= mul;
	}
}

void EMSCRIPTEN_KEEPALIVE DSchwingungsZerlegung(double complex * inparr, int laenge, double complex * outarr, int ordnung, double masse,  double zeit){
	double dreh =  zeit*M_PI*M_PI/(laenge*laenge*2*masse);
	int n;
	int i;
	double complex integr;
	double complex winkel;
	double schritt;
	int lminuseins = (laenge - 1);
	for(n=1; n<=ordnung; n++){
		winkel = exp(dreh * n*n * I);
		schritt = n * M_PI / lminuseins;
		integr = 0;
		for(i = 0; i < laenge; i++){
			integr += sin(i*schritt)*inparr[i]*winkel;
		}
		outarr[n] = integr * 2 / lminuseins;
	}
}

void EMSCRIPTEN_KEEPALIVE DSchwingungsZusammensetzung(double complex * inparr, int ordnung, double complex * outarr, int laenge, double masse, double zeit){
	double dreh =  zeit*M_PI*M_PI/(laenge*laenge*2*masse);
	int n;
	int i;
	double complex winkel;
	double schritt;
	memset(outarr, 0, sizeof(double complex) * laenge);
	int lminuseins = (laenge - 1);
	for(n = 1; n<=ordnung; n++){
		winkel = exp((-dreh) * n*n * I);
		schritt = n * M_PI / lminuseins;
		for(i = 0; i < laenge; i++){
			outarr[i] += sin(i*schritt)*inparr[n]*winkel;
		}
	}
}


void EMSCRIPTEN_KEEPALIVE cf64_SchwingungsZerlegung(double complex * inparr, int laenge, double complex * outarr, int ordnung){
	int n;
	int i;
	double schritt;
	int lminuseins = (laenge - 1);
	_Complex double integr;
	for(n=1; n<=ordnung; n++){
		schritt = n * M_PI / lminuseins;
		integr = 0;
		for(i = 0; i < laenge; i++){
			integr += sin(i*schritt)*inparr[i];
		}
		outarr[n] = integr * 2 / lminuseins;
	}
}
void EMSCRIPTEN_KEEPALIVE cf64_SchwingungsZusammensetzung(double complex * inparr, int ordnung, double complex * outarr, int laenge){
	int n;
	int i;
	double schritt;
	memset(outarr, 0, sizeof(double complex) * laenge);
	int lminuseins = (laenge - 1);
	for(n = 1; n<=ordnung; n++){
		schritt = n * M_PI / lminuseins;
		for(i = 0; i < laenge; i++){
			outarr[i] += sin(i*schritt)*inparr[n];
		}
	}
}

static inline double s_rf_rf64_VEK_wertbei(s_rf64_VEK* selbst, double raum){
	double r = (raum - selbst->anf) * selbst->lendurchbreite;
	int u = floor(r);
	int o = u + 1;
	double ru = 0;
	double ro = 0;
	//EM_ASM({alert($0+", "+$1+", "+$2+", "+$3+", "+$4+", ")}, r, u, o, ru, ro);
	if(u > -1 && u < selbst->len)
		ru = selbst->ptr[u];
	if(o > -1 && o < selbst->len)
		ro = selbst->ptr[o];
	r -= u;
	return r * ro + (1 - r) * ru;
}
fdef(f_rf64_VEK, i_rf64_WERTBEI, s_rf64_VEK, s_f64_RAUMZEIT, double){
	return s_rf_rf64_VEK_wertbei(selbst, arg.raum);
}

void* EMSCRIPTEN_KEEPALIVE mache_f_rf64_VEK(void* vek, double laenge, double anf, double end){
	i_rf64_WERTBEI* rueck = malloc(sizeof(i_rf64_WERTBEI));
	*rueck = f_rf64_VEK_neu((s_rf64_VEK){vek, laenge, anf, (laenge - 1) / (end - anf)});
	return rueck;
}
void EMSCRIPTEN_KEEPALIVE free_f_rf64_VEK(void* ptr){
	i_rf64_WERTBEI_leeren(ptr);
	free(ptr);
}
void EMSCRIPTEN_KEEPALIVE tausche_f_rf64_VEK(i_rf64_WERTBEI* wb, double* ptr){
	((s_rf64_VEK*)wb->inhalt)->ptr = ptr;
}

void EMSCRIPTEN_KEEPALIVE fuelle_f_cf64_VEK(_Complex double* ptr, int len, i_cf64_WERTBEI* wb, double start, double schritt, double zeit){
	double ort = start;
	int i;
	for(i=0;i<len;i++){
		ort = start + schritt*i;
		ptr[i] = i_cf64_WERTBEI_f(wb,(s_f64_RAUMZEIT){ort, zeit});
	}
}

void EMSCRIPTEN_KEEPALIVE fuelle_f_rf64_VEK(double* ptr, int len, i_rf64_WERTBEI* wb, double start, double schritt, double zeit){
	double ort = start;
	int i;
	for(i=0;i<len;i++){
		ort = start + schritt*i;
		ptr[i] = i_rf64_WERTBEI_f(wb,(s_f64_RAUMZEIT){ort, zeit});
	}
}

void EMSCRIPTEN_KEEPALIVE Dsetzekonst(double* ptr, int len, double wert){
	int i;
	for(i=0;i<len;i++){
		ptr[i] = wert;
	}
}

double temp_im_wertbei;
double EMSCRIPTEN_KEEPALIVE f64_wertbei(void* ptr, double raum, double zeit){
	return i_rf64_WERTBEI_f(ptr,(s_f64_RAUMZEIT){raum, zeit});
}
double EMSCRIPTEN_KEEPALIVE re_f64_wertbei(void* ptr, double raum, double zeit){
	_Complex double r = i_cf64_WERTBEI_f(ptr,(s_f64_RAUMZEIT){raum, zeit});
	temp_im_wertbei = cimag(r);
	return creal(r);
}
double EMSCRIPTEN_KEEPALIVE im_f64_wertbei_nachreichen(){
	return temp_im_wertbei;
}


s_f64_KEY_FRAME* finde_s_f64_KEY_FRAME(s_f64_KEY_FRAME* aktuell, double zeit, double radius){
	double radzeit = zeit - radius;
	while(aktuell->zeit < radzeit){
		if(aktuell->nachher == NULL)
			break;
		else
			aktuell = aktuell->nachher;
	}
	radzeit = zeit + radius;
	while(aktuell->zeit > radzeit)
		if(aktuell->vorher == NULL)
			break;
		else
			aktuell = aktuell->vorher;
	return aktuell;
}

double wertbei_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, s_f64_RAUMZEIT arg){
	double zeit = arg.zeit;
	s_f64_KEY_FRAME* hinten = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	wp->aktuell = hinten;
	if(hinten->zeit > zeit || hinten->nachher == NULL){
		return s_rf_rf64_VEK_wertbei(&hinten->werte, arg.raum);
	}
	s_f64_KEY_FRAME* vorne = hinten->nachher;
	double vornewert = s_rf_rf64_VEK_wertbei(&vorne->werte, arg.raum);
	double hintenwert = s_rf_rf64_VEK_wertbei(&hinten->werte, arg.raum);
	if(vorne->modus == 0)
		return hintenwert;
	return (zeit-vorne->zeit)*(hintenwert-vornewert)/(hinten->zeit - vorne->zeit) + vornewert;
}

fdef(f_f64_WANDEL_POT, i_rf64_WERTBEI, s_f64_WANDEL_POT, s_f64_RAUMZEIT, double){
	return wertbei_f64_WANDEL_POT(selbst, arg);
}

s_f64_KEY_FRAME* mache_s_f64_KEY_FRAME(double zeit, double laenge, double anf, double end, i_rf64_WERTBEI* fuell, int modus){
	void* ptr = malloc(sizeof(double)*laenge);
	fuelle_f_rf64_VEK(ptr, laenge, fuell, anf, (end - anf)/(laenge - 1), zeit);
	s_f64_KEY_FRAME* kf = malloc(sizeof(s_f64_KEY_FRAME));
	*kf = (s_f64_KEY_FRAME){(s_rf64_VEK){ptr, laenge, anf, (laenge - 1) / (end - anf)} ,zeit,NULL,NULL, modus};
	return kf;
}
void free_s_f64_KEY_FRAME(s_f64_KEY_FRAME* kf){
	free(kf->werte.ptr);
	free(kf);
}

s_f64_WANDEL_POT* EMSCRIPTEN_KEEPALIVE mache_f64_WANDEL_POT(double zeit, double laenge, double anf, double end, double radius, int modus){
	i_rf64_WERTBEI temp = tempf_f64_NULL();
	s_f64_KEY_FRAME* kf = mache_s_f64_KEY_FRAME(zeit, laenge, anf, end, &temp, modus);
	s_f64_WANDEL_POT* r = malloc(sizeof(s_f64_WANDEL_POT));
	*r = (s_f64_WANDEL_POT){kf,kf,kf, laenge, anf, end, radius, modus, f_f64_WANDEL_POT_neu_ptr(NULL)};
	r->wertbei = f_f64_WANDEL_POT_neu_ptr(r);
	return r;
}
void EMSCRIPTEN_KEEPALIVE free_f64_WANDEL_POT(s_f64_WANDEL_POT* wpa){
	s_f64_KEY_FRAME* folgend = wpa->anfang;
	s_f64_KEY_FRAME* aktuell;
	while(folgend != NULL){
		aktuell = folgend;
		folgend = aktuell->nachher;
		free_s_f64_KEY_FRAME(aktuell);
	}
	free(wpa);
}


//findet oder erstellt keyframe bei Zeit
s_f64_KEY_FRAME* EMSCRIPTEN_KEEPALIVE beschaffe_s_f64_KEY_FRAME(s_f64_WANDEL_POT* wp, double zeit, double radius, int lenfallsneu, double anffallsneu, double endfallsneu, int modusfallsneu){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, radius);
	double abstest = aktuell->zeit - zeit;
	if(abstest <= radius && -abstest <= radius){
		return aktuell;
	}
	i_rf64_WERTBEI temp = f_f64_WANDEL_POT_neu_ptr(wp);
	s_f64_KEY_FRAME* neu = mache_s_f64_KEY_FRAME(zeit, lenfallsneu, anffallsneu, endfallsneu, &temp, modusfallsneu);
	if(aktuell->zeit < zeit){
		if(aktuell->nachher == NULL){
			aktuell->nachher = neu;
			neu->vorher = aktuell;
			wp->aktuell = neu;
			return neu;
		}
		else{
			aktuell = aktuell->nachher;
		}
	}
	else if(aktuell->vorher == NULL){
		aktuell->vorher = neu;
		neu->nachher = aktuell;
		wp->aktuell = neu;
		return neu;
	}
	s_f64_KEY_FRAME* vor = aktuell->vorher;
	vor->nachher = neu;
	neu->vorher = vor;
	neu->nachher = aktuell;
	aktuell->vorher = neu;
	wp->aktuell = neu;
	return neu;
}

void strich_rf64_VEK(s_rf64_VEK* werte, double anfx, double endx, double anfy, double endy){
	double temp;
	if(anfx>endx){
		temp = endx;
		endx = anfx;
		anfx = temp;
		temp = endy;
		endy = anfy;
		anfy = temp;
	}
	double anf = werte->anf;
	double lendurchbreite = werte->lendurchbreite;
	double* arr = werte->ptr;
	int len = werte->len;
	int intanf = (int)((anfx - anf)*lendurchbreite);
	int intend = (int)((endx - anf)*lendurchbreite);
//EM_ASM(alert($0), intanf);
	if(intanf == intend)
		return;
	double xleninv = 1.0 /(intend - intanf);
	double ylen = endy - anfy;
	int start = intanf > 0 ? intanf : 0;
	int stop = intend > len - 1 ? len : intend + 1;
	int i;
	for(i = start; i < stop; i++)
		arr[i] = (i - intanf)*ylen*xleninv + anfy;
}
void EMSCRIPTEN_KEEPALIVE strich_cf64_ARR(_Complex double* arr, int len, double anf, double lendurchbreite,double anfx, double endx, double anfy_r, double anfy_i, double endy_r, double endy_i){
	_Complex double anfy = anfy_r + I * anfy_i;
	_Complex double endy = endy_r + I * endy_i;
	if(anfx>endx){
		double temp = endx;
		endx = anfx;
		anfx = temp;
		_Complex double Ctemp = endy;
		endy = anfy;
		anfy = Ctemp;
	}
	int intanf = (int)((anfx - anf)*lendurchbreite);
	int intend = (int)((endx - anf)*lendurchbreite);
//EM_ASM(alert($0), intanf);
	if(intanf == intend)
		return;
	double xleninv = 1.0 /(intend - intanf);
	_Complex double ylen = endy - anfy;
	int start = intanf > 0 ? intanf : 0;
	int stop = intend > len - 1 ? len : intend + 1;
	int i;
	for(i = start; i < stop; i++){
		arr[i] = (i - intanf)*xleninv*ylen + anfy;
		EM_ASM(console.log($0), arr[i]);
	}
}

void EMSCRIPTEN_KEEPALIVE strich_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double anfx, double endx, double anfy, double endy, double zeit, double frameradius, int modus){
	s_f64_KEY_FRAME* fr = beschaffe_s_f64_KEY_FRAME(wp, zeit, frameradius, wp->ort_laenge, wp->ort_anf, wp-> ort_end, modus);
	strich_rf64_VEK(&(fr->werte),anfx,endx, anfy,endy);
}


double EMSCRIPTEN_KEEPALIVE aktueller_keyframe_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	return aktuell->zeit;
}
double EMSCRIPTEN_KEEPALIVE naechster_keyframe_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	if(aktuell->nachher != NULL)
		aktuell = aktuell->nachher;
	wp->aktuell = aktuell;
	return aktuell->zeit;
}
double EMSCRIPTEN_KEEPALIVE letzter_keyframe_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	if(aktuell->zeit != zeit)
		return aktuell->zeit;
	if(aktuell->vorher != NULL)
		aktuell = aktuell->vorher;
	wp->aktuell = aktuell;
	return aktuell->zeit;
}
double EMSCRIPTEN_KEEPALIVE frame_zahl_f64_WANDEL_POT(s_f64_WANDEL_POT* wp){
	s_f64_KEY_FRAME* aktuell = wp->anfang;
	int i = 1;
	while(aktuell->nachher != NULL){
		aktuell = aktuell->nachher;
		i += 1;
	}
	return i;
}
double EMSCRIPTEN_KEEPALIVE anfangszeit_f64_WANDEL_POT(s_f64_WANDEL_POT* wp){
	return wp->anfang->zeit;
}

double* EMSCRIPTEN_KEEPALIVE frame_ptr_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	return aktuell->werte.ptr;
}
double EMSCRIPTEN_KEEPALIVE frame_anf_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	return aktuell->werte.anf;
}
double EMSCRIPTEN_KEEPALIVE frame_end_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	return aktuell->werte.anf + (aktuell->werte.len - 1) / aktuell->werte.lendurchbreite;
}
int EMSCRIPTEN_KEEPALIVE frame_modus_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	return aktuell->modus;
}
double EMSCRIPTEN_KEEPALIVE frame_len_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	return aktuell->werte.len;
}
void EMSCRIPTEN_KEEPALIVE loesche_frame_f64_WANDEL_POT(s_f64_WANDEL_POT* wp, double zeit){
	s_f64_KEY_FRAME* aktuell = finde_s_f64_KEY_FRAME(wp->aktuell, zeit, 0);
	if(aktuell->nachher == NULL && aktuell->vorher == NULL)
		return;
	if(aktuell->vorher == NULL){
		wp->aktuell = aktuell->nachher;
		aktuell->nachher->vorher = NULL;
		wp->anfang = aktuell->nachher;
	}
	else {
		wp->aktuell = aktuell->vorher;
		if(aktuell->nachher == NULL){
			aktuell->vorher->nachher = NULL;
			wp->anfang = aktuell->vorher;
		}
		else{
			aktuell->vorher->nachher = aktuell->nachher;
			aktuell->nachher->vorher = aktuell->vorher;
		}
	}
	free_s_f64_KEY_FRAME(aktuell);
}
i_rf64_WERTBEI* EMSCRIPTEN_KEEPALIVE wertbei_ptr_f64_WANDEL_POT(s_f64_WANDEL_POT* wp){
	return &(wp->wertbei);
}

i_rf64_WERTBEI tempf_f64_NULL(){
	return f_f64_NULL_neu_ptr((int*)NULL);
}
fdef(f_f64_NULL, i_rf64_WERTBEI, int, s_f64_RAUMZEIT, double){return 0.0;}

