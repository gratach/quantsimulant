
export class Wertb{
	constructor(daten, fremd){
		this.daten = daten
		this.beobachter = []
		if(fremd){
			this.fremd = fremd
			this.fremdnr = fremd.beobachte(this.wandel.bind(this))
		}
	}
	beobachten(wert, func){
		i = this.beobachter.indexOf(null)
		if(i == -1){
			i = this.beobachter.length
			this.beobachter.push(func)
		}
		else{
			this.beobachter[i] = func
		}
	}
	vergessen(nr){
		this.beobachter[nr] = null
		for(let i = this.beobachter.length - 1; i > 0 ; i--){
			if(this.beobachter[i] != null)
				continue
			this.beobachter.splice(i)
		}
	}
	wandel(neu){
		this.daten = neu
	}
}

function MapPlus(map, plus){//map = {kenn : [werte]} plus = {kenn : wert}
	plus.forEach((val, key)=>{
		if(!map.has(key))
			map.set(key, [])
		map.set(key, map.get(key).concat(val))
	})
}
function MapMix(map, plus){//map, plus = Map{ding : Map{kenn : [werte]}} 
	plus.forEach((val, key)=>{
		if(!map.has(key))
			map.set(key, new Map())
		let g = map.get(key)
		val.forEach((v, k)=>{
			if(!g.has(k))
				g.set(k, [])
			g.get(k).push(val)
		})
	})
}


