#include "../components.h"
#include "../internal.h"
#include "../ui.h"

FrameTimer logoFadeAnimation;
FrameTimer toGameAnim;

Shader logoShader;
u32 shaderColors[] = {PALETTE_GRAY,
                      PALETTE_DARK_GRAY,
                      PALETTE_DARK_BROWN,
                      BG_COLOR};

Sprite* slideSprites;

void InitLogoAnimation()
{
    float frameTime = 0.15;

    logoFadeAnimation.frame_count = sizeof(shaderColors) / sizeof(u32);
    logoFadeAnimation.time_to_start = 2;
    logoFadeAnimation.time_after_end = 0.75;
    logoFadeAnimation.finished = false;
    logoFadeAnimation.time_scale = 1.5;

    logoFadeAnimation.frames = new Keyframe[logoFadeAnimation.frame_count];

    for (int i = 0; i < logoFadeAnimation.frame_count; i++)
    {
        logoFadeAnimation.frames[i].index = i;
        logoFadeAnimation.frames[i].time_per_frame = frameTime;
    }
}

// TODO: this doesn't look quite right yet
//  but i guess it's ok for now
void InitTransition_ToGame()
{
    float frameTime = 0.1;

    Sprite slideFrames[] = {Sprite{1, 1, 1, &Res.bitmaps.effects},
                            Sprite{1, 1, 2, &Res.bitmaps.effects},
                            Sprite{1, 1, 0, &Res.bitmaps.effects}};

    toGameAnim.frame_count = Game.tile_map.columns * 3;
    toGameAnim.finished = false;
    toGameAnim.frames = new Keyframe[toGameAnim.frame_count];
    toGameAnim.time_scale = 3;

    slideSprites = new Sprite[toGameAnim.frame_count];

    for (int i = 0; i < toGameAnim.frame_count; i++)
    {
        toGameAnim.frames[i].index = i;
        toGameAnim.frames[i].time_per_frame = frameTime;

        if (i % 3 == 2)
        {
            // HACK: to not have to draw over the previous row again
            // this doesn't look perfectly, but i guess its good enough for now?
            toGameAnim.frames[i].time_per_frame = 0;
        }

        slideSprites[i] = slideFrames[i % 3];
    }
}

void InitScenes()
{
    InitLogoAnimation();
    InitTransition_ToGame();
}

void DrawToGameTransition(ScreenBuffer buffer)
{
    int keyframeIndex = NextKeyframeIndex(&toGameAnim);
    if (toGameAnim.frame_index == -1)
    {
        // finished -> start game
        Action_StartGame();
    }
    else
    {
        // Draw on the column index of the current one right?
        for (int i = 0; i < Game.tile_map.rows; i++)
        {
            // this has to be synchronized with the amount of frames
            // we want to draw 3 frames for each x position
            int xPos = keyframeIndex / 3 * Game.tile_size.width;
            v2 pos = {xPos, i * Game.tile_size.height};
            DrawSprite(buffer, pos, slideSprites[keyframeIndex]);
        }
    }
}

void ShowLogoScreen(ScreenBuffer buffer)
{
    int centerX = Scale.render_dim.width / 2 - Res.bitmaps.logo.width / 2;
    int centerY = Scale.render_dim.height / 2 - Res.bitmaps.logo.height / 2;

    int keyframeIndex = NextKeyframeIndex(&logoFadeAnimation);

    if (keyframeIndex == -1)
    {
        if (logoFadeAnimation.finished) Action_GoToMenu();
    }
    else
    {
        logoShader.type = COLOR_REPLACE;
        logoShader.shader_color = shaderColors[keyframeIndex];
    }

    DrawBitmap(buffer, Res.bitmaps.logo, centerX, centerY, logoShader);
}
