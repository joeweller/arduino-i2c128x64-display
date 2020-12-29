import serial
from time import sleep

SERIAL_DEVICE = 'COM3'
FRAME_DELAY = 5

def main():

    ser = serial.Serial(SERIAL_DEVICE, 9600)

    if (ser.isOpen()):
        print(f'serial started on: {ser.name}')
        sleep(5)  # allows arduino to init
        framecount = 0  # counter that
    else:
        print('unable to open serial')

    # main control loop that will switch through different screens
    while ser.isOpen():
        framecount+=1
        currframe = framecount
        
        # flush serial buffers
        ser.flushInput()
        ser.flushOutput()

        if 1 == framecount:
            ser.write('hello\n'.encode())
        elif 2 == framecount:
            ser.write('world\n'.encode())
        elif 3 == framecount:
            ser.write('!\n'.encode())
            framecount = 0
        
        print(f'framecount: {currframe} {(ser.readline()).decode()}', end='\n')
        sleep(FRAME_DELAY)

    ser.close()


if __name__ == "__main__":
    main()