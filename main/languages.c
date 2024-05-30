// languages.c
//
#include "string.h"

#include "languages.h"

char strJoystick[9];
char strFiles[9];
char strParameters[12];
char strTools[7];
char strGrblConnected[15];
char strGrblDisconnected[18];
char strSpeed[8];
char strDistance[9];
char strRead[5];
char strOk[3];
char strError[7];
char strFilenameTooLong[25];
char strNoSDReader[18];
char strNoSDCard[16];
char strFileReadError[26];
char strEngraving[17];
char strBuffer[7];
char strCancel[8];
char strEngrave[8];
char strMeasure[8];
char strSize[7];

char strAlarms[10][40];
char strErrors[40][40];

void languageInit(uint16_t language)
{
    if (language == 1)
    {
        strcpy(strJoystick, "Joystick");
        strcpy(strFiles, "Files");
        strcpy(strParameters, "Parameters");
        strcpy(strTools, "Tools");
        strcpy(strGrblConnected, "GRBL connected");
        strcpy(strGrblDisconnected, "GRBL disconnected");
        strcpy(strSpeed, "Speed");
        strcpy(strDistance, "Distance");
        strcpy(strRead, "Read");
        strcpy(strOk, "Ok");
        strcpy(strError, "Error");
        strcpy(strFilenameTooLong, "Filename too long");
        strcpy(strNoSDReader, "No SD Reader");
        strcpy(strNoSDCard, "No SD Card");
        strcpy(strFileReadError, "File read error");
        strcpy(strEngraving, "Engraving");
        strcpy(strBuffer, "Buffer");
        strcpy(strCancel, "Cancel");
        strcpy(strEngrave, "Engrave");
        strcpy(strMeasure, "Measure");
        strcpy(strSize, "Size");

        strcpy(strAlarms[0], "Unknown Alarm");
        strcpy(strAlarms[1], "A1.Hard limit reached(P?)");
        strcpy(strAlarms[2], "A2.Motion exceed CNC");
        strcpy(strAlarms[3], "A3.Reset in motion(P?)");
        strcpy(strAlarms[4], "A4.Probe init fail");
        strcpy(strAlarms[5], "A5.Probe travel fail");
        strcpy(strAlarms[6], "A6.Reset during homing");
        strcpy(strAlarms[7], "A7.Door open (homing)");
        strcpy(strAlarms[8], "A8.Limit ON(homing)");
        strcpy(strAlarms[9], "A9.Limit missing(homing)");

        strcpy(strErrors[0], "0.Unknown error");
        strcpy(strErrors[1], "1.Expected cmd letter");
        strcpy(strErrors[2], "2.Bad number format");
        strcpy(strErrors[3], "3.Invalid $ sytem cmd");
        strcpy(strErrors[4], "4.Negative value");
        strcpy(strErrors[5], "5.Homing not enabled");
        strcpy(strErrors[6], "6.Step pulse <3 usec");
        strcpy(strErrors[7], "7.EEPROM read fail");
        strcpy(strErrors[8], "8. $ while not IDLE");
        strcpy(strErrors[9], "9.Locked(alarm or jog)");
        strcpy(strErrors[10], "10.Soft limit(no Homing)");
        strcpy(strErrors[11], "11.Line overflow");
        strcpy(strErrors[12], "12.Step rate to high");
        strcpy(strErrors[13], "13.Safety door detected");
        strcpy(strErrors[14], "14.Line length exceeded");
        strcpy(strErrors[15], "15.Jog travel exceeded");
        strcpy(strErrors[16], "16.Invalid jog command");
        strcpy(strErrors[17], "17.Laser requires PWM");
        strcpy(strErrors[18], "18.No Homing/Cycle defined");
        strcpy(strErrors[19], "19.Unknown error");
        strcpy(strErrors[20], "20.Unsupported command");
        strcpy(strErrors[21], "21.Modal group violation");
        strcpy(strErrors[22], "22.Undef. feed rate");
        strcpy(strErrors[23], "23.Cmd requires integer");
        strcpy(strErrors[24], "24.Several axis Gcode");
        strcpy(strErrors[25], "25.Repeated Gcode");
        strcpy(strErrors[26], "26.Axis missing in Gcode");
        strcpy(strErrors[27], "27.Invalid line number");
        strcpy(strErrors[28], "28.Value missing in Gcode");
        strcpy(strErrors[29], "29.G59 WCS not supported");
        strcpy(strErrors[30], "30.G53 without G0 and G1");
        strcpy(strErrors[31], "31.Axis not allowed");
        strcpy(strErrors[32], "32.G2,G3 require a plane");
        strcpy(strErrors[33], "33.Invalid motion target");
        strcpy(strErrors[34], "34.Invalid arc radius");
        strcpy(strErrors[35], "35.G2,G3 require offset");
        strcpy(strErrors[36], "36.Unused value");
        strcpy(strErrors[37], "37.G43.1 tool length");
        strcpy(strErrors[38], "38.Tool number > max");
        strcpy(strErrors[39], "39.Parameter P > max");
    }
    else
    {
        strcpy(strJoystick, "Joystick");
        strcpy(strFiles, "Fichiers");
        strcpy(strParameters, "Paramètres");
        strcpy(strTools, "Outils");
        strcpy(strGrblConnected, "GRBL connecté");
        strcpy(strGrblDisconnected, "GRBL déconnecté");
        strcpy(strSpeed, "Vitesse");
        strcpy(strDistance, "Distance");
        strcpy(strRead, "Lire");
        strcpy(strOk, "Ok");
        strcpy(strError, "Erreur");
        strcpy(strFilenameTooLong, "Nom de fichier trop long");
        strcpy(strNoSDReader, "Pas de lecteur SD");
        strcpy(strNoSDCard, "Pas de carte SD");
        strcpy(strFileReadError, "Erreur de lecture fichier");
        strcpy(strEngraving, "Gravure en cours");
        strcpy(strBuffer, "Buffer");
        strcpy(strCancel, "Annuler");
        strcpy(strEngrave, "Graver");
        strcpy(strMeasure, "Mesurer");
        strcpy(strSize, "Taille");

        strcpy(strAlarms[0], "Alarme inconnue");
        strcpy(strAlarms[1], "A1.Fin de course atteinte");
        strcpy(strAlarms[2], "A2.Le mouvement dépasse la CNC");
        strcpy(strAlarms[3], "A3.Reset lors d'un mouvement");
        strcpy(strAlarms[4], "A4.Erreur initiation Probe");
        strcpy(strAlarms[5], "A5.Erreur lors du trajet de la Probe");
        strcpy(strAlarms[6], "A6.Reset durant le homing");
        strcpy(strAlarms[7], "A7.Porte ouverte (homing)");
        strcpy(strAlarms[8], "A8.Fin de course durant homing");
        strcpy(strAlarms[9], "A9.Pas de fin de course(homing)");

        strcpy(strErrors[0], "Erreur inconnue");
        strcpy(strErrors[1], "1.Lettre Cmd attendue");
        strcpy(strErrors[2], "2.Erreur de nombre (format)");
        strcpy(strErrors[3], "3.Cmd $ non valide");
        strcpy(strErrors[4], "4.Valeur négative");
        strcpy(strErrors[5], "5.Homing non actif");
        strcpy(strErrors[6], "6.Impulsion Step <3 usec");
        strcpy(strErrors[7], "7.Echec lecture EEPROM");
        strcpy(strErrors[8], "8.$ alors que non Idle");
        strcpy(strErrors[9], "9.Verrouillé (alarme ou jog)");
        strcpy(strErrors[10], "10.Soft limit sans homing");
        strcpy(strErrors[11], "11.Débordement de ligne");
        strcpy(strErrors[12], "12.Step rate trop élevé");
        strcpy(strErrors[13], "13.Sécurite porte detectée");
        strcpy(strErrors[14], "14.Longueur de ligne depassé");
        strcpy(strErrors[15], "15.Jog trop long");
        strcpy(strErrors[16], "16.Commande de Jog non valide");
        strcpy(strErrors[17], "17.Laser requiert PWM");
        strcpy(strErrors[18], "18.Pas de Homing/Cycle défini");
        strcpy(strErrors[19], "19.Erreur inconnue");
        strcpy(strErrors[20], "20.Commande non supportée");
        strcpy(strErrors[21], "21.Violation du groupe modal");
        strcpy(strErrors[22], "22.Feed rate non defini");
        strcpy(strErrors[23], "23.La Cmd exige un nbr entier");
        strcpy(strErrors[24], "24.Plusieurs axes Gcode");
        strcpy(strErrors[25], "25.Gcode répété");
        strcpy(strErrors[26], "26.Axe manquant dans Gcode");
        strcpy(strErrors[27], "27.Numéro de ligne non valide");
        strcpy(strErrors[28], "28.Valeur manquante dans Gcode");
        strcpy(strErrors[29], "29.G59 WCS non supporté");
        strcpy(strErrors[30], "30.G53 sans G0 et G1");
        strcpy(strErrors[31], "31.Axe non autorisée");
        strcpy(strErrors[32], "32.G2);G3 requiert un plan");
        strcpy(strErrors[33], "33.Cible du mouvement non valide");
        strcpy(strErrors[34], "34.Rayon d'arc non valide");
        strcpy(strErrors[35], "35.G2);G3 requiert un offset");
        strcpy(strErrors[36], "36.Valeur inutilisée");
        strcpy(strErrors[37], "37.G43.1 longueur d'outil");
        strcpy(strErrors[38], "38.Numero d'outil > max");
        strcpy(strErrors[39], "39.Parametre P > max");
    }
}