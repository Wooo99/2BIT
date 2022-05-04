
#---------------------------------------------UKOL 4
#Michal Reznik
#xrezni28

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



s1, fs = sf.read('../audio/Maskon_tone.wav')
s2, fs = sf.read('../audio/Maskoff_tone.wav')

#MOST SIMILAR SECOND
s1 = s1[12000 :28000]        
s2 = s2[11000:27000]

#CENTR
s1-= np.mean(s1)
s2-= np.mean(s2)

#NORMALIZATION
s1/=abs(s1).max()
s2/=abs(s1).max()

#SET SIZES
overlap = 160           #10ms
framesize = 320         #20ms
expnumframes = 99      # expected size of frames in 1 sec

#SET FRAMES 2DARRAY
FramesMask = np.ndarray((expnumframes, framesize))
FramesNoMask = np.ndarray((expnumframes, framesize))

#SET FRAMES 2DARRAY
framing(s1,FramesMask)  
framing(s2, FramesNoMask)
clipped = np.ndarray(framesize)

F0With = np.ndarray(expnumframes)
F0Without = np.ndarray(expnumframes)

#SET KOEFECIENTS GRAPH FOR
for i in range (0,expnumframes):
     clipped = centralclipping(FramesNoMask[i])
     autocorelated = autocorelation(clipped)
     Border500Check(autocorelated)
     F0With[i] = fs *1/(np.argmax(autocorelated))

for i in range (0,expnumframes):
    clipped = centralclipping(FramesMask[i])
    autocorelated = autocorelation(clipped)
    Border500Check(autocorelated)
    F0Without[i] = fs *1/(np.argmax(autocorelated))

print(" stredni hodnota je s rouskou", np.mean(F0With))
print(" stredni hodnota je bez rousky", np.mean(F0Without))

print("rozptyl s rouskou je", np.var(F0With))
print("rozptyl bez rousky je", np.var(F0Without))

#clipped = centralclipping(FramesNoMask[0])
#autocorelated = autocorelation(clipped)
#Border500Check(autocorelated)
#plt.axvline(x = fs/500, color = 'k', label = "Prah")
#x = [np.argmax(autocorelated)]
#y = [autocorelated[x]]

#SETUP PRO GRAF  
plt.plot(F0Without, "-r",label = "Bez masky")
#plt.plot(autocorelated, "-r",label = "Rámec")
plt.plot(F0With, "-b",label = "S Maskou")
#plt.gca().set_title("Autokorelace") 
#plt.stem(x,y,"black" , label = "Lag")
plt.legend(loc = 'upper left')

plt.show()