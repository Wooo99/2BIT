
#---------------------------------------------UKOL 3
#Michal Reznik
#xrezni28

import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf
import IPython
from scipy.signal import spectrogram


def framing(signal,table):
    for k in range (0,expnumframes-1):
        for i in range (0,framesize):
            if((k*overlap+i) >= 16000):             #16000 = size in samples for 1 sec
                table[k][i] = 0
            else:
                table[k][i]= signal[k*overlap+i]

    pass

s1, fs = sf.read('../audio/Maskon_tone.wav')
s2, fs = sf.read('../audio/Maskoff_tone.wav')

s1 = s1[11000 :27000]           #nastaveni nejvice podobne vteriny
s2 = s2[9750:25750]

#USTREDNENI
s1-= np.mean(s1)
s2-= np.mean(s2)
#NORMALIZACE
s1/=abs(s1).max()
s2/=abs(s1).max()
#NASTAVENI RAMCU
overlap = 160   #10ms
framesize = 320  #20ms
expnumframes = 100      # expected size of frames in 1 sec
framesMask = np.ndarray((expnumframes, framesize))
framesNoMask = np.ndarray((expnumframes, framesize))

framing(s1,framesMask)  
framing(s2, framesNoMask)

plt.plot(framesMask[0], "-b",label = "With Mask")
plt.plot(framesNoMask[0], "-r", label = "No Mask")

#SETUP PRO GRAF
plt.gca().set_title('Porovnani vybraneho framu') 
plt.legend()

plt.show()