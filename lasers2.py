import pygame
import serial
import time
from pynput.mouse import Button, Controller
import datetime
import shutil
import mysql.connector
from mysql.connector import Error
from moviepy.editor import VideoFileClip
mode = ""
# SQL Configuration 
config = { 
  "host":" 192.168.1.208",
  "port":"3333",
  "user":"root",
  "password":"pass",
  "database": "scores",
}

##### uncomment for  RFID
teamName = ""

mouse = Controller()

pygame.init()

# Set up the screen
screen_width, screen_height = 1920, 1080
screen = pygame.display.set_mode((screen_width, screen_height))
pygame.display.set_caption("Scan Card")

background_image = pygame.image.load("cp_logo.jpeg")
background_image = pygame.transform.scale(background_image, (screen_width, screen_height))

# Font for rendering text
font_big = pygame.font.Font("C:/Users/break/Desktop/lasers2/Lasers2/Super_Creamy.ttf", 150)
font_small = pygame.font.Font("C:/Users/break/Desktop/lasers2/Lasers2/Super_Creamy.ttf", 100)

# Function to display text at the center of the screen
def draw_text_center(text, font, color, y_offset=0):
    text_surface = font.render(text, True, color)
    text_rect = text_surface.get_rect(center=(screen.get_width() // 2, screen.get_height() // 2 + y_offset))
    screen.blit(text_surface, text_rect)

# Main loop
scanned_card_data = None
running = True
input_text = ''
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if scanned_card_data is None:
                if event.key == pygame.K_RETURN:
                    scanned_card_data = input_text
                    input_text = ''
                elif event.key == pygame.K_BACKSPACE:
                    input_text = input_text[:-1]
                else:
                    input_text += event.unicode

    # Clear the screen
    screen.blit(background_image, (0, 0))

    #mouse.click(Button.left, 1)

    # Display message
    if scanned_card_data is None:
        draw_text_center("Please scan your card", font_small, (255, 255, 255))
    else:
        try:
            with mysql.connector.connect(**config) as db:
                cursor = db.cursor()

                cursor.execute(f"SELECT * FROM scores.teams where CardID = '{scanned_card_data}';")

                result = cursor.fetchone()

                if(result is not None):
                    draw_text_center(f"Welcome", font_small, (255, 255, 255), y_offset=-75)
                    teamName = result[2]
                    mode = result[14]
                    print("mode", mode)
                    draw_text_center(f"{teamName}", font_big, (255, 255, 173), y_offset=50)
                    pygame.display.flip()
                    time.sleep(4)
                    running = False
                else:
                    draw_text_center(f"Invalid card!", font_small, (255, 51, 51))
                    scanned_card_data = None
                    pygame.display.flip()
                    time.sleep(2)
        except Exception as ex:
            print(ex)
    pygame.display.flip()
    


print("Script is now running...")




# Play the video before game starts
clip = VideoFileClip("lasers2.mp4")
clip_resized = clip.resize(height=1080)  # You can set the desired height. The width will be adjusted automatically to keep the aspect ratio.
pygame.quit()
clip_resized.preview()

# Initialize Pygame and the font
pygame.init()
pygame.font.init()
myfont = pygame.font.SysFont('Calibri Bold', 200)
myfont2 = pygame.font.SysFont('Calibri Bold', 120)
screen = pygame.display.set_mode((1920, 1080))

# Initialize the serial port
ser = serial.Serial('COM3', 9600, timeout=1)

# Initialize the sounds
correct_sound = pygame.mixer.Sound("correct.mp3")
incorrect_sound = pygame.mixer.Sound("incorrect.mp3")
soundtrack_sound = pygame.mixer.Sound("soundtrack.mp3")
halloweensoundtrack_sound = pygame.mixer.Sound("saw.mp3")
airhorn_sound = pygame.mixer.Sound("airhorn.mp3")

# Score and timer variables
score = 0
start_time = pygame.time.get_ticks()
time_limit = 5 * 60 * 1000  # 5 minutes in milliseconds
message_display_time = 5000  # Adjusted to 5 seconds
message_to_display = ""
show_final_score = False

airhorn_sound.play()
time.sleep(1)
if mode.lower() != 'y' or mode.lower() != 'yes':
    soundtrack_sound.play()
else:
    halloweensoundtrack_sound.play()

def update_DB(score):
    retry = 0
    numofretry = 3
    # while retry < numofretry:
    #     with mysql.connector.connect(**config) as db:
    #         cursor = db.cursor()
    #         cursor.execute(f"UPDATE scores.teams SET Laser2 = '{score}' WHERE TeamName = '{teamName}'")
    #         print(f"UPDATE scores.teams SET Laser2 = '{score}' WHERE TeamName = '{teamName}'")
    #         db.commit()
    #         print(cursor.rowcount, "record(s) affected")
    #         retry = retry + 1
    #         time.sleep(0.5)
    #     # cursor.execute("SELECT * FROM scores.teams")
    #     # data = list(cursor.fetchall())
    #     cursor.close()


    while retry < numofretry:
        try:
            with mysql.connector.connect(**config) as db:
                cursor = db.cursor()
                query = "UPDATE scores.teams SET Laser2 = %s WHERE TeamName = %s"
                values = (score, teamName)
                cursor.execute(query, values)
                print(cursor.statement)
                db.commit()

                print(cursor.rowcount, "record(s) affected")

                time.sleep(0.5)

        except Error as e:
            print(f"Error: {e}")
            if db.is_connected():
                db.rollback()  
        
        finally:
            retry += 1
            if db.is_connected():
                cursor.close()
                db.close()


def save_score(score):
    # Get the current date and time
    current_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    # Append the score and current time to the file
    with open('scores.txt', 'a') as file:
        file.write(f"{str(current_time)} Name: {teamName} - Score: {score}\n")
    file.close()

def save_error(err):
    # Get the current date and time
    current_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')

    # Append the score and current time to the file
    with open('log.txt', 'a') as file:
        file.write(f"{str(current_time)} - {err}\n")
    file.close()


# Main loop
running = True
extra_time = 0

while running:
    # Event handling

    # Check for serial messages
    if ser.in_waiting > 0:
        line = ser.readline().decode('utf-8').strip()
        if line == "COMPLETE":
            score += 1
            correct_sound.play()
        elif line == "INCOMPLETE":
            message_to_display = "Return to the start"
            incorrect_sound.play()
            # Capture the current time to manage message display duration
            message_display_start = pygame.time.get_ticks()

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = True
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if(event.button == 1): #left click (add time)
                extra_time += 60000
            elif(event.button == 3): #right click (deduct time)
                extra_time -= 60000

    # Timer
    current_time = pygame.time.get_ticks()
    elapsed_time = current_time - start_time - extra_time
    time_left = max(0, time_limit - elapsed_time)
    if time_left <= 0:
        running = False
        show_final_score = True

    # Drawing
    screen.fill((0, 0, 0))  # Clear the screen
    
    if message_to_display:
        if current_time - message_display_start < message_display_time:
            # Draw the "Return to the start" message
            message_text = myfont.render(message_to_display, True, (255, 0, 0))
            message_rect = message_text.get_rect(center=(960, 540))
            screen.blit(message_text, message_rect)
        else:
            message_to_display = ""  # Reset message after display duration
    else:
        # Draw the score when no message is being displayed
        score_text = myfont.render(f"Score: {score}", True, (255, 255, 255))
        score_rect = score_text.get_rect(center=(960, 540))
        screen.blit(score_text, score_rect)

    # Always draw the timer
    minutes = time_left // 60000
    seconds = (time_left % 60000) // 1000
    timer_text = myfont2.render(f"{minutes:02}:{seconds:02}", True, (255, 255, 255))
    timer_rect = timer_text.get_rect(topright=(1880, 20))
    screen.blit(timer_text, timer_rect)

    pygame.display.flip()  # Update the screen

    # Cap the frame rate
    pygame.time.Clock().tick(60)

if show_final_score:
    # Final score display after the main loop ends
    airhorn_sound.play()
    screen.fill((0, 0, 0))
    final_score_text = myfont.render(f"Final Score: {score}", True, (255, 255, 255))
    timer = myfont2.render(f"Return to the studio!", True, (255,0,0))
    timer_rect = timer.get_rect(center=(1920 // 2, 1080 // 1.5))
    screen.blit(timer, timer_rect)
    try:
        save_score(score)
    except Exception as e:
        print(e)

    try:
        update_DB(score)
    except Exception as e:
        save_error(e)

    final_score_rect = final_score_text.get_rect(center=(960, 540))
    screen.blit(final_score_text, final_score_rect)
    pygame.display.flip()

    # Wait for a few seconds before exiting
    pygame.time.wait(1000000)



# Clean up
ser.close()
pygame.quit()
