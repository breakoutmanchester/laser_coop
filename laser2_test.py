import os
import pygame
from pygame import mixer
from pygame.locals import *
##from gtts import gTTS
##from playsound import playsound
import serial.tools.list_ports
import time
import random
import serial
##import pyttsx3



found = False
BLACK = (0, 0, 0)
WHITE = (255,255,255)
X = 7000
Y = 900

#number of sound effects in folder
numOfSoundEffects = 4

pygame.init()

# find all ports and select arduino

while(found == False):
    ports = serial.tools.list_ports.comports(include_links=False)
    for port, desc, _ in sorted(ports):
            if(str(desc).__contains__("Arduino") or str(desc).__contains__("arduino")):
                _serial = serial.Serial(port=str(port), baudrate=9600)
                print("Found:", port)
                found = True

# initialise and play the music

screen = pygame.display.set_mode((X, Y))

#set the font for the text
sysfont = pygame.font.get_default_font()

font = pygame.font.SysFont(None, 240)

count = 1

img = font.render("", True, WHITE)
text_rect = img.get_rect(center=(screen.get_rect().center))
screen.blit(img, text_rect)

score = 0
running = True
background = BLACK

prev_res = ""


while running:
    score += 1
    for event in pygame.event.get():
        if event.type == QUIT:
            running = False
   
#------reads from serial-----------
    while _serial.in_waiting:
        line = str(_serial.readline()).replace('b', "").replace('\\r', "").replace('\\n', "").replace('\'', "")

        if not ((line == "") or (line == " ")) and not(prev_res == line) :
            img = font.render(line, True, WHITE)
            text_rect = img.get_rect(center=(screen.get_rect().center))
            screen.fill(background)
            screen.blit(img, text_rect)
            prev_res = line
            pygame.display.update()
        elif((line == "") or (line == " ") and prev_res == line):
            img = font.render("All good :)", True, WHITE)
            text_rect = img.get_rect(center=(screen.get_rect().center))
            screen.fill(background)
            screen.blit(img, text_rect)
            prev_res = line
            pygame.display.update()  

try:
    while True:
        pygame.display.update()
except KeyboardInterrupt:
    pass
