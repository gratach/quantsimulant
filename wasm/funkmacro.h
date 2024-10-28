#ifndef FUNKMACRO_FILE
#define FUNKMACRO_FILE

#include<stdlib.h>

#define machenix(arg1);
#define existiert(handlung) nix) handlung( 
#define mache_wenn_existiert(bedingung, arg) machenix(bedingung arg)

#define tys typedef struct

#define idek(funkartname, argumenttype,rueckgabetype)\
  typedef struct{\
    rueckgabetype (*aufruf)(void*,argumenttype);\
    void* inhalt;\
  } funkartname; \
  rueckgabetype funkartname ## _f (funkartname *self, argumenttype arg);\
  funkartname funkartname ## _neu (rueckgabetype(*aufrufarg)(void*,argumenttype), void* inhaltarg);\
  void funkartname ## _leeren(funkartname* arg);

#define idef(funkartname, argumenttype,rueckgabetype)\
  rueckgabetype funkartname ## _f (funkartname *self, argumenttype arg){\
    return (*self).aufruf((*self).inhalt, arg);\
  }\
  funkartname funkartname ## _neu (rueckgabetype(*aufrufarg)(void*,argumenttype), void* inhaltarg){\
    return (funkartname){.aufruf = aufrufarg, .inhalt = inhaltarg};\
  }\
  void funkartname ## _leeren(funkartname* arg){\
        free(arg->inhalt);\
  }


#define fdek(funkname, funkartname, eigentype, argumenttype, rueckgabetype)\
    rueckgabetype funkname ## _def(eigentype* selbst, argumenttype arg);\
    rueckgabetype funkname ## _intern(void* selb, argumenttype arg);\
	funkartname funkname ## _neu_ptr(eigentype* ptr);\
    funkartname funkname ## _neu(eigentype inhalt);

#define fdef(funkname, funkartname, eigentype, argumenttype, rueckgabetype)\
    rueckgabetype funkname ## _intern(void* selb, argumenttype arg){\
        return funkname ## _def((eigentype*)selb, arg);\
    }\
    funkartname funkname ## _neu_ptr(eigentype* ptr){\
        return funkartname ## _neu(funkname ## _intern, ptr);\
    }\
    funkartname funkname ## _neu(eigentype inhalt){\
        eigentype* platz = malloc(sizeof(inhalt));\
        *platz = inhalt;\
        return funkartname ## _neu(funkname ## _intern, platz);\
    }\
    rueckgabetype funkname ## _def(eigentype* selbst, argumenttype arg)

#endif
