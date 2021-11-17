// This file was generated by the following script:
//
//  $ ../../../tools/tzcompiler.py --input_dir ../../../tools/../../tz --output_dir /home/brian/dev/AceTime/src/ace_time/zonedb --tz_version 2019c --action zonedb --language arduino --scope basic --start_year 2000 --until_year 2050
//
// using the TZ Database files
//
//  africa, antarctica, asia, australasia, backward, etcetera, europe, northamerica, southamerica
//
// from https://github.com/eggert/tz/releases/tag/2019c
//
// DO NOT EDIT

#ifndef ACE_TIME_ZONEDB_ZONE_POLICIES_H
#define ACE_TIME_ZONEDB_ZONE_POLICIES_H

#include <ace_time/internal/ZonePolicy.h>

namespace ace_time {
namespace zonedb {

//---------------------------------------------------------------------------
// Supported zone policies: 65
//
extern const basic::ZonePolicy kPolicyAN;
extern const basic::ZonePolicy kPolicyAQ;
extern const basic::ZonePolicy kPolicyAS;
extern const basic::ZonePolicy kPolicyAT;
extern const basic::ZonePolicy kPolicyAV;
extern const basic::ZonePolicy kPolicyAW;
extern const basic::ZonePolicy kPolicyArmenia;
extern const basic::ZonePolicy kPolicyAus;
extern const basic::ZonePolicy kPolicyAzer;
extern const basic::ZonePolicy kPolicyBarb;
extern const basic::ZonePolicy kPolicyBrazil;
extern const basic::ZonePolicy kPolicyC_Eur;
extern const basic::ZonePolicy kPolicyCO;
extern const basic::ZonePolicy kPolicyCR;
extern const basic::ZonePolicy kPolicyCanada;
extern const basic::ZonePolicy kPolicyChatham;
extern const basic::ZonePolicy kPolicyChile;
extern const basic::ZonePolicy kPolicyCook;
extern const basic::ZonePolicy kPolicyCuba;
extern const basic::ZonePolicy kPolicyDhaka;
extern const basic::ZonePolicy kPolicyEU;
extern const basic::ZonePolicy kPolicyEUAsia;
extern const basic::ZonePolicy kPolicyEcuador;
extern const basic::ZonePolicy kPolicyEire;
extern const basic::ZonePolicy kPolicyFiji;
extern const basic::ZonePolicy kPolicyGhana;
extern const basic::ZonePolicy kPolicyGuat;
extern const basic::ZonePolicy kPolicyHK;
extern const basic::ZonePolicy kPolicyHaiti;
extern const basic::ZonePolicy kPolicyHoliday;
extern const basic::ZonePolicy kPolicyHond;
extern const basic::ZonePolicy kPolicyIran;
extern const basic::ZonePolicy kPolicyIraq;
extern const basic::ZonePolicy kPolicyJapan;
extern const basic::ZonePolicy kPolicyJordan;
extern const basic::ZonePolicy kPolicyLH;
extern const basic::ZonePolicy kPolicyLebanon;
extern const basic::ZonePolicy kPolicyMacau;
extern const basic::ZonePolicy kPolicyMauritius;
extern const basic::ZonePolicy kPolicyMexico;
extern const basic::ZonePolicy kPolicyMoldova;
extern const basic::ZonePolicy kPolicyMoncton;
extern const basic::ZonePolicy kPolicyMongol;
extern const basic::ZonePolicy kPolicyNC;
extern const basic::ZonePolicy kPolicyNZ;
extern const basic::ZonePolicy kPolicyNic;
extern const basic::ZonePolicy kPolicyPRC;
extern const basic::ZonePolicy kPolicyPakistan;
extern const basic::ZonePolicy kPolicyPara;
extern const basic::ZonePolicy kPolicyPeru;
extern const basic::ZonePolicy kPolicyPhil;
extern const basic::ZonePolicy kPolicyROK;
extern const basic::ZonePolicy kPolicyRussiaAsia;
extern const basic::ZonePolicy kPolicySA;
extern const basic::ZonePolicy kPolicySalv;
extern const basic::ZonePolicy kPolicySyria;
extern const basic::ZonePolicy kPolicyTaiwan;
extern const basic::ZonePolicy kPolicyThule;
extern const basic::ZonePolicy kPolicyTonga;
extern const basic::ZonePolicy kPolicyTunisia;
extern const basic::ZonePolicy kPolicyUS;
extern const basic::ZonePolicy kPolicyUruguay;
extern const basic::ZonePolicy kPolicyVanuatu;
extern const basic::ZonePolicy kPolicyWinn;
extern const basic::ZonePolicy kPolicyZion;


//---------------------------------------------------------------------------
// Unsupported zone policies: 74
//
// kPolicyAlbania (unused)
// kPolicyAlgeria (unused)
// kPolicyArg (unused)
// kPolicyAustria (unused)
// kPolicyBahamas (unused)
// kPolicyBelgium (unused)
// kPolicyBelize (LETTER 'CST' too long)
// kPolicyBulg (unused)
// kPolicyCA (unused)
// kPolicyChicago (unused)
// kPolicyCyprus (unused)
// kPolicyCzech (unused)
// kPolicyDR (unused)
// kPolicyDenmark (unused)
// kPolicyDenver (unused)
// kPolicyDetroit (unused)
// kPolicyE-Eur (unused)
// kPolicyE-EurAsia (unused)
// kPolicyEdm (unused)
// kPolicyEgypt (Found 2 transitions in year/month '2010-09')
// kPolicyEgyptAsia (unused)
// kPolicyFalk (unused)
// kPolicyFinland (unused)
// kPolicyFrance (unused)
// kPolicyGB-Eire (unused)
// kPolicyGermany (unused)
// kPolicyGreece (unused)
// kPolicyGuam (unused)
// kPolicyHalifax (unused)
// kPolicyHungary (unused)
// kPolicyIceland (unused)
// kPolicyIndianapolis (unused)
// kPolicyItaly (unused)
// kPolicyKyrgyz (unused)
// kPolicyLatvia (unused)
// kPolicyLibya (unused)
// kPolicyLouisville (unused)
// kPolicyLux (unused)
// kPolicyMalta (unused)
// kPolicyMarengo (unused)
// kPolicyMenominee (unused)
// kPolicyMorocco (unused)
// kPolicyNBorneo (unused)
// kPolicyNT_YK (unused)
// kPolicyNYC (unused)
// kPolicyNamibia (LETTER 'WAT' too long)
// kPolicyNeth (unused)
// kPolicyNorway (unused)
// kPolicyPalestine (Found 2 transitions in year/month '2011-08')
// kPolicyPerry (unused)
// kPolicyPike (unused)
// kPolicyPoland (unused)
// kPolicyPort (unused)
// kPolicyPulaski (unused)
// kPolicyRegina (unused)
// kPolicyRomania (unused)
// kPolicyRussia (unused)
// kPolicySanLuis (unused)
// kPolicyShang (unused)
// kPolicySovietZone (unused)
// kPolicySpain (unused)
// kPolicySpainAfrica (unused)
// kPolicyStJohns (unused)
// kPolicyStarke (unused)
// kPolicySudan (unused)
// kPolicySwift (unused)
// kPolicySwiss (unused)
// kPolicyToronto (unused)
// kPolicyTroll (unused)
// kPolicyTurkey (unused)
// kPolicyVanc (unused)
// kPolicyVincennes (unused)
// kPolicyW-Eur (unused)
// kPolicyWS (unused)


// Notable zone policies: 0
//


}
}

#endif
