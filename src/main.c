#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define W 1920
#define H 1080
#define FPS 60
#define TOTAL_FRAMES (FPS * 5)

int main(void)
{
    InitWindow(W, H, "pipe record");

    RenderTexture2D fbo = LoadRenderTexture(W, H);

    // ─── 启动 ffmpeg 子进程（读 stdin 的 rawvideo）───
    // 用 fork+exec 或 popen；这里示意 popen 最简写法：
    // ⚠️ 注意：popen 不能设 stderr 分离，生产环境建议 fork+exec
    char cmd[512];
    snprintf(cmd, sizeof(cmd),
        "ffmpeg -y "
        "-f rawvideo -pix_fmt rgba -s %dx%d -r %d -i - "
        "-c:v libx264 -preset medium -crf 17 -pix_fmt yuv420p "
        "-vf \"scale=trunc(iw/2)*2:trunc(ih/2)*2\" "
        "output.mp4",
        W, H, FPS
    );

    FILE* ffmpeg = popen(cmd, "w");
    if (!ffmpeg) { puts("ffmpeg spawn failed!"); return 1; }

    // raylib 内部有 rlgl 层，我们用 LoadImageFromTexture 读回
    // 它返回的格式是 UNCOMPRESSED_R8G8B8A8 = 4 字节 RGBA
    for (int frame = 0; frame < TOTAL_FRAMES; frame++)
    {
        float t = (float)frame / FPS;

        BeginTextureMode(fbo);
            ClearBackground((Color){30,30,40,255});
            DrawCircleV(
                (Vector2){ W/2 + 300*cosf(t*2), H/2 + 200*sinf(t*3) },
                80, RED
            );
            DrawText("Pipe Mode 🚀", 80, 80, 48, YELLOW);
        EndTextureMode();

        // GPU → CPU
        Image img = LoadImageFromTexture(fbo.texture);
        ImageFlipVertical(&img);  // OpenGL 左下原点 → 正常左上

        // img.data 就是 RGBA 裸像素，size = W*H*4 字节
        fwrite(img.data, 1, W * H * 4, ffmpeg);
        fflush(ffmpeg);

        UnloadImage(img);
        printf("Frame %d\r", frame);
    }

    pclose(ffmpeg);  // ffmpeg 收到 EOF → 完成写入
    UnloadRenderTexture(fbo);
    CloseWindow();
    puts("\nDone → output.mp4");
}
