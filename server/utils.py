import cv2
import numpy as np

def resize(img, max_width, max_height):
    '''Resize image so it fits within our '''
    ratio = float(img.shape[1]) / float(img.shape[0])
    ratio_resize = float(max_width) / float(max_height)
    if ratio < ratio_resize:
        # we can scale to max height
        # calculate the target width
        target_width = int(max_height * ratio)
        if target_width % 2 == 1:
            target_width = target_width + 1
        img = cv2.resize(img, (target_width, max_height))
    else:
        # we can do max height
        target_height = int(max_width / ratio)
        if target_height % 2 == 1:
            target_height = target_height + 1
        img = cv2.resize(img, (max_width, target_height))
    # diffH = int((self.character_size[0] - img.shape[0]) / 2)
    # diffW = int((self.character_size[1] - img.shape[1]) / 2)
    # img = cv2.copyMakeBorder(
    #     img, diffH, diffH, diffW, diffW, cv2.BORDER_CONSTANT, value=255)
    return img

def prepare_image(img):
    # LilyGo T5 4.7 has a resolution of 960x540
    # resize the image to fit on the screen
    HOR_RES, VERT_RES = 960, 540
    img2 = resize(img, HOR_RES, VERT_RES)
    # cv2.imshow('img', img2)
    # cv2.waitKey(0)
    # map the image to one contigous array
    # default dtype is uint8 -> 8 bits per integer (0-255)
    img3 = img2.reshape(-1)
    # we need to go to 4 bit integers since that is wat the e-ink display supports
    # remap in range 0 - 15
    img3 = (img3 / 16).astype(np.uint8)
    # remap every value below 250 to 0 (black) and everything above to 15 (white)
    # img3[img3 <= 250] = 0
    # img3[img3 > 250] = 15
    # pack two pixels per byte
    img4 = np.empty((img3.shape[0] // 2,), np.uint8)
    for i in range(0, img3.shape[0], 2):
        img4[i // 2] = img3[i+1] << 4 | img3[i]
    return img4, img2.shape[1], img2.shape[0]