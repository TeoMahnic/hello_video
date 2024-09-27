#include "RTE_Components.h"
#include <stdint.h>
#include CMSIS_device_header
#include "video_functions.h"
#include "video_drv.h"
#include <stdio.h>
#include <string.h>

#define CAMERA_FRAME_WIDTH      560
#define CAMERA_FRAME_HEIGHT     560
#define CAMERA_FRAME_SIZE       (CAMERA_FRAME_WIDTH * CAMERA_FRAME_HEIGHT * 3)

#define DISPLAY_FRAME_WIDTH     480
#define DISPLAY_FRAME_HEIGHT    800
#define DISPLAY_FRAME_SIZE      (DISPLAY_FRAME_WIDTH * DISPLAY_FRAME_HEIGHT * 3)

#define IMAGE_WIDTH             480
#define IMAGE_HEIGHT            480
#define IMAGE_SIZE              (IMAGE_WIDTH * IMAGE_HEIGHT * 3)

static uint8_t rawImage [CAMERA_FRAME_SIZE] __attribute__((aligned(32), section(".bss.camera_frame_buf")));
static uint8_t outImage [DISPLAY_FRAME_SIZE] __attribute__((aligned(32), section(".bss.lcd_frame_buf")));
static uint8_t rgbImage [IMAGE_SIZE] __attribute__((aligned(32), section(".bss.camera_frame_bayer_to_rgb_buf")));


int app_main() {
    int32_t ret = 0U;
    void *imgFrame;
    void *outFrame;
    void *imgBuf = rgbImage;

    if (VideoDrv_Initialize(NULL) != VIDEO_DRV_OK) {
        // printf("Failed to initialise video driver\n");
        return 1;
    }

    /* Configure Input Video */
    if (VideoDrv_Configure(VIDEO_DRV_IN0, CAMERA_FRAME_WIDTH, CAMERA_FRAME_HEIGHT, VIDEO_DRV_COLOR_RGB888, 60U) != VIDEO_DRV_OK) {
        // printf("Failed to configure video input\n");
        return 1;
    }

    /* Configure Output Video */
    if (VideoDrv_Configure(VIDEO_DRV_OUT0, DISPLAY_FRAME_WIDTH, DISPLAY_FRAME_HEIGHT, VIDEO_DRV_COLOR_RGB888, 60U) != VIDEO_DRV_OK) {
        // printf("Failed to configure video input\n");
        return 1;
    }

    /* Set Input Video buffer */
    if (VideoDrv_SetBuf(VIDEO_DRV_IN0, rawImage, CAMERA_FRAME_SIZE) != VIDEO_DRV_OK) {
        // printf("Failed to set buffer for video input\n");
        return 1;
    }

    /* Set Output Video buffer */
    if (VideoDrv_SetBuf(VIDEO_DRV_OUT0, outImage, DISPLAY_FRAME_SIZE) != VIDEO_DRV_OK) {
        // printf("Failed to set buffer for video input\n");
        return 1;
    }

    while (true) {
        VideoDrv_Status_t status;

        /* Start video capture (single frame) */
        if (VideoDrv_StreamStart(VIDEO_DRV_IN0, VIDEO_DRV_MODE_SINGLE) != VIDEO_DRV_OK) {
            // printf("Failed to start video capture\n");
            return 1;
        }

        /* Wait for video input frame */
        do {
            status = VideoDrv_GetStatus(VIDEO_DRV_IN0);
        } while (status.buf_empty != 0U);

        /* Get input video frame buffer */
        imgFrame = VideoDrv_GetFrameBuf(VIDEO_DRV_IN0);

        int debayerState = CropAndDebayer(
                            imgFrame,
                            CAMERA_FRAME_WIDTH,
                            CAMERA_FRAME_HEIGHT,
                            (CAMERA_FRAME_WIDTH - IMAGE_WIDTH)/2,
                            (CAMERA_FRAME_HEIGHT - IMAGE_HEIGHT)/2,
                            imgBuf,
                            IMAGE_WIDTH,
                            IMAGE_HEIGHT,
                            GRBG);

        if (debayerState != 0) {
            // printf("Debayering failed\n");
            return 1;
        }

        /* Get output video frame buffer */
        outFrame = VideoDrv_GetFrameBuf(VIDEO_DRV_OUT0);

        /* Copy image frame with detection boxes to output frame buffer */
        memcpy(outFrame, imgBuf, IMAGE_SIZE);

        /* Release input frame */
        ret = VideoDrv_ReleaseFrame(VIDEO_DRV_IN0);

        /* Release output frame */
        ret = VideoDrv_ReleaseFrame(VIDEO_DRV_OUT0);

        /* Start video output (single frame) */
        ret = VideoDrv_StreamStart(VIDEO_DRV_OUT0, VIDEO_DRV_MODE_CONTINUOS);

        /* Check for end of stream (when using AVH with file as Video input) */
        if (status.eos != 0U) {
            while (VideoDrv_GetStatus(VIDEO_DRV_OUT0).buf_empty == 0U);
            break;
        }
    }

    /* De-initialize Video Interface */
    VideoDrv_Uninitialize();

    while(1) {
    }
}
