import serial
from frame.frame import Frame
from time import sleep

SERIAL_DEVICE = 'COM3'
FRAME_DELAY = 2


def main():

    framecount = 0  # frame counter
    ser = serial.Serial(SERIAL_DEVICE, 9600)

    if (ser.isOpen()):
        print(f'serial started on: {ser.name}')
        sleep(5)  # allow arduino to initialise
    else:
        print('unable to open serial')

    # main control loop that will switch through different screens
    while ser.isOpen():
        framecount+=1
        currframe = framecount
        
        frame = Frame()
        
        # flush serial buffers
        ser.flushInput()
        ser.flushOutput()

        if 1 == framecount:

            frame.init_line(1, 1)
            frame.line(1).setcolumntext(1, 'test')

            frame.init_line(2, 2)
            frame.line(2).setcolumntext(1, 'test', 'center')
            frame.line(2).setcolumntext(2, 'test', 'center')
            
            frame.init_line(3, 3)
            frame.line(3).setcolumntext(1, 'test', 'left')
            frame.line(3).setcolumntext(2, 'test', 'center')
            frame.line(3).setcolumntext(3, 'test', 'right')
            
            frame.init_line(4, 1)
            frame.line(4).setcolumntext(1, 'test', 'center')

            frame.init_line(5, 2)
            frame.line(5).setcolumntext(1, 'test', 'center')
            frame.line(5).setcolumntext(2, 'test', 'center')

            print(frame.getstring())
            print(frame.getstring_encoded())
            print(len(frame.getstring()))
            ser.write(frame.getstring_encoded())
            framecount = 0
        print(f'framecount: {currframe} {(ser.readline()).decode()}', end='\n')
        del frame
        sleep(FRAME_DELAY)

    ser.close()


if __name__ == "__main__":
    main()