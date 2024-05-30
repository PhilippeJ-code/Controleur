# Controleur

    Controleur GRBL. L'application tourne sur une Waveshare ESP32-S3-Touch-LCD-4.3.

        L'ESP32-S3 dispose d'un port USB OTG configuré en Host


## Modifications à effectuer

    L'initialisation du composant GT911 pose problème. La solution est de modifier esp_lcd_touch_gt911.c et de commenter les lignes suivantes

    ```
    /* Read status and config info */
    //ret = touch_gt911_read_cfg(esp_lcd_touch_gt911);
    //ESP_GOTO_ON_ERROR(ret, err, TAG, "GT911 init failed");

    ```

## Box

    Le répertoire box contient les fichiers stl permettant d'imprimer un boitier pour la carte waveshare, le fichier openscad est inclus

