#include <Windows.h>
#include "Drawing.h"
#include "Vectors.h"
#include "WorldToScreen.h"
#include "Offsets.h"
#include "pch.h"

// Variables globales
HWND hwndBlackops1 = nullptr;
HBRUSH Brush = nullptr;
HDC hdcBlackops1 = nullptr;
HFONT Font = nullptr;
float Matrix[16];
COLORREF TextCOLOR;

// Offset
DWORD entityList = 0x01BCE4D0;
DWORD viewMatrix = 0x00B81060;
DWORD healthOffset = 0x184;

/**
 * @brief Thread principal de l'ESP. Affiche les informations des entités à l'écran.
 * 
 * Ce thread lit la mémoire du jeu pour obtenir la liste des entités, leur position, leur santé,
 * puis dessine une boîte et la santé de chaque entité vivante à l'écran.
 * 
 * @param lpParam Paramètre non utilisé.
 * @return DWORD Toujours 0 à la fin du thread.
 */
DWORD WINAPI mainThread(LPVOID lpParam)
{
    // Couleur du texte par défaut (blanc)
    TextCOLOR = RGB(255, 255, 255);
    // Récupère le handle de la fenêtre du jeu
    hwndBlackops1 = FindWindow(0, "Call of Duty®: BlackOps");

    DWORD injectTime = GetTickCount();

    while (true)
    {
        // Récupère le contexte de périphérique pour dessiner sur la fenêtre du jeu
        hdcBlackops1 = GetDC(hwndBlackops1);

        // Affiche le message uniquement pendant 2 centième secondes après l'injection
        if (GetTickCount() - injectTime < 200)
        {
            DrawString(100, 20, RGB(255, 0, 0), "DLL Injectee !");
        }

        // Affiche l'offset viewMatrix actuel
        //DrawString(200, 40, RGB(255, 255, 255), "Offset de viewMatrix actuelle : ");
        //char buffer[64];
        //sprintf_s(buffer, sizeof(buffer), "0x%08X", viewMatrix);
        //DrawString(510, 40, RGB(66, 244, 158), buffer);

        memcpy(&Matrix, (PBYTE*)(viewMatrix), sizeof(Matrix));

        for (short int i = 0; i < 32; i++)
        {
            DWORD entity = *(DWORD*)(entityList + i * 0x8C);

            if (entity != NULL)
            {
                DWORD health = *(DWORD*)(entity + healthOffset);
                if (!health <= 0) {
                    // Lit la position de l'entité (coordonnées monde)
                    float enemyX = *(float*)(entity + 0x18);
                    float enemyY = *(float*)(entity + 0x1C);

                    float enemyZ = *(float*)(entity + 0x20);

                    Vec3 entityLocation = { enemyX, enemyY, enemyZ };
                    // Lit la santé de l'entité

                    DWORD health = *(DWORD*)(entity + healthOffset);

                    //// Correct the problematic line by converting the DWORD entity to a string representation before passing it to DrawString.
                    //char entityBuffer[64];
                    //sprintf_s(entityBuffer, sizeof(entityBuffer), "Entity that are !NULL: 0x%08X", entity);
                    //DrawString(100, 20, RGB(255, 0, 0), entityBuffer);

                    /*if (health > 0)
                    {*/
                    Vec2 vScreen, vHead;
                    // Convertit la position monde en position écran
                    if (WorldToScreen(entityLocation, vScreen, Matrix, 1024, 768))
                    {
                        // Lit la position de la tête de l'entité
                        float enemyHeadX = *(float*)(entity + 0x110);
                        float enemyHeadY = *(float*)(entity + 0x114);
                        float enemyHeadZ = *(float*)(entity + 0x118);
                        Vec3 enemyHeadPos = { enemyHeadX, enemyHeadY, enemyHeadZ };

                        // Convertit la position de la tête en position écran
                        if (WorldToScreen(enemyHeadPos, vHead, Matrix, 1024, 768))
                        {
                            float head = vHead.y - vScreen.y;
                            float width = head / 2;
                            float center = width / -2;
                            if (health > 0) {

                                // Crée un pinceau pour dessiner la boîte
                                Brush = CreateSolidBrush(RGB(158, 66, 244));
                                // Dessine la boîte autour de l'entité
                                DrawBorderBox(vScreen.x + center, vScreen.y, width, head - 5, 1);

                                // Affiche la santé de l'entité
                                char healthChar[255];
                                sprintf_s(healthChar, sizeof(healthChar), "Health: %i", (int)(health));
                                DrawString(vScreen.x, vScreen.y, TextCOLOR, healthChar);

                                // Libère le pinceau
                                DeleteObject(Brush);
                            }

                        }
                    }
                    //}

                }
            }
        }
        // Petite pause pour éviter de surcharger le CPU
        Sleep(1);
        // Libère le contexte de périphérique
        DeleteObject(hdcBlackops1);
    }
    return 0;
}