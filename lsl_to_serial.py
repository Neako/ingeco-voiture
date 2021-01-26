"""
>>> python lsl_to_serial.py -sp '/dev/cu.usbmodem146101' -thr 200 200 -td 500 -sd 200
"""

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import os, sys, re
import time
import argparse

from pylsl import StreamInlet, resolve_stream
import serial
from scipy.fftpack import fftfreq, irfft, rfft
from collections import deque

def argparser():
    """Creating arparse.ArgumentParser and returning arguments
    """
    argparser = argparse.ArgumentParser(description='Use BioBert to predict Litcovid categories.', formatter_class=argparse.RawTextHelpFormatter)
    argparser.add_argument('--serial', '-sp', type=str, required=True, default='/dev/cu.usbmodem146101', help="Port to use to communicate with Arduino.")
    argparser.add_argument('--mode', '-m', choices=['emg', 'alphabeta'], default='emg', help="Port to use to communicate with Arduino.")
    argparser.add_argument('--nbsamples', '-n', type=int, default=20, help="Number of samples for fft.")
    argparser.add_argument('--lefteye', '-l', type=int, default=0, help="Electrode for left eye.")
    argparser.add_argument('--righteye', '-r', type=int, default=1, help="Electrode for right eye.")
    argparser.add_argument('--threshold', '-thr', type=int, nargs=2, default=[200,200], help="Threshold for each eye.")
    argparser.add_argument('--turndelay', '-td', type=int, default=500, help="Duration of turn.")
    argparser.add_argument('--stopdelay', '-sd', type=int, default=200, help="Duration of turn.")
    
    args = argparser.parse_args()
    return args

def parse_alphabeta():
    return None

def filter_emg(s:list, fs:int = 250, cut_bounds:tuple = (7,13)):
    W = fftfreq(len(s), d=(1/fs))
    f_signal = rfft(s)

    cut_f_signal = f_signal.copy()
    cut_f_signal[(np.abs(W)>cut_bounds[1])] = 0 # cut signal above XX Hz
    cut_f_signal[(np.abs(W)<cut_bounds[0])] = 0 # cut signal under XX Hz
    cs = irfft(cut_f_signal)

    return cs
    
def adapt_emg(left_blink:bool, right_blink:bool, prev_posting, prev_time, turn_delay:int, stop_delay:int):
    #### Initial value
    if left_blink and right_blink:
        posting = b'0,0\n'
    elif left_blink:
        posting = b'1,0\n'
    elif right_blink:
        posting = b'0,1\n'
    else:
        posting = b'1,1\n'

    #### Adapt with duration / prev value
    # If turning, do not stop until timer end
    if (prev_posting in [b'1,0\n', b'0,1\n']) and (posting == b'1,1\n'):
        if time.time() - prev_time <= turn_delay: 
            posting = prev_posting # if should go forward but still too close from turn, keep turn

    # If turning, adapt if detect a blink in the other eye before the end of timing
    elif prev_posting in [b'1,0\n', b'0,1\n'] and (posting in [b'1,0\n', b'0,1\n'] and prev_posting != posting):
        if (time.time() - prev_time <= stop_delay): 
            posting = b'0,0\n' # short delay between both eyes but stop
    
    # If restarting from stop, start with going forward
    if (prev_posting == b'0,0\n'):
        if (posting == b'1,1\n'):
            posting = b'0,0\n' # need movement in eyes to restart
        elif (time.time() - prev_time > stop_delay):
            if posting == b'0,0\n':
                posting = b'1,1\n'git 
            #else: # only blinks since '1,1' condition is dealt with before 
        else: # timer not reached
            posting = b'0,0\n'

    return posting


if __name__ == '__main__':
    args = argparser()
    print(args)

    # Start Serial stream
    ser = serial.Serial(args.serial, 9600)

    # Start LSL Stream
    streams = {stream.name(): stream for stream in resolve_stream('type', 'EEG') }
    fft = StreamInlet(streams['obci_eeg1'])
    fft_data = deque([], maxlen=20)

    # Check new data
    # moves = [b'1,1\n', b'0,0\n', b'0,1\n', b'1,0\n']
    posting = b'0,0\n'
    start_turn = None
    while True:
        prev_posting = posting
        sample, _ = fft.pull_sample()
        fft_data.append(sample)
        an_data = np.array(fft_data)

        if len(fft_data) < args.nbsamples:
            posting = b'1,1\n'
        else:
            left_blink = (max(filter_emg(an_data[:,args.lefteye])) > args.threshold[0])
            right_blink = (max(filter_emg(an_data[:,args.righteye])) > args.threshold[1])
        
        posting = adapt_emg(left_blink, right_blink, prev_posting, start_turn, args.turndelay, args.stopdelay)   
        if prev_posting != posting:
            start_turn = time.time()
            ser.write(posting)  
            print(f"Command: {posting}")
    
    # Close Serial stream
    ser.close()   