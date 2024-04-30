# -*- coding: utf-8 -*-
"""
Created on Wed Oct 20 09:16:35 2021

@author: Albano Peñalva
"""

# Librerías
from scipy import signal
from scipy.io import wavfile
import numpy as np
import matplotlib.pyplot as plt
# from funciones_fft import fft_mag

plt.close('all') # cerrar gráficas anteriores

# %% Lectura del archivo de audio 
filename = 'Mariposa tecknicolor.wav'          # nombre de archivo
fs, data = wavfile.read(filename)   # frecuencia de muestreo y datos de la señal

# Definición de parámetro temporales
ts = 1 / fs                     # tiempo de muestreo
N = len(data)                   # número de muestras en el archivo de audio
t = np.linspace(0, N * ts, N)   # vector de tiempo
senial = data[:, 0]            # se extrae un canal de la pista de audio (si el audio es estereo)

T_START = 0     # Tiempo de inicio en segundos
T_END = 58      # Tiempo de fin en segundos

# Se crea una gráfica que contendrá dos sub-gráficos ordenados en una fila y dos columnas
fig1, ax1 = plt.subplots(1, 1, figsize=(20, 10))
# Se grafica la señal temporal
ax1.plot(t, senial)
ax1.set_xlabel('Tiempo [s]', fontsize=15)
ax1.set_ylabel('Tensión [V]', fontsize=15)
ax1.set_title('Señal temporal', fontsize=15)
ax1.set_xlim([T_START, T_END])
ax1.grid()

# Recorte
INICIO = int(T_START * fs) 
FINAL = int(T_END * fs)
senial_rec = senial[INICIO:FINAL]
N_rec = len(senial_rec)
 
# Submuestreo
F_SUB = 8000
senial_submuest = signal.resample(senial_rec, int(N_rec*F_SUB/fs))

# Escalado (para DAC de 8 bits)
senial_esc = senial_submuest / np.max(np.abs(senial_submuest))
senial_esc = (senial_esc * (127) + 128).astype(np.int16)
N_esc = len(senial_esc)

# Guardado en archivo .h
np.savetxt("song.h", senial_esc, newline=',', 
            header=f'#define N_SONG {N_esc}\n#define SONG_NAME "Mariposa Teknicolor"\n#define SONG_ARTIST "Fito Paez"\nconst uint8_t song[] ='+' {128',
            footer='};//',
            comments='',
            )