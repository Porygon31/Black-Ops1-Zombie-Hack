#include "Drawing.h"
#include "pch.h"

/**
 * @brief Dessine un rectangle plein aux coordonnées spécifiées.
 *
 * Construit un `RECT` GDI avec les coins gauche/haut et droit/bas, puis appelle `FillRect` avec le pinceau global.
 *
 * @param x Abscisse du coin supérieur gauche.
 * @param y Ordonnée du coin supérieur gauche.
 * @param w Largeur du rectangle en pixels (w >= 0).
 * @param h Hauteur du rectangle en pixels (h >= 0).
 *
 * @note Les limites du `RECT` sont [x, x + w) et [y, y + h) dans l’espace appareil.
 * @pre `hdcBlackops1` et `Brush` doivent être valides.
 * @see FillRect
 */
void DrawFilledRect(int x, int y, int w, int h)
{
    RECT rect = { x, y, x + w, y + h };
    FillRect(hdcBlackops1, &rect, Brush);
}

/**
 * @brief Dessine une boîte avec des bordures d’épaisseur donnée.
 *
 * Compose quatre rectangles pleins pour former un cadre rectangulaire:
 *  - Bord supérieur (largeur w, épaisseur thickness)
 *  - Bord gauche (hauteur h, épaisseur thickness)
 *  - Bord droit (hauteur h, épaisseur thickness)
 *  - Bord inférieur (largeur w + thickness, épaisseur thickness) afin de fermer le cadre
 *
 * @param x Abscisse du coin supérieur gauche du cadre.
 * @param y Ordonnée du coin supérieur gauche du cadre.
 * @param w Largeur intérieure du cadre en pixels.
 * @param h Hauteur intérieure du cadre en pixels.
 * @param thickness Épaisseur de la bordure en pixels (thickness > 0).
 *
 * @note Cette fonction s’appuie sur `DrawFilledRect` et le `Brush` courant.
 * @pre `hdcBlackops1` et `Brush` doivent être valides.
 */
void DrawBorderBox(int x, int y, int w, int h, int thickness)
{
    DrawFilledRect(x, y, w, thickness);                  // Haut
    DrawFilledRect(x, y, thickness, h);                  // Gauche
    DrawFilledRect((x + w), y, thickness, h);            // Droit
    DrawFilledRect(x, y + h, w + thickness, thickness);  // Bas
}

/**
 * @brief Trace une ligne horizontale à partir d’un point de départ.
 *
 * Utilise `MoveToEx` pour positionner le curseur GDI puis `LineTo` pour tracer jusqu’à x + width.
 *
 * @param bottomW Abscisse de départ.
 * @param bottomH Ordonnée de départ.
 * @param width Longueur de la ligne en pixels (peut être négative pour aller vers la gauche).
 *
 * @pre Un stylo (pen) approprié doit être sélectionné dans `hdcBlackops1` avant l’appel pour définir la couleur/épaisseur.
 * @see MoveToEx, LineTo
 */
void DrawLine(int bottomW, int bottomH, int width)
{
    MoveToEx(hdcBlackops1, bottomW, bottomH, NULL);
    LineTo(hdcBlackops1, bottomW + width, bottomH);
}

/**
 * @brief Dessine une chaîne de caractères aux coordonnées données avec un alignement centré.
 *
 * Configure l’alignement du texte (`TA_CENTER | TA_NOUPDATECP`), un fond transparent, la couleur du texte,
 * sélectionne la police globale `Font`, puis rend la chaîne via `TextOutA`.
 *
 * @param x Abscisse de l’origine du texte (centre horizontal du rendu avec `TA_CENTER`).
 * @param y Ordonnée de l’origine du texte (ligne de base par défaut).
 * @param color Couleur du texte (type `COLORREF`, via `RGB(r,g,b)`).
 * @param text Pointeur vers une chaîne ANSI terminée par `'\0'`.
 *
 * @warning Cette implémentation appelle `DeleteObject(Font)` après le rendu. Si `Font` est réutilisée ailleurs,
 *          sa suppression ici peut provoquer un comportement indéfini. Assurez-vous que la stratégie de
 *          gestion de `Font` (création/sélection/suppression) est cohérente à l’échelle du module.
 * @pre `hdcBlackops1` et `Font` doivent être valides. `text` ne doit pas être nul.
 * @see SetTextAlign, SetBkColor, SetBkMode, SetTextColor, SelectObject, TextOutA, DeleteObject
 */
void DrawString(int x, int y, COLORREF color, const char* text)
{
    SetTextAlign(hdcBlackops1, TA_CENTER | TA_NOUPDATECP);
    SetBkColor(hdcBlackops1, RGB(0, 0, 0));
    SetBkMode(hdcBlackops1, TRANSPARENT);
    SetTextColor(hdcBlackops1, color);
    SelectObject(hdcBlackops1, Font);
    TextOutA(hdcBlackops1, x, y, text, strlen(text));
    DeleteObject(Font);
}
