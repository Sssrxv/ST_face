#! /usr/bin/env python
# -*- coding: utf-8 -*-

import os
import cv2

cwd = f'{os.path.dirname(__file__)}'

def img2rgba(img_name):
    img = cv2.imread(os.path.join(cwd, img_name+'.png'))
    img = cv2.cvtColor(img, cv2.COLOR_BGR2RGBA)
    img.tofile(f'{os.path.join(cwd, f"{img_name}_32x32_rgba.bin")}')

for img in ['close_eye', 'open_eye']:
    img2rgba(img)