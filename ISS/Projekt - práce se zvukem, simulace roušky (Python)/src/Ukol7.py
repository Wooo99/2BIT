#!/usr/bin/env python3
#---------------------------------------------UKOL 7
#Michal Reznik
#xrezni28
import copy
import cmath
import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf
import IPython
from scipy.signal import spectrogram, lfilter, freqz, tf2zpk
from scipy.ndimage.interpolation import shift


def framing(signal,arr):
    for k in range (0,expnumframes):
        for i in range (0,framesize):
            if((k*overlap+i) >= 16000):             #16000 = size in samples for 1 sec
                arr[k][i] = 0
            else:
                arr[k][i]= signal[k*overlap+i]

pass

def centralclipping(arr):
    finish = np.ndarray(framesize)
    for i in range (0,framesize):
        if(arr[i] > 0.7*max(abs(arr))):
            finish[i] = '1'
        elif(arr[i] < -0.7*max(abs(arr))):
            finish[i] = '-1'
        else:
            finish[i] = '0'
    return finish

def scalar(arr1,arr2):
    resultscalar = 0
    for i in range(0,framesize):
        result_temp = arr1[i] * arr2[i]
        resultscalar += result_temp
    return resultscalar

def autocorelation(arr):
    arr1 = arr
    corelated = np.ndarray(framesize)
    for i  in range (0,framesize):
        corelated[i] = scalar(arr1,arr)
        arr1 =shift(arr1, 1, cval = 0.0)
    return corelated

def Border500Check(Koefarr):
    if(np.argmax(Koefarr) == 0): 
        Koefarr[np.argmax(Koefarr)] = 0
    while 1:
        index =np.argmax(Koefarr)
        p = fs*1/index
        if(p > 500):
            Koefarr[index] = 0
        else:
            break
pass

def dft(InputArray):
    Result = []
    N = len(InputArray)
    for k in range(0, N):
	    Result.append(np.complex(0, 0))
	    for n in range(0, N):
		    Result[k] += InputArray[n] * np.exp(np.complex(0, -2*np.pi*k*n/N))
    return Result

def idft(InputArray):
    Result = []
    N = len(InputArray)
    for k in range(0, N):
	    Result.append(np.complex(0, 0))
	    for n in range(0, N):
		    Result[k] += InputArray[n] * np.exp(np.complex(0, 2*np.pi*k*n/N))

    newList = [x / N for x in Result]
    return newList

s1, fs = sf.read('../audio/Maskon_tone.wav')
s2, fs = sf.read('../audio/Maskoff_tone.wav')

#MOST SIMILAR SECOND
s1 = copy.deepcopy(s1[12000 :28000])      
s2 = copy.deepcopy(s2[11000:27000])

#CENTR
s1-= np.mean(s1)
s2-= np.mean(s2)

#NORMALIZATION
s1/=abs(s1).max()
s2/=abs(s1).max()

#SET SIZES
overlap = 160           #10ms
framesize = 320         #20ms
expnumframes = 99     # expected size of frames in 1 sec

#SET FRAMES 2DARRAY
FramesMask = np.ndarray((expnumframes, framesize))
FramesNoMask = np.ndarray((expnumframes, framesize))

#SET FRAMES 2DARRAY
framing(s1,FramesMask)  
framing(s2, FramesNoMask)


DFTWithMask = np.ndarray((expnumframes, 1024))
DFTWithoutMask = np.ndarray((expnumframes, 1024))
MaskOff = []
MaskOn = []
DFTMaskoOff = []
DftMaskOn = []

for r in range (0,expnumframes):
    MaskOff.append(np.fft.fft(FramesNoMask[r], n = 1024))
    MaskOn.append(np.fft.fft(FramesMask[r], n = 1024))

for i in range (0, expnumframes):
    DftMaskOn.append(10*np.log10(np.abs(MaskOn[i])**2))
    DFTMaskoOff.append(10*np.log10(np.abs(MaskOff[i])**2))

Hejw = np.divide(DftMaskOn,DFTMaskoOff)

Hejwmean = np.ndarray(1024)
for i in range(1023):
    temp = 0
    for p in range (0,98):
       temp  += np.abs(Hejw[p][i])
    temp /= expnumframes
    Hejwmean[i] = temp

 
Latency = np.fft.ifft(Hejwmean)
 

plt.gca().set_title("Impulzivn?? odezva rou??ky") 
#plt.gca().set_xlabel("Cas[s]")
#plt.gca().set_ylabel("Frekvence")
plt.plot(Latency, "-b",label = "With Mask")

plt.show()