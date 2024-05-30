# Controleur

    Application qui permet de contrôler une imprimante laser

    L'application affiche en permanence l'état de la connexion avec Grbl et l'état 
    de Grbl lui-même ainsi que les coordonnées X, Y et Z.

## 1. Panneau Joystick

    Panneau de contrôle manuel

        La vitesse de déplacement peut-être réglée grâce au slider vitesse
        La distance de déplacement peut-être choisie grâce aux boutons radio de distance

![ArrowUp](../img/arrow_up.png "ArrowUp")
        Déplacement Y négatif

![ArrowDown](../img/arrow_down.png "ArrowDown")
        Déplacement Y positif

![ArrowLeft](../img/arrow_left.png "ArrowLeft")
        Déplacement X négatif

![ArrowRight](../img/arrow_right.png "ArrowRight")
        Déplacement X positif

![Home](../img/home.png "Home")
        Retour à la position de Homing

![Unlock](../img/unlock.png "Unlock")
        Permet d'annuler l'alarme émise par Grbl

![Marker](../img/marker.png "Marker")
        Définit la position actuelle comme la position zéro

## 2. Panneau Fichiers

    Panneau de lecture de la carte SD et du choix d'un fichier.Un clic sur un fichier
    fera apparaitre une boite de dialogue rappelant le nom du fichier ainsi que trois choix

        Graver.     Pour graver le fichier
        Mesurer.    Pour mesurer les valeurs X et Y max. Ces valeurs seront utilisées
        Annuler.    Pour annuler
                    au niveau du panneau Outils.
        
![Marker](../img/folder.png "Marker")
        Remonte d'un niveau dans le répertoire, permet également de relire la carte SD 
        si celle-ci vient d'être insérée.

## 3. Panneau Paramètres

    Permet de lire les paramètres de configuration de Grbl

## 4. Panneau Outils

    On peut effectuer ici le choix de la langue qui sera effectif au redémmarage du controleur.

    Quelques outils permettant de préparer la gravure.

![Spot](../img/spot.png "Spot")
        Allume ou éteint le spot laser

![Square](../img/square.png "Square")
        Délimite la surface d'impression avec un spot laser

![Center](../img/center.png "Center")
        Positionne le laser au milieu de la surface d'impression

![Engraving](../img/engraving.png "Engraving")
        Démarre la gravure du fichier précédemment mesuré
        


