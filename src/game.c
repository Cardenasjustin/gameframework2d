#include <SDL.h>
#include <stdio.h>
#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"

#include "camera.h"
#include "entity.h"
#include "monster.h"
#include "tankmonster.h"
#include "huntermonster.h"
#include "spittermonster.h"
#include "witchmonster.h"
#include "player.h"
#include "world.h"
#include "projectile.h"
#include "assaultrifle_pickup.h"
#include "shotgun_pickup.h"
#include "pistol_pickup.h"
#include "deagle_pickup.h"
#include "smg_pickup.h"
#include "barrel.h"
#include "medkit.h"
#include "money.h"

int main(int argc, char * argv[])
{
    int done = 0;
    const Uint8 * keys;
    World *world;
    
    int mx,my;
    Sprite *mouse;

    Entity* player;
    Entity* assaultRiflePickup;
    Entity* shotgunPickup;
    Entity* pistolPickup;
    Entity* deaglePickup;
    Entity* smgPickup;
    Entity* barrel1;
    Entity* medkit1;
    Entity* money1;
    Entity* money2;

    Sprite* uiAssaultRifle;
    Sprite* uiShotgun;
    Sprite* uiPistol;
    Sprite* uiDeagle;
    Sprite* uiSmg;
    Sprite* uiMoney;
    Sprite* uiDigits;

    int spawnCooldown = 0;
    int spawnOffset = 200;
    
    init_logger("gf2d.log",0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        800,
        608,
        800,
        608,
        gfc_vector4d(0,0,0,255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    entity_system_initialize(1024);
    SDL_ShowCursor(SDL_DISABLE);
    camera_set_size(gfc_vector2d(800,608));
    
    /*demo setup*/
    GFC_Vector2D mouseCenter = { 16.0f, 10.5f };
    mouse = gf2d_sprite_load_all("images/reticle.png", 32, 21, 1, 0);
    uiAssaultRifle = gf2d_sprite_load_all("images/AKUI.png", 600, 600, 1, 0);
    uiShotgun = gf2d_sprite_load_all("images/shotgunUI.png", 600, 600, 1, 0);
    uiPistol = gf2d_sprite_load_all("images/pistolUI.png", 600, 600, 1, 0);
    uiDeagle = gf2d_sprite_load_all("images/DeagleUI.png", 600, 600, 1, 0);
    uiSmg = gf2d_sprite_load_all("images/SMGUI.png", 600, 600, 1, 0);
    uiMoney = gf2d_sprite_load_all("images/Money.png", 500, 500, 1, 0);  
    uiDigits = gf2d_sprite_load_all("images/digits.png", 32, 32, 10, 0);
    slog("press [escape] to quit");
    player = player_new();

    assaultRiflePickup = assaultrifle_pickup_new(player, gfc_vector2d(300, 200));
    shotgunPickup = shotgun_pickup_new(player, gfc_vector2d(450, 200));
    pistolPickup = pistol_pickup_new(player, gfc_vector2d(600, 200));
    deaglePickup = deagle_pickup_new(player, gfc_vector2d(750, 200));
    smgPickup = smg_pickup_new(player, gfc_vector2d(150, 200));
    barrel1 = barrel_new(gfc_vector2d(500, 350));
    medkit1 = medkit_new(player, gfc_vector2d(350, 350));
    money1 = money_new(player, gfc_vector2d(250, 350), 25);
    money2 = money_new(player, gfc_vector2d(650, 350), 50);

    world = world_load("maps/testworld.map");
    world_setup_camera(world);
    //monster_new(player, gfc_vector2d(400, 300));
    //tankmonster_new(player, gfc_vector2d(600, 300));
    //huntermonster_new(player, gfc_vector2d(750, 200));
    //spittermonster_new(player, gfc_vector2d(800, 150));
    //witchmonster_new(player, gfc_vector2d(500, 250));
    /*main game loop*/
    while(!done)
    {
        SDL_PumpEvents();
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx,&my);
        if (spawnCooldown > 0)
        {
            spawnCooldown--;
        }
        if (spawnCooldown == 0)
        {
            GFC_Vector2D spawnPos;

            /* spawn slightly to the right of the player */
            spawnPos = gfc_vector2d(player->position.x + spawnOffset, player->position.y);

            if (keys[SDL_SCANCODE_1])
            {
                monster_new(player, spawnPos);
                slog("Spawned regular zombie");
                spawnCooldown = 12;
            }
            else if (keys[SDL_SCANCODE_2])
            {
                tankmonster_new(player, spawnPos);
                slog("Spawned tank zombie");
                spawnCooldown = 12;
            }
            else if (keys[SDL_SCANCODE_3])
            {
                huntermonster_new(player, spawnPos);
                slog("Spawned hunter zombie");
                spawnCooldown = 12;
            }
            else if (keys[SDL_SCANCODE_4])
            {
                spittermonster_new(player, spawnPos);
                slog("Spawned spitter zombie");
                spawnCooldown = 12;
            }
            else if (keys[SDL_SCANCODE_5])
            {
                witchmonster_new(player, spawnPos);
                slog("Spawned witch zombie");
                spawnCooldown = 12;
            }
        }
        entity_system_think();
        entity_system_update();
        
        gf2d_graphics_clear_screen();// clears drawing buffers
        // all drawing should happen betweem clear_screen and next_frame
            //backgrounds drawn first
            world_draw(world);

            entity_system_draw();

            {
                PlayerWeaponType currentWeapon;
                Sprite* currentWeaponSprite = NULL;

                GFC_Vector2D uiPos = { 80.0f, 60.0f };

                GFC_Vector2D uiCenter = { 300.0f, 300.0f };

                GFC_Vector2D uiScale = { 0.22f, 0.22f };

                float uiRotation = 180.0f;

                currentWeapon = player_get_weapon(player);

                if (currentWeapon == PLAYER_WEAPON_ASSAULT_RIFLE)
                {
                    currentWeaponSprite = uiAssaultRifle;
                }
                else if (currentWeapon == PLAYER_WEAPON_SHOTGUN)
                {
                    currentWeaponSprite = uiShotgun;
                }
                else if (currentWeapon == PLAYER_WEAPON_PISTOL)
                {
                    currentWeaponSprite = uiPistol;
                }
                else if (currentWeapon == PLAYER_WEAPON_DEAGLE)
                {
                    currentWeaponSprite = uiDeagle;
                }
                else if (currentWeapon == PLAYER_WEAPON_SMG)
                {
                    currentWeaponSprite = uiSmg;
                }

                if (currentWeaponSprite)
                {
                    gf2d_sprite_draw(
                        currentWeaponSprite,
                        uiPos,
                        NULL,
                        &uiCenter,
                        &uiRotation,
                        &uiScale,
                        NULL,
                        0
                    );
                }
            }
            {
                GFC_Vector2D moneyPos = { 55.0f, 525.0f };
                GFC_Vector2D moneyCenter = { 250.0f, 250.0f };
                GFC_Vector2D moneyScale = { 0.10f, 0.10f };

                gf2d_sprite_draw(
                    uiMoney,
                    moneyPos,
                    NULL,
                    &moneyCenter,
                    NULL,
                    &moneyScale,
                    NULL,
                    0
                );
            }
            {
                char moneyText[32];
                int money = player_get_money(player);
                int j;

                GFC_Vector2D moneyPos = { 52.0f, 522.0f };
                GFC_Vector2D moneyCenter = { 250.0f, 250.0f };
                GFC_Vector2D moneyScale = { 0.10f, 0.10f };
                float moneyRotation = 180.0f;

                gf2d_sprite_draw(
                    uiMoney,
                    moneyPos,
                    NULL,
                    &moneyCenter,
                    &moneyRotation,
                    &moneyScale,
                    NULL,
                    0
                );

                sprintf(moneyText, "%d", money);

                for (j = 0; moneyText[j] != '\0'; j++)
                {
                    int digit = moneyText[j] - '0';

                    GFC_Vector2D digitPos = {
                        85.0f + (j * 18.0f),
                        522.0f
                    };

                    GFC_Vector2D digitCenter = { 16.0f, 16.0f };
                    GFC_Vector2D digitScale = { 0.8f, 0.8f };

                    if (digit >= 0 && digit <= 9)
                    {
                        gf2d_sprite_draw(
                            uiDigits,
                            digitPos,
                            NULL,
                            &digitCenter,
                            &moneyRotation,
                            &digitScale,
                            NULL,
                            digit
                        );
                    }
                }
            }
            {
                int health = player_get_health(player);
                int maxHealth = player_get_max_health(player);

                float healthPercent = (float)health / (float)maxHealth;

                GFC_Vector2D barPos = { 40.0f, 560.0f };

                float barWidth = 200.0f;
                float barHeight = 20.0f;

                gf2d_draw_rect(
                    gfc_rect(barPos.x - 2, barPos.y - 2, barWidth + 4, barHeight + 4),
                    gfc_color(0, 0, 0, 255)
                );

                gf2d_draw_rect_filled(
                    gfc_rect(barPos.x, barPos.y, barWidth, barHeight),
                    gfc_color(100, 0, 0, 255)
                );

                gf2d_draw_rect_filled(
                    gfc_rect(barPos.x, barPos.y, barWidth * healthPercent, barHeight),
                    gfc_color(0, 200, 0, 255)
                );
            }

            gf2d_sprite_draw(
                mouse,
                gfc_vector2d(mx, my),
                NULL,
                &mouseCenter,
                NULL,
                NULL,
                NULL,
                0);

        gf2d_graphics_next_frame();// render current draw frame and skip to the next frame
        
        if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
        //slog("Rendering at %f FPS",gf2d_graphics_get_frames_per_second());
    }
    entity_free(player);
    world_free(world);
    slog("---==== END ====---");
    return 0;
}
/*eol@eof*/
