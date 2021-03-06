import numpy as np
import matplotlib.pyplot as plt
from scipy import fftpack
from scipy.io import wavfile
import serial

def wav_to_freq(rate, audio):   # Wav 파일에서 읽어온 rate, audio 값으로 해당 노래의 frequency list를 return

    idx = []  # audio 파일에서 각 음이 시작되는 index를 저장하기 위한 list
    song = []  # 음악 전체 frequency 저장 하는 list

    i = 0
    while (i < audio.shape[0]):
        if abs(audio[i]) > 500:  # audio 파일의 값이 500 이상일 때 해당 index-2000을 해당 음의 시작 index로 저장 - Backoff 대략 2000 sample
            idx.append(i - 2000)
            i = i + 23000  # 한 음을 대략 25000 sample로 잡았음. - Piano 한음 with no sustain 길이
        i = i + 1

    for start in idx:
        end = start + 25000
        if start + 25000 > audio.shape[0]:
            end = audio.shape[0]

        N = end - start  # FFT할 한 음의 길이
        L = N / rate  # 한음당 시간
        f_s = rate  # Sampling rate - 1초에 몇번 sample 하였는지

        X = fftpack.fft(audio[start:end])
        freqs = fftpack.fftfreq(N) * f_s
        dom_freq = int(freqs[np.argmax(np.abs(X[:]))])
        song.append(dom_freq)

        start = start / rate    # 초 단위로 변환
        print('%5.2fs,  ' % start, 'frequency: ', dom_freq, ', duration: ', L)

    return song


def freq_mapper(song):  # fft로 추출한 frequency 값을 arduino가 원하는 값으로 변환
    freq_mapped_song = []  # 주파수로 저장
    note_mapped_song = []  # list of notes in 'char' data type

    for freq in song:
        if (250 < freq < 270) or (510 < freq < 530):  # C4
            freq_mapped_song.append(262)
            note_mapped_song.append('C')
        elif (280 < freq < 300) or (570 < freq < 600):  # D4
            freq_mapped_song.append(294)
            note_mapped_song.append('D')
        elif (310 < freq < 339) or (640 < freq < 670):  # E4
            freq_mapped_song.append(330)
            note_mapped_song.append('E')
        elif (340 < freq < 359) or (680 < freq < 710):  # F4
            freq_mapped_song.append(349)
            note_mapped_song.append('F')
        elif (380 < freq < 400) or (770 < freq < 800):  # G4
            freq_mapped_song.append(392)
            note_mapped_song.append('G')
        elif (430 < freq < 450) or (870 < freq < 890):  # A4
            freq_mapped_song.append(440)
            note_mapped_song.append('A')
        elif (480 < freq < 503) or (970 < freq < 1000):  # B4
            freq_mapped_song.append(494)
            note_mapped_song.append('B')
        # else:
        #     freq_mapped_song.append(-1)
        #     note_mapped_song.append(0)

    return freq_mapped_song, note_mapped_song


rate, audio = wavfile.read('./test.wav')   # Read wav file
# test.wav는 "반짝반짝 작은별" 비교적 빠른 템포로 친 것. 이거 바탕으로 자르는 값 수정 했더니 빠른 템포도 잘됨-마지막에 잡음 있음
# 한음을 대략 25000샘플 - 0.5초로 자름. 자른 지점부터 다음 음 사이는 소리 크기(amplitude)가 500 이하여야 됨.
# 한음이 0.5초 이상 500이상의 크기로 지속되면 값 이상하게 나올 수도.
# https://www.onlinepianist.com/virtual-piano 여기서 sustain 끄고 한음 짧게 짧게 쳐서 녹음

plt.plot(audio)       # Audio 파일 Time-Amplitude 그래프
plt.show()

music_frq = wav_to_freq(rate, audio)
freq_list, note_list = freq_mapper(music_frq)
print(freq_list)
print(note_list)


# pyserial 설치 필요 (Terminal에서 pip3 install pyserial)
# 파이썬에서 실행할 경우, 파이썬 코드를 Run하지 않는 상황에서 아두이노 코드를 먼저 Upload한 후, 파이썬에서 Run해야 실행된다.
# PORT number만 확인 후 수정 필요 (Arduino -> Tools -> Port)

# Set a PORT Number & baud rate
PORT = 'COM4'                       # 아두이노에서 포트 number 확인 후 필요 시 수정
BaudRate = 9600                     # BaudRate = n | Serial.begin(n)
ARD = serial.Serial(PORT,BaudRate)

note_string = ''.join([str(item) for item in note_list])
note_string = note_string.encode('utf-8')

while (True):
    ARD.write(note_string)