import EMSIMPO from "./main.js"
export class Numerik{
	constructor(anmeld){
		this.anmeld = anmeld
		this.Modul = EMSIMPO()
		this.Modul.onRuntimeInitialized = this.bereit.bind(this)
		
		this.wasm_def(this.Modul)
	}
	wasm_def(Modul){
		this.re_f64_wertbei = Modul.cwrap('re_f64_wertbei', 'number', ['number','number','number']);
		this.im_f64_wertbei = Modul.cwrap('im_f64_wertbei_nachreichen', 'number', []);
		this.f64_wertbei = Modul.cwrap('f64_wertbei', 'number', ['number','number','number']);
		this.Dsetzekonst = Modul.cwrap('Dsetzekonst', null, ['number','number','number']);
		this.cf64_SchwingungsZerlegung = Modul.cwrap('cf64_SchwingungsZerlegung', null, ['number','number','number','number']);
		this.cf64_SchwingungsZusammensetzung = Modul.cwrap('cf64_SchwingungsZusammensetzung', null, ['number','number','number','number']);
		this.schwingungs_zer = Modul.cwrap('DSchwingungsZerlegung', null, ['number','number','number','number','number','number']);
		this.schwingungs_zus = Modul.cwrap('DSchwingungsZusammensetzung', null, ['number','number','number','number','number','number']);
		this.Cf64quad_f64iff = Modul.cwrap('Cf64quad_f64iff', 'number', ['number','number','number','number']); 
		this.Cf64Mittlere_abw = Modul.cwrap('Cf64Mittlere_abw', 'number', ['number','number','number']); 
		this.Cf64Mittlerer_Betrag = Modul.cwrap('Cf64Mittlerer_Betrag', 'number', ['number','number']);
		this.getf64 = Modul.cwrap('getf64', 'number', ['number']);
		this.setf64 = Modul.cwrap('setf64', null, ['number','number']);
		this.mache_arbeiter = Modul.cwrap('mache_arbeiter', null, ['number']);
		this.copy_vek = Modul.cwrap('copy_vek', null, ['number','number','number']);
		
		
		for (let k in this.klassen) {
			if (this.klassen.hasOwnProperty(k)) {
				let kl = this.klassen[k]
				this[k] = kl
				kl.boss = this
				kl.setze_werte(this, Modul)
			}
		}
	}
	bereit(){
		this.anmeld()
	}
	zeige(f){
		
	}
	klassen = {
		wandelpot : class{
			constructor(zeit, laenge, anf, end, aufloesung_rad, modus){
				let ich = this.constructor
				this.zeit = zeit;
				this.laenge = laenge;
				this.modus = modus;
				this.anf = anf;
				this.end = end;
				this.aufloesung_rad = aufloesung_rad;
				this.ptr = ich.mache(zeit, laenge, anf, end, aufloesung_rad, modus);
				this.wertbeiptr = ich.wertbeiptr(this.ptr);
				this.fertig = () =>{
					ich.free(this.ptr);
				}
				this.strich = (xanf, xend, yanf, yend, zeit) =>{
					ich.strich(this.ptr, xanf, xend, yanf, yend, zeit, this.aufloesung_rad, this.modus);
				}
				this.naechster_frame = (zeit)=>{return ich.naechsterframe(this.ptr, zeit);};
				this.letzter_frame = (zeit)=>{return ich.letzterframe(this.ptr, zeit);};
				this.aktueller_frame = (zeit)=>{return ich.aktuellerframe(this.ptr, zeit);};
				this.loescheframe = (zeit)=>{return ich.loescheframe(this.ptr, zeit);};
			}
			getframeinfo(jetzt){
				let ich = this.constructor
				var len = ich.framelen(this.ptr, jetzt);
				var anf = ich.frameanf(this.ptr, jetzt);
				var end = ich.frameend(this.ptr, jetzt);
				return {
					len: len,
					arr: this.constructor.boss.getf64ArrVonPtr(ich.frameptr(this.ptr, jetzt), len),
					xanf: anf, 
					xend: end,
					deltax: (end - anf)/(len - 1)};
			}
			auswerten(x, t){
				return this.constructor.boss.f64_wertbei(this.wertbeiptr, x, t);
			}
			
			schreibe(schr){
				var ges = ich.framezahl(this.ptr);
				schr.schreibeUInt(ges);
				var jetzt = ich.anfzeit(this.ptr);
				for(let i = 0; i < ges; i++){
					var len = ich.framelen(this.ptr, jetzt);
					schr.schreibeF64(jetzt);
					schr.schreibeUInt(len);
					schr.schreibeF64(ich.frameanf(this.ptr, jetzt));
					schr.schreibeF64(ich.frameend(this.ptr, jetzt));
					schr.schreibeUInt8(ich.framemodus(this.ptr, jetzt));
					schr.schreibeF64Daten(getByteArrVonPtr(ich.frameptr(this.ptr, jetzt), len * l_f64));//push(new Blob(getByteArrVonPtr(ich.frameptr(this.ptr, jetzt), len * l_f64)));
					jetzt = this.naechster_frame(jetzt);
				}
				
			}
			static titel = "F64WANDELPOT"
			
			static lese(les){
				var framezahl = les.leseUInt();
				var zeit = les.leseF64();
				var len = les.leseUInt();
				var anf = les.leseF64();
				var end = les.leseF64();
				var modus = les.leseUInt8();
				var wp = new f64_WANDEL_POT(zeit, len, anf, end, 0.0000000001, 0);
				buff = f64_WANDEL_POT.frameptr(wp.ptr, zeit);
				buff = getf64ArrVonPtr(buff, len);
				buff.set(les.leseF64Daten(len), 0);
				var buff;
				for(let i = 1; i < framezahl;i ++){
					zeit = les.leseF64();
					len = les.leseUInt();
					anf =  les.leseF64();
					end = les.leseF64();
					modus = les.leseUInt8();
					f64_WANDEL_POT.beschaffeFrame(wp.ptr, zeit, 0.0000000001, len, anf, end, modus);
					buff = f64_WANDEL_POT.frameptr(wp.ptr, zeit);
					buff = getf64ArrVonPtr(buff, len);
					buff.set(les.leseF64Daten(len), 0);
				}
				return wp;
			}
			static segmentLesen(les){
				var neu = f64_WANDEL_POT.lese(les);
				if(wandelpot)
					wandelpot.fertig();
				wandelpot = neu;
				berechnen.setze_pot(neu);
			}
			
			//TODO Konzepte.push(f64_WANDEL_POT);
			
			static setze_werte(s1d, Modul){
				this.boss = s1d
				this.mache = Modul.cwrap('mache_f64_WANDEL_POT', 'number', ['number','number','number','number','number','number']);
				this.free = Modul.cwrap('free_f64_WANDEL_POT', null, ['number']);
				this.strich = Modul.cwrap('strich_f64_WANDEL_POT', null, ['number','number','number','number','number','number','number','number']);
				this.beschaffeFrame = Modul.cwrap('beschaffe_s_f64_KEY_FRAME', 'number', ['number','number','number','number','number','number','number']);
				this.naechsterframe = Modul.cwrap('naechster_keyframe_f64_WANDEL_POT', 'number', ['number','number']);
				this.aktuellerframe = Modul.cwrap('aktueller_keyframe_f64_WANDEL_POT', 'number', ['number','number']);
				this.letzterframe = Modul.cwrap('letzter_keyframe_f64_WANDEL_POT', 'number', ['number','number']);
				this.framezahl = Modul.cwrap('frame_zahl_f64_WANDEL_POT', 'number', ['number']);
				this.anfzeit = Modul.cwrap('anfangszeit_f64_WANDEL_POT', 'number', ['number']);
				this.framelen = Modul.cwrap('frame_len_f64_WANDEL_POT', 'number', ['number', 'number']);
				this.frameanf = Modul.cwrap('frame_anf_f64_WANDEL_POT', 'number', ['number', 'number']);
				this.frameend = Modul.cwrap('frame_end_f64_WANDEL_POT', 'number', ['number', 'number']);
				this.framemodus = Modul.cwrap('frame_modus_f64_WANDEL_POT', 'number', ['number', 'number']);
				this.frameptr = Modul.cwrap('frame_ptr_f64_WANDEL_POT', 'number', ['number', 'number']);
				this.wertbeiptr = Modul.cwrap('wertbei_ptr_f64_WANDEL_POT', 'number', ['number']);
				this.loescheframe = Modul.cwrap('loesche_frame_f64_WANDEL_POT', null, ['number','number']);
			}
		},
		moden : class{
			constructor(ord, xanf, xend, mass){
				let ich = this.constructor
				this.ord = ord;
				this.xanf = xanf;
				this.xend = xend;
				this.mass = mass;
				this.ptr = ich.mache(ord, xanf, xend, mass);
				this.modenptr = ich.getmoden(this.ptr);
				this.arr = ich.boss.getf64ArrVonPtr(this.modenptr, (ord + 1) * 2);
				this.fertig = ()=>{ich.free(this.ptr);};
				this.normiere = ()=>{
					ich.normiere(this.ptr);
				}
				this.setze_masse = (mass) => {this.mass = mass; this.werte_auffrischen();}
				this.werte_auffrischen = () => {ich.setze_werte(this.ptr, this.xanf, this.xend, this.mass)}
			}
			static setze_werte(s1d, Modul){
				this.mache = Modul.cwrap('mache_f_f64_KASTEN_MODEN', 'number', ['number','number','number','number']);
				this.free = Modul.cwrap('free_f_f64_KASTEN_MODEN', null, ['number']);
				this.getmoden = Modul.cwrap('getmoden_f_f64_KASTEN_MODEN', 'number', ['number']);
				this.normiere = Modul.cwrap('normiere_f_f64_KASTEN_MODEN', null, ['number']);
				this.setze_werte = Modul.cwrap('setze_werte_f_f64_KASTEN_MODEN', null, ['number','number','number','number']);
			}
		},
		rungekutta : class{
			constructor(bytelen, matrix, eul, vadd){
				let ich = this.constructor
				this.bytelen = bytelen;
				this.matrix = matrix;
				this.vadd = vadd;
				this.eul = eul;
				this.ptr = ich.mache(matrix.ordnung, bytelen, eul.ptr,  vadd.ptr, matrix.ptr);
				this.iteration = (alt_ptr, neu_ptr, zeit, zeitschritt)=>{
					ich.iteration(this.ptr, alt_ptr, neu_ptr, zeit, zeitschritt);
				}
				this.fertig = ()=>{
					ich.free(this.ptr);
				}
				
			}
			static setze_werte(s1d, Modul){
				this.mache = Modul.cwrap('mache_f64_RUNGEKUTTA', 'number', ['number','number','number','number','number']);
				this.free = Modul.cwrap('free_f64_RUNGEKUTTA', null, ['number']);
				this.iteration = Modul.cwrap('RungeKuttaAnwenden_f64', null, ['number', 'number','number','number','number']);
			}
		},
		schroed : class{
			constructor(wertbeiptr, laenge, xanf, xend, masse, periodisch){
				var gitterabstand = (xend - xanf)/(laenge - 1);
				this.ptr = this.constructor.mache(wertbeiptr, laenge, 1/(2*masse*gitterabstand*gitterabstand), xanf, gitterabstand, periodisch);
				this.fertig = ()=>{
					this.constructor.free(this.ptr);
				}
				//this.setzepot = ()=>{this.constructor.potsetzen(this.ptr ,doppelpot.alt.ptr, doppelpot.neu.ptr);}
				//this.setzeparam  = (masse, gitterabstand)=>{this.constructor.laplacegewichtsetzen(this.ptr ,1/(2*masse*gitterabstand*gitterabstand));}
			}
			setze_pot_ptr(ptr){
				this.constructor.potsetzen(this.ptr, ptr);
			}
			static setze_werte(s1d, Modul){
				this.mache = Modul.cwrap('mache_f_f64_SCHROED_WANDEL', 'number', ['number','number','number','number','number','number']);
				this.free = Modul.cwrap('free_f_f64_SCHROED_WANDEL', null, ['number']);
				this.potsetzen = Modul.cwrap('potsetzen_f_f64_SCHROED_WANDEL', null, ['number','number','number']);
				this.laplacegewichtsetzen = Modul.cwrap('laplacegewichtungsetzen_f_f64_SCHROED_WANDEL', null, ['number','number']);
				this.periodischsetzen = Modul.cwrap('periodischsetzen_f_f64_SCHROED_WANDEL', null, ['number','number']);
			}
		},
		
		plus : class{
			constructor(len){
				this.ptr = this.constructor.mache(len);
				this.fertig = ()=>{
					this.constructor.free(this.ptr);
				}
			}
			static setze_werte(s1d, Modul){
				this.mache = Modul.cwrap('mache_f_f64_VEK_ADD', 'number', ['number']);
				this.free = Modul.cwrap('free_f_f64_VEK_ADD', null, ['number']);
			}
		},
		
		rungekuttamatrix : class{
			constructor(ord, arr){
				this.structptr = this.constructor.mache(ord);
				this.ptr = this.constructor.getarr(this.structptr);
				this.arr = this.constructor.boss.getf64ArrVonPtr(this.ptr, 2 * ord + ord*(ord-1)/2);
				this.arr.set(arr);
				this.ordnung = ord;
				this.fertig = ()=>{
					this.constructor.free(this.structptr);
				};
			}
			static setze_werte(s1d, Modul){
				this.mache = Modul.cwrap('mache_s_f64_RUNGEKUTTA_MATRIX', 'number', ['number']);
				this.free = Modul.cwrap('free_s_f64_RUNGEKUTTA_MATRIX', null, ['number']);
				this.getarr = Modul.cwrap('get_s_f64_RUNGEKUTTA_MATRIX_ptr', 'number', ['number']);
			}
		},
		
		CDoppelVek : class{
			constructor(gitterpunkte, xanf, xend, beschr){
				this.p1 = new this.constructor.boss.CVek(gitterpunkte, xanf, xend, beschr);
				this.p2 = new this.constructor.boss.CVek(gitterpunkte, xanf, xend, beschr);
				this.vertauscht = false;
				this.tausche = () => {
					this.vertauscht = !this.vertauscht;
				}
				this.dochnicht = false
				this.fertig = ()=>{
					this.p1.fertig();
					this.p2.fertig();
				};
				this.altzuneu = () => {
					var alt = this.alt;
					var neu = this.neu;
					copy_vek(neu, alt, 2*l_f64 * this.len);
				}
				//TODO: komponenten.push(this);
			}
			get alt(){return this.vertauscht ? this.p2 : this.p1;}
			get neu(){return this.vertauscht ? this.p1 : this.p2;}
			
			static setze_werte(s1d, Modul){
				this.tausche = Modul.cwrap('tausche_f_cf64_VEK', null, ['number', 'number']);
			}
		},
		
		DoppelVek : class{
			constructor(gitterpunkte, xanf, xend, beschr){
				this.len = gitterpunkte;
				this.p1 = new this.constructor.boss.Vek(gitterpunkte, xanf, xend, beschr);
				this.p2 = new this.constructor.boss.Vek(gitterpunkte, xanf, xend, beschr);
				this.vertauscht = false;
				this.tausche = () => {
					this.vertauscht = !this.verauscht;
				}
				this.fertig = ()=>{
					this.p1.fertig();
					this.p2.fertig();
				};
				this.altzuneu = () => {
					var alt = this.alt;
					var neu = this.neu;
					copy_vek(neu, alt, l_f64 * this.len);
				}
				//TODO: komponenten.push(this);
			}
			get alt(){return this.vertauscht ? this.p2 : this.p1;}
			get neu(){return this.vertauscht ? this.p1 : this.p2;}
			static setze_werte(s1d, Modul){
				this.tausche = Modul.cwrap('tausche_f_rf64_VEK', null, ['number', 'number']);
			}
		},
		
		CVek : class{
			constructor(len, xanf, xend, beschr){
				this.len = len;
				this.xanf = xanf;
				this.xend = xend;
				this.breite = xend - xanf;
				this.ptr = this.constructor.boss.Modul._malloc(len*16);
				this.constructor.boss.Dsetzekonst(this.ptr, len*2, 0);
				this.wertbei = this.constructor.neuwertbei(this.ptr, len, xanf, xend);
				this.wertbeiR = this.constructor.neuwertbeiR(this.wertbei);
				this.wertbeiI = this.constructor.neuwertbeiI(this.wertbei);
				this.wertbeiQ = this.constructor.neuwertbeiQ(this.wertbei);
				this.getre = (nr) => {return this.constructor.boss.getf64(this.ptr + nr*2*l_f64);};
				this.setre = (nr, wert) => {return this.constructor.boss.setf64(this.ptr + nr*2*l_f64, wert);};
				this.getim = (nr) => {return this.constructor.boss.getf64(this.ptr + nr*2*l_f64 + l_f64);};
				this.setim = (nr, wert) => {return this.constructor.boss.setf64(this.ptr + nr*1*l_f64 + l_f64, wert);};
				this.getarr = () => {return this.constructor.boss.getf64ArrVonPtr(this.ptr, len * 2)};
				this.fuelle = (ptr, zeit) => {
					this.constructor.fuelle(this.ptr, this.len, ptr, this.xanf, (this.xend-this.xanf)/(this.len - 1), zeit);
				};
				this.dochnicht = false
				this.fertig = () => {this.dochnicht = true}
				//TODO: komponenten.push(this);
			}
			randbedingung(periodisch){
				this.constructor.randbedingung(this.ptr, this.len, periodisch);
			}
			R_auswerten(x){
				return this.constructor.boss.f64_wertbei(this.wertbeiR, x, 0);
			}
			I_auswerten(x){
				return this.constructor.boss.f64_wertbei(this.wertbeiI, x, 0);
			}
			Q_auswerten(x){
				return this.constructor.boss.f64_wertbei(this.wertbeiQ, x, 0);
			}
			strich(xanf, xend, yanf_r, yanf_i, yend_r, yend_i){
				this.constructor.strich(this.ptr, this.len, this.xanf, this.lendurchbreite, xanf, xend, yanf_r, yanf_i, yend_r, yend_i);
				
			}
			normiere(){
				this.multipliziere(1/Math.sqrt(this.betrquad));
			}
			multipliziere(wert){
				this.constructor.multipliziere(this.ptr, this.len, wert);
			}
			rotiere(winkel){
				this.constructor.rotiereptr(this.ptr, this.len, winkel);
			}
			get lendurchbreite(){return (this.len - 1)/this.breite;}
			get bytelen(){return this.len*2*l_f64;}
			get betrquad(){return this.constructor.Cf64betrquad(this.ptr, this.len, this.breite);}
			
			static setze_werte(s1d, Modul){
				this.fertig = ()=>{
					Modul._free(this.ptr);
					this.constructor.entfwertbei(this.wertbeiptr);
					this.constructor.entfwertbeiX(this.wertbeiR);
					this.constructor.entfwertbeiX(this.wertbeiI);
					this.constructor.entfwertbeiX(this.wertbeiQ);
				};
				if(this.dochnicht)
					return this.fertig()
				this.randbedingung =  Modul.cwrap('randbedingung_cf64_VEK', 'number', ['number','number','number']);
				this.neuwertbei =  Modul.cwrap('mache_f_cf64_VEK', 'number', ['number','number','number','number']);
				this.Cf64betrquad =  Modul.cwrap('Cf64betrquad', 'number', ['number','number','number']);
				this.neuwertbeiR =  Modul.cwrap('mache_f_cf64_R_WERTBEI', 'number', ['number']);
				this.neuwertbeiI =  Modul.cwrap('mache_f_cf64_I_WERTBEI', 'number', ['number']);
				this.neuwertbeiQ =  Modul.cwrap('mache_f_cf64_Q_WERTBEI', 'number', ['number']);
				this.rotiereptr =  Modul.cwrap('rotiere_cf64_ARR', null, ['number','number','number']);
				this.entfwertbei =  Modul.cwrap('free_f_cf64_VEK', null, ['number']);
				this.entfwertbeiX =  Modul.cwrap('free_Cf64R_Cf64I_f_cf64_Q_WERTBEI', null, ['number']);
				this.fuelle = Modul.cwrap('fuelle_f_cf64_VEK', null, ['number', 'number', 'number', 'number', 'number', 'number']);
				this.multipliziere = Modul.cwrap('cf64_multiplizieren', null, ['number','number', 'number']);
				this.strich = Modul.cwrap('strich_cf64_ARR', null, ['number','number', 'number','number','number', 'number','number','number', 'number','number']); 
			}
		},
		
		Vek : class{
			constructor(len, xanf, xend, beschr){
				this.len = len;
				this.xanf = xanf;
				this.xend = xend;
				this.breite = xend - xanf;
				this.ptr = this.constructor.boss.Modul._malloc(len*8);
				this.constructor.boss.Dsetzekonst(this.ptr, len, 0);
				this.wertbeiptr = this.constructor.neuwertbei(this.ptr, len, xanf, xend);
				this.get = (nr) => {return this.constructor.boss.getf64(this.ptr + nr*l_f64);};
				this.set = (nr, wert) => {return this.constructor.boss.setf64(this.ptr + nr*l_f64, wert);};
				this.getarr = () => {return this.constructor.boss.getf64ArrVonPtr(this.ptr, len)};
				this.dochnicht = false
				this.fertig = () => {this.dochnicht = true}
				//TODO: komponenten.push(this);
			}
			auswerten(x){
				return this.constructor.boss.f64_wertbei(this.wertbeiptr, x, 0);
			}
			get bytelen(){return this.len*l_f64;}
			static setze_werte(s1d, Modul){
				this.fertig = ()=>{
					Modul._free(this.ptr);
				}
				if(this.dochnicht)
					return this.fertig()
				this.Modul = Modul
				this.neuwertbei =  Modul.cwrap('mache_f_rf64_VEK', 'number', ['number','number','number','number']);
				this.entfwertbei =  Modul.cwrap('free_f_rf64_VEK', null, ['number']);
			}
		},
	}
	
	
	getf64ArrVonPtr(ptr, len){
		var p = ptr >> 3;
		return this.Modul.HEAPF64.subarray(p, p + len);
	}
	getByteArrVonPtr(ptr, len){
		return this.Modul.HEAPU8.subarray(ptr, ptr + len);
	}
}

var numerik = null
var numerik_melden = []
export function standard(){
	numerik = new Numerik(()=>{
			for(m of numerik_melden)
				m.ja(numerik)
			numerik_melden = null
		})
	return new Promise((ja, nein)=>{
		if(numerik.geladen)
			ja(numerik)
		else{
			numerik_melden.push({ja, nein})
		}
	})
}

export class Ansicht{
	constructor(bild, ding, inhalte, events){
		this.bild = bild
		this.ding = ding
		this.inhalte = inhalte
		this.events = events
	}
	verdecke(){
		this.ding.abmelden(this)
	}Modul
}

export class Fenster{
	constructor(div, ding){
		this.div = div
		if(ding){
			this.ansicht = ding.zeige(this)
		}
		else{
				this.ansicht = undefined
		}
	}
	zeige(ding){
		if(this.ansicht)
			this.ansicht.verdecke(this)
		ding.zeige(this)
		this.ding = ding
	}
	verdecke(){
		if(this.ansicht)
			this.ansicht.verdecke(this)
		this.ansicht = undefined
	}
}
