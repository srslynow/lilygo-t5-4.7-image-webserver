import numpy as np
import cv2
from flask import Flask, Response

from utils import prepare_image

img_path = './server/teemo.jpg'

# read logo from disk
logo_img_orig = cv2.imread(img_path, cv2.IMREAD_GRAYSCALE)
logo_img, logo_img_width, logo_img_height = prepare_image(logo_img_orig)

# Create a white image
custom_img_orig = np.full((540,960), 255, np.uint8)
# Write some Text
font                   = cv2.FONT_HERSHEY_SIMPLEX
bottomLeftCornerOfText = (250,270)
fontScale              = 1
fontColor              = (0,0,0)
lineType               = 2
cv2.putText(
    custom_img_orig,
    'Hello World from my server!', 
    bottomLeftCornerOfText, 
    font, 
    fontScale,
    fontColor,
    2,
    cv2.LINE_AA
)
custom_img, custom_img_width, custom_img_height = prepare_image(custom_img_orig)


# create flask application
app = Flask(__name__)
i = 0
# define flask application routes

@app.route("/")
def index():
    return b'Hello World!'

@app.route("/image")
def image():
    global i
    if i % 2 == 0:
        response = Response(
            response=logo_img.tobytes(),
            headers={
                'Image-Width': logo_img_width,
                'Image-Height': logo_img_height
            }
        )
    else:
        response = Response(
            response=custom_img.tobytes(),
            headers={
                'Image-Width': custom_img_width,
                'Image-Height': custom_img_height
            }
        )
    i += 1
    return response

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=12345)
